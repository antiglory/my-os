#ifndef TTY_H
#define TTY_H

#define STDIN_BUFF_SIZE VGA_HEIGHT * VGA_WIDTH

unsigned char stdin_buffer[STDIN_BUFF_SIZE] = {0};
bool          stdin_listen                  = false;

void clear_stdin_buffer(void)
{
    memset(stdin_buffer, 0, STDIN_BUFF_SIZE);
}

void gets(unsigned char* str)
{
    // new read
    clear_stdin_buffer();
    stdin_listen = true;

    while (true)
    {
        volatile char* c = &stdin_buffer[0];
        bool newline_found = false;

        while (*c != '\0')
        {
            if (*c == '\n')
            {
                newline_found = true;
                break;
            }
            c++;
        }

        if (newline_found)
        {
            break;
        }

        asm volatile("hlt\n\t");
    }

    stdin_listen = false;

    size_t i = 0;

    while (stdin_buffer[i] != '\n')
    {
        str[i] = stdin_buffer[i];
        i++;
    }

    str[i] = '\0';
}

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

static int32_t current_x = 0;
static int32_t current_y = 0;

static unsigned short* vga = (unsigned short*)0xFFFFFF80000B8000;

void clear(void)
{
    for (int i = 0; i < VGA_HEIGHT * VGA_HEIGHT; i++)
    {
        vga[i] = (short)0x0720;
    }

    current_x = 0;
    current_y = 0;
}

// kernel print formatted (almost)
void kprintf(const unsigned char* str, ...)
{
    static const unsigned short  ref  = 7936; // 0x1F00

    va_list args;
    va_start(args, str);

    for (int32_t i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\n')
        {
            current_x = 0;
            current_y++;
        }
        else if (str[i] == '%')
        {
            i++;

            unsigned char format = str[i];

            void print_string(const char* s)
            {
                for (int j = 0; s[j] != '\0'; j++)
                {
                    int32_t position = current_y * VGA_WIDTH + current_x;

                    vga[position] = (ref | s[j]);

                    current_x++;

                    if (current_x >= VGA_WIDTH)
                    {
                        current_x = 0;
                        current_y++;
                    }
                }
            }
            
            if (format == 'p') // pointer
            {
                uintptr_t ptr_val = va_arg(args, uintptr_t);
                
                unsigned char hex_str[19];
                hex_str[0] = '0';
                hex_str[1] = 'x';
                
                for (int j = 0; j < 16; j++)
                {
                    int32_t nibble = (ptr_val >> (60 - j * 4)) & 0xF;

                    if (nibble < 10)
                        hex_str[j + 2] = '0' + nibble;
                    else
                        hex_str[j + 2] = 'a' + (nibble - 10);
                }

                hex_str[18] = '\0';

                print_string(hex_str);
            }
            else if (format == 'x') // hex
            {
                unsigned int int_val = va_arg(args, unsigned int);
                unsigned char hex_str[9];
                
                for (int j = 0; j < 8; j++)
                {
                    int32_t nibble = (int_val >> (28 - j * 4)) & 0xF;

                    if (nibble < 10)
                        hex_str[j] = '0' + nibble;
                    else
                        hex_str[j] = 'a' + (nibble - 10);
                }

                hex_str[8] = '\0';

                print_string(hex_str);
            }
            else if (format == 'c') // char
            {
                unsigned char c = (unsigned char)va_arg(args, int);
                unsigned char tmp_str[2];

                tmp_str[0] = c;
                tmp_str[1] = '\0';

                print_string((const char*)tmp_str);
            }
            else if (format == 's') // string
            {
                const char* s = va_arg(args, const char*);

                print_string(s);
            }
        }
        else
        {
            int32_t position = current_y * VGA_WIDTH + current_x;

            vga[position] = (ref | str[i]);

            current_x++;
        }

        if (current_x >= VGA_WIDTH)
        {
            current_x = 0;
            current_y++;
        }

        if (current_y >= VGA_HEIGHT)
        {
            for (int y = 0; y < VGA_HEIGHT - 1; y++)
            {
                for (int x = 0; x < VGA_WIDTH; x++)
                {
                    vga[y * VGA_WIDTH + x] = vga[(y + 1) * VGA_WIDTH + x];
                }
            }

            for (int x = 0; x < VGA_WIDTH; x++)
            {
                vga[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ref | ' ';
            }

            current_y = VGA_HEIGHT - 1;
        }
    }
    
    va_end(args);
}

#endif
