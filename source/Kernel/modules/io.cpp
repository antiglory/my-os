void clear_screen(void)
{
    uint16_t* video_memory = (uint16_t*)0xB8000;
    uint16_t blank = 0x20 | (0x07 << 8);

    for (int i = 0; i < 80 * 25; i++) {
        video_memory[i] = blank;
    }
}

void pkprintnf(const char* string) {
    uint16_t* video_memory = (uint16_t*)0xB8000;
    uint16_t color = 0x07 << 8;
    int index = 0;

    while (string[index] != '\0') {
        video_memory[index] = (string[index] & 0xFF) | color;
        index++;
    }

    return;
}