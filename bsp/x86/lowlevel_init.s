#the first code be calling from bootsector.s
#This is the size of a sector.

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

#Iniialization sequence 8259a
    movb $0x11, %al
	movw $0x20, %dx
    out %al,    %dx
    nop;
    nop;
	movw $0xA0, %dx
    out %al,    %dx
    nop;
    nop;
    movb $0x20, %al
	movw $0x21, %dx
    out %al,    %dx
    nop;
    nop;
    movb $0x28, %al
	movw $0xA1, %dx
    out %al,    %dx
    nop;
    nop;
    movb $0x4,  %al
	movw $0x21, %dx
    out %al,    %dx
    nop;
    nop;
    movb $0x2,  %al
	movw $0xA1, %dx
    out %al,    %dx
    nop;
    nop;
    movb $0x01, %al
	movw $0x21, %dx
    out %al,    %dx
	movw $0xA1, %dx
    out %al,    %dx

    movb $0xFF, %al
	movw $0x21, %dx
    out %al,    %dx
    nop;
    nop;
	movw $0xA1, %dx
    out %al,    %dx
    nop;
    nop;
    
    movb $0xfb, %al
	movw $0x21, %dx
    out %al,    %dx
    nop;
    nop;
    movb $0xff, %al
	movw $0xA1, %dx
    out %al,    %dx
    nop;
    nop;
    
#Get memory map and store memroy table start from  0x00098004, each entry has 20bytes data but
#currently we reserve 0x20 bytes for each entry
#Each entry has :
#First uint64_t = Base address
#Second uint64_t = Length of "region" (if this value is 0, ignore the entry)
#Next uint32_t = Region "type"
#   Type 1: Usable (normal) RAM
#   Type 2: Reserved - unusable
#   Type 3: ACPI reclaimable memory
#   Type 4: ACPI NVS memory
#   Type 5: Area containing bad memory
#Set register base https://wiki.osdev.org/Detecting_Memory_(x86)
    mov $0x8004, %di
    xor %ebx, %ebx
    mov $0x0534D4150, %edx
    mov $0xe820,%eax
    mov $24, %ecx
    int $0x15
    #call again
get_next:
    add $0x20, %di
    mov $0xe820,%eax
    mov $24, %ecx
    int $0x15
    cmp $0, %ebx /*ebx=0 means no other entry*/
    jnz get_next
#for debug
#    hlt

#Inital Serial Device 1
#com1 at 0x3F8

#Disable all interrupt
	movb $0x1, %al
	movw $(0x3f8+1), %dx
	out %al, %dx
#Enable DLAB bit for baud bits
	movb $0x80, %al
	movw $(0x3f8+3), %dx
	out %al, %dx
#Write 1 to divisor for 115200 bits
	movb $1, %al
	movw $(0x3f8), %dx
	out %al, %dx
	movb $0, %al
	movw $(0x3f8+1), %dx
	out %al, %dx
#8 bits, no parity, one stop bit
	movb $0x3, %al
	movw $(0x3f8+3), %dx
	out %al, %dx
#enable FIFO, clear them, with 14-byte threshold
	movb $0xc7, %al
	movw $(0x3f8+2), %dx
	out %al, %dx
#IRQs enabled, RTS/DSR set
	movb $0x0B, %al
	movw $(0x3f8+4), %dx
	out %al, %dx


#######################################################
#switch to pmode
############################
    #Disable interrupt in CPU core.
    cli


X86_CR0_NW = 0x20000000 /* Not Write-through */
X86_CR0_CD = 0x40000000 /* Cache Disable */


#porting from Uboot , Disable Cache and Not Write-through
    movl    %cr0, %eax
    orl $(X86_CR0_NW | X86_CR0_CD), %eax
    movl    %eax, %cr0

#write back cache line and flush cache, WBINVD is not support on earlier than i486.
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

INITSEG = 0x7000

idt_ptr:
    .word   0       /* limit */
    .long   0       /* base */

gdt_ptr:
    .word   0x20        /* limit (32 bytes = 5 GDT entries) */
    .long   gdt + (INITSEG * 16) /* base  [INITSEG:gdt]*/

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
    /*base = base_high .+. base_middle .+. base_low = 0x10000 */


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


.org 0x200, 0x90
