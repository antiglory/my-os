#ifndef STRING_H
#define STRING_H

void* memcpy(void* dest, const void* src, size_t n)
{
          unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    
    if (d > s && d < s + n)
    {
        d += n;
        s += n;

        while (n--)
        {
            *--d = *--s;
        }
    } 
    else
    {
        while (n--)
        {
            *d++ = *s++;
        }
    }
    
    return dest;
}

void* memset(void* s, int32_t c, size_t n)
{
    unsigned char* p = (unsigned char*)s;

    while (n--)
    {
        *p++ = (unsigned char)c;
    }

    return s;
}

#endif