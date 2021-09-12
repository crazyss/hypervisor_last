#the frist 512 bytes
#This is the size of a sector.

CYLS = 10
BOOTSEG =   0x07C0
SYSSEG  =   0x1000
.code16
.section ".bstext", "ax"
start:
#bios will load bootsector into 0x07c0:0000, then we using ljmp
#to jump to 0x07c0:0000 to start our code
#machine code ea 05 00 c0 07
#ljmp   $0x7c0,$0x5 (Through objdump -D -Mi8086)
#cs will be set to 0x7c0
ljmp $BOOTSEG, $main_start

main_start:
	movw	%cs, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	xorw	%sp, %sp
    
    movw $0xFBFF, %sp

read_disk:
#load lowlevel_init after BOOTSEG RAM from Disk
#the early GDT store on the area of lowlevel code.
    movw $BOOTSEG, %ax
    movw %ax,%es
    movb $0, %ch #cylinder
    movb $2, %cl #sector
    movb $0, %dh #header


readloop:
    mov $0,%si #record error times
retry:
    movb $2, %ah    #read operation
    movb $1, %al    #a number of "sector moving" !.
    movw $0x200, %bx    #set BX register point to 0x200 offset
    #movb $0, %dl    #A driver
    int  $0x13      #bios
    jnc  next
    
    add  $1, %si    #counter + 1
    cmp  $5, %si
    jae  error

    movb $0, %ah    #reset drive
    movb $0, %dl
    int $0x13
next:


#move BootSector/lowlevel to 0x10000(SYSSEG)
#gdt table in lowlevel will take this address as base address
    movw $BOOTSEG, %ax
    movw %ax,   %ds
    movw $SYSSEG, %ax
    movw %ax,   %es
    movw $512,  %cx
    xorw %si,   %si
    xorw %di,   %di
    rep;
    movsw

#read SYSSEG data to RAM from Disk
    movw $SYSSEG, %ax
    movw %ax,%es
    movb $0, %ch #cylinder
    movb $0, %dh #header
    movb $3, %cl #sector


readloop2:
    mov $0,%si #record error times
retry2:
    movb $2, %ah    #read operation
    movb $1, %al    #a number of "sector moving" !.
    movw $0x400, %bx    #set bx point to 0x400 offset
    movb $0, %dl    #A driver
    int  $0x13      #bios
    jnc  next2
    
    add  $1, %si    #counter + 1
    cmp  $5, %si
    jae  error
#
    movb $0, %ah    #reset drive
    movb $0, %dl
    int $0x13
    jmp retry2

next2:

    mov %es,%ax         #move ES to point the next segment start-point
    add $0x20,%ax
    mov %ax,%es
#    
    add $1,%cl      #point to the next sector
    cmp $35,%cl     #Check the total number of sectors !!!IMPORTANT
    jbe readloop2        #reading 18 sectors
#    
    movb $1,%cl
    add $1,%dh
    cmp $2,%dh
    jb readloop2         #reading 1 header

    movb $0,%dh         #reading 1 cylinders
    add $1,%ch
    cmp $2,%ch
    jb readloop2

#
##
#loadmsg:
#display welcome string 
##
    movw $msg,%si
putloop:
    lodsb
    cmp $0,%al
   # je read_disk
    je fin
    movb $0x0e, %ah
    int $0x10
    jmp putloop
##
#
#
error:
    movw    $0,%ax          #restore ES register
    movw    %ax,%es
    movw    $error_msg,   %bp
    movw    len_error,    %cx
    movb    $0x00,  %al
    movb    $0x13,  %ah

    movb    $0x03,  %bl
    movb    $0x00,  %bh

    movb    $0x10,  %dh  #cl
    movb    $0x03,  %dl  #line
    int $0x10

disk_error_loop:
    hlt
    jmp disk_error_loop

#
fin:

#setting video mode
    #VIDEO - SET VIDEO MODE
    #AH=0x00,
    #AL=0x13, #0x13 : G  40x25  8x8   320x200  256/256K  .   A000 VGA,MCGA,ATI VIP

    mov $0x00,%ah
    mov $0x13,%al
    int $0x10
#
    #jmp lowlevel_init
    ljmp $SYSSEG, $lowlevel_init
    hlt

    jmp fin


.byte 0,2,31,23,21,12,31,23
msg:
.asciz "Hypervisor"
len:
    .int . - msg
.byte 0
error_msg:
.asciz "have some error on read_disk to ram "
len_error:
    .int . - error_msg


.org 0x1fe, 0x90

.byte 0x55,0xaa

