#include <unistd.h>

#define ngx_tolower(c) (u_char)((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define ngx_toupper(c) (u_char)((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

typedef unsigned char u_char;

void ngx_strlow(u_char *dst, u_char *src, size_t n)
{
    while (n)
    {
        *dst = ngx_tolower(*src);
        dst++;
        src++;
        n--;
    }
}