#include <stdio.h>
#include <stdlib.h>

// sysdeps/i386/start.S
// __libc_csu_init -> _init -> __libc_global_ctors
// objdump -sj .init_array a.out

// ld --verbose
// OUTPUT_ARCH(i386:x86-64)
// ENTRY(_start)
// ...
// .init_array:
// {
//     PROVIDE_HIDDEN(__init_array_start =.);
//     KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
//     KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
//     PROVIDE_HIDDEN(__init_array_end =.);
// }
// .ctors:
// {
//     /* gcc uses crtbegin.o to find the start of
//        the constructors, so we make sure it is
//        first.  Because this is a wildcard, it
//        doesn't matter if the user does not
//        actually link against crtbegin.o; the
//        linker won't look for a file to match a
//        wildcard.  The wildcard also means that it
//        doesn't matter which directory crtbegin.o
//        is in.  */
//     KEEP(*crtbegin.o(.ctors))
//     KEEP (*crtbegin?.o(.ctors))
//     /* We don't want to include the .ctor section from
//        the crtend.o file until after the sorted ctors.
//        The .ctor section from the crtend file contains the
//        end of ctors marker and it must be last */
//     KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
//     KEEP(*(SORT(.ctors.*)))
//     KEEP(*(.ctors))
// }

void __attribute__((constructor)) before()
{
    printf("before main\n");
}

int main(int argc, char const *argv[])
{
    printf("Main\n");
    return 0;
}
