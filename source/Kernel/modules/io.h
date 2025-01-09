#ifndef IO_H
#define IO_H

// kernel print not formatted (almost)
void kprintnf(unsigned char* str)
{
    static       unsigned short* vmem = (unsigned short*)0xFFFFFF80000B8000; // canonical form of VGA mem
    static const unsigned short  ref  = 0x1F00; // white on blue

    static const int32_t VGA_WIDTH  = 80;
    static const int32_t VGA_HEIGHT = 25;
    // standart VGA
    
    static int32_t current_x = 0;
    static int32_t current_y = 0;

    for (int32_t i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\n')
        {
            current_x = 0;
            current_y++;

            continue;
        }

        int32_t position = current_y * VGA_WIDTH + current_x;

        vmem[position] = (ref | str[i]);

        current_x++;

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
}

#endif