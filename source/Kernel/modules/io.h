#ifndef IO_H
#define IO_H

// kernel print formatted (almost)
void kprintf(const unsigned char* str, ...)
{
    static       unsigned short* vmem = (unsigned short*)0xFFFFFF80000B8000; // canonical form of VGA mem
    static const unsigned short  ref  = 7936; // 0x1F00

    static const int32_t VGA_WIDTH  = 80;
    static const int32_t VGA_HEIGHT = 25;
    
    static int32_t current_x = 0;
    static int32_t current_y = 0;

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
                    vmem[position] = (ref | s[j]);
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
        } else
        {
            int32_t position = current_y * VGA_WIDTH + current_x;

            vmem[position] = (ref | str[i]);

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
                    vmem[y * VGA_WIDTH + x] = vmem[(y + 1) * VGA_WIDTH + x];
                }
            }

            for (int x = 0; x < VGA_WIDTH; x++)
            {
                vmem[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ref | ' ';
            }

            current_y = VGA_HEIGHT - 1;
        }
    }
    
    va_end(args);
}

#endif
