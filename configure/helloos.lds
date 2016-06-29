OUTPUT_FORMAT("elf32-i386", "elf32-i386","elf32-i386")
OUTPUT_ARCH(i386)

SECTIONS
{
        . = 0x0000;
        .bstext : { bootsector.o(.bstext) }
        .inittext : { lowlevel_init.o(.inittext) }
        .text :
        {
            begin.o(.text*)
            start.o(.text*)
            start.o(.rodata*)    
            mouse.o(.text*)
            int_entry.o(.text*)
            memtest.o(.text*)
            libmemtest.o(.text*)
            lib.o(.*)
            int.o(.*)
            *(.text*)
        }
        .bss : {
            *(.bss*)
        }
        .rodata : {
            *(.rodata*)
            font.o(.rodata*)
        }
}

