#the first code be calling from boot.s

.code16
.section ".inittext", "ax"
.global lowlevel_init
.global io_hlt
lowlevel_init:
jmp _lowlevel_init


################################################
#    
io_hlt:
    hlt
    ret
#
################################################
###############################################
_lowlevel_init:
	movw	%cs, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs



    
#######################################################
#switch to pmode
############################
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


fin:
    hlt
    jmp fin

INITSEG = 0x9000

idt_ptr:
    .word   0       /* limit */
    .long   0       /* base */

gdt_ptr:
    .word   0x28        /* limit (32 bytes = 5 GDT entries) */
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
    .byte   0x01        /* base_middle */
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
    .byte   0x01        /* base_middle */
    .byte   0x92        /* access */
    .byte   0xcf        /* flags + limit_high */
    .byte   0x00        /* base_high */


    /*
     * The Stack Segment Descriptor:
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
.org 0x200, 0x90
