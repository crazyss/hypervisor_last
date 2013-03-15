OUTPUT_FORMAT("elf32-i386", "elf32-i386","elf32-i386")
OUTPUT_ARCH(i386)

SECTIONS
{
        . = 0x7c00;
        .data : { helloos.o(.data) }
        .rodata : {
            start.o(.rodata*)    
        }

        .text :
        {
            lowlevel_init.o(.text*)    
            begin.o(.text*)
            start.o(.text*)
            mouse.o(.text*)
            int_entry.o(.text*)
            font.o(.rodata*)
        }

}

