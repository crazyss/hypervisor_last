#the first code be calling from boot.s

VIDEO_PALLETE_PORT = 0x03c8
BG_INDEX_COLOR     = 0
COLOR_SELECTION_PORT = 0x03c9

.code16
.global lowlevel_init
.global io_hlt
lowlevel_init:
jmp _lowlevel_init



color:
.byte 0x00,0x00,0x00 #black
.byte 0xff,0x00,0x00
.byte 0x00,0xff,0x00
.byte 0xff,0xff,0x00
.byte 0x00,0x00,0xff
.byte 0xff,0x00,0xff
.byte 0x00,0xff,0xff
.byte 0xff,0xff,0xff
.byte 0xc6,0xc6,0xc6
.byte 0x84,0x00,0x00
.byte 0x00,0x84,0x00
.byte 0x84,0x84,0x00
.byte 0x00,0x00,0x84
.byte 0x84,0x00,0x84
.byte 0x00,0x84,0x84
.byte 0x00,0x84,0x84
.byte 0x84,0x84,0x84


##------------------------------------------------
##
#set_video_mode_vbe_0x103:
#    movw    $0x800, %ax
#    movw    %ax,    %es
#    movw    %ax,    %ds
#    xorw    %di,    %di
#
##check VBE support
#    movb    $0x4f,      %ah
#    movb    $0x00,      %al
#    int     $0x10
#
#    cmp     $0x004f,    %ax
#    jne     1f
#    movw    0x04(%di),  %ax
#    cmp     $0x0200,    %ax
#    jb      1f
#
#    movw    $0x103,     %cx
#    movb    $0x4f,      %ah
#    movb    $0x01,      %al
#    int $0x10
#
#    cmp     $0x00,  %ah
#    jne     1f
#    cmp     $0x4f,  %al
#    jne     1f
#    cmpb    $8,     0x19(%di)
#    jne     1f
#    cmpb    $4,     0x1b(%si)
#    jne     1f
#    movw    (%di),      %ax
#    andw    $0x0080,    %ax
#    jz      1f
#
#    #set VBE mode to 0x103
#    movw    $0x103,     %bx
#    addw    $0x4000,    %bx
#    movb    $0x4f,      %ah
#    movb    $0x02,      %al
#    int     $0x10
#
#    movw    $1,     %ax
#    ret
#
#1:  
#    movw    $0,     %ax
#    ret
################################################
#set_video_mode_0x13:
#    movb    $0,     %ah  #0x0  func
#    movb    $0x13,  %al     #display mode
#    int     $0x10
#    ret
#
################################################
#set_bg_color:
#    movw    $VIDEO_PALLETE_PORT,    %dx
#    movb    $BG_INDEX_COLOR,           %al
#    outb    %al,                    %dx
#    
#    movw    $COLOR_SELECTION_PORT,  %dx
#    movb    $0,                     %al
#    outb    %al,                    %dx
#    movb    $0,                     %al
#    outb    %al,                    %dx
#    movb    $25,                     %al
#    outb    %al,                    %dx
#    ret
#
################################################
#    
io_hlt:
    hlt
    ret
#
################################################
#clear_screen:
#    movb    $0x6, %ah
#    movb    $0,   %al
#    movb    $0,   %ch
#    movb    $0,   %cl
#    movb    $24,  %dh
#    movb    $79,  %dl
#    movb    $0x07,  %bh
#    int     $0x10
#    ret
#
###############################################
_lowlevel_init:
	movw	%cs, %ax
	movw	%ax, %ds


#    call clear_screen
#setting video mode

#    mov $0x13,%al
#    mov $0x00,%ah
#    int $0x10

#   call set_video_mode_vbe_0x103

#    call set_bg_color

#
#    movb $0x0, %bh
#    movw $300, %dx
#    movw $0, %cx
#    movb $0x0c, %ah
#    movb $9,    %al
#
#1:
#    int $0x10
#    incw    %cx
#    cmpw    $200,   %cx
#    jne 1b
#
    
############################################
#
#
#    movw $0xa000,%ax
#    movw %ax,   %es
#    movw $-1,    %bx
#2:
#    incw %bx
#    movw $0x3332, %es:(%bx)
#    cmpw $0xffff, %bx
#    jb 2b
#
##enter into hypervisor
#    call    hypervisor
#
#
#######################################################

    cli


X86_CR0_NW = 0x20000000 /* Not Write-through */
X86_CR0_CD = 0x40000000 /* Cache Disable */


#porting from Uboot , Disable Cache and Not Write-through
    movl    %cr0, %eax
    orl $(X86_CR0_NW | X86_CR0_CD), %eax
    movl    %eax, %cr0
    wbinvd

    lidtw    idt_ptr
    lgdtw    gdt_ptr
    nop
    nop


X86_CR0_PE = 0x00000001 /* Protection Enable */
    /* Now, we enter protected mode */
    movl    %cr0, %eax
    orl $X86_CR0_PE, %eax
    movl    %eax, %cr0


goto32:
    /* Flush the prefetch queue */
    ljmp $(8*2),$hypervisor

    jmp fin

INITSEG = 0x9000

#entry
#    jmp main

fin:
    hlt
    jmp fin

idt_ptr:
    .word   0       /* limit */
    .long   0       /* base */

gdt_ptr:
    .word   0x20        /* limit (32 bytes = 4 GDT entries) */
    .long   gdt + (INITSEG * 16) /* base */

gdt:
    /*
     * The GDT table ...
     *
     *   Selector   Type
     *   0x00       NULL
     *   0x08       Unused
     *   0x10       32bit code
     *   0x18       32bit data/stack
     */
    /* The NULL Desciptor - Mandatory */
    .word   0x0000      /* limit_low */
    .word   0x0000      /* base_low */
    .byte   0x00        /* base_middle */
    .byte   0x00        /* access */
    .byte   0x00        /* flags + limit_high */
    .byte   0x00        /* base_high */

    /* Unused Desciptor - (matches Linux) */
    .word   0x0000      /* limit_low */
    .word   0x0000      /* base_low */
    .byte   0x00        /* base_middle */
    .byte   0x00        /* access */
    .byte   0x00        /* flags + limit_high */
    .byte   0x00        /* base_high */

    /*
     * The Code Segment Descriptor:
     * - Base   = 0x00000000
     * - Size   = 4GB
     * - Access = Present, Ring 0, Exec (Code), Readable
     * - Flags  = 4kB Granularity, 32-bit
     */
    .word   0xffff      /* limit_low */
    .word   0x0000      /* base_low */
    .byte   0x09        /* base_middle */
    .byte   0x9a        /* access */
    .byte   0xcf        /* flags + limit_high */
    .byte   0x00        /* base_high */


    /*
     * The Data Segment Descriptor:
     * - Base   = 0x00000000
     * - Size   = 4GB
     * - Access = Present, Ring 0, Non-Exec (Data), Writable
     * - Flags  = 4kB Granularity, 32-bit
     */
    .word   0xffff      /* limit_low */
    .word   0x0000      /* base_low */
    .byte   0x09        /* base_middle */
    .byte   0x92        /* access */
    .byte   0xcf        /* flags + limit_high */
    .byte   0x00        /* base_high */

