CYLS = 10
BOOTSEG =   0x07C0
INITSEG =   0x9000
SYSSEG  =   0x1000
.code16
.section ".bstext", "ax"
start:
#.byte 0xeb,0x4e,0x90
ljmp $BOOTSEG, $main_start

main_start:
	movw	%cs, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	xorw	%sp, %sp

    
#move BootSector to 0x9000(INITSEG)
    movw $BOOTSEG, %ax
    movw %ax,   %ds
    movw $INITSEG, %ax
    movw %ax,   %es
    movw $256,  %cx
    xorw %si,   %si
    xorw %di,   %di
    rep;
    movsw
    
    ljmp $INITSEG, $goto_init_seg
goto_init_seg:
    movw %cs, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss

    movw $0xFF00, %sp

read_disk:
#read lowlevel_init data to INITSEG RAM from Disk
    movw $INITSEG, %ax
    movw %ax,%es
    movb $0, %ch #cylinder
    movb $0, %dh #header
    movb $2, %cl #sector


readloop:
    mov $0,%si #record error times
retry:
    movb $2, %ah    #read operation
    movb $1, %al    #a number of "sector moving" !.
    movw $0x200, %bx    #clear bx
    movb $0, %dl    #A driver
    int  $0x13      #bios
    jnc  next
    
    add  $1, %si    #timer + 1
    cmp  $5, %si
    jae  error
#
    movb $0, %ah    #reset drive
    movb $0, %dl
    int $0x13
    jmp retry 
#
#
next:


#move BootSector/lowlevel to 0x10000(SYSSEG)
    movw $INITSEG, %ax
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
    movw $0x400, %bx    #clear bx
    movb $0, %dl    #A driver
    int  $0x13      #bios
    jnc  next2
    
    add  $1, %si    #timer + 1
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
    add $1,%cl
    cmp $18,%cl
    jbe readloop2        #reading 18 sectors
#    
    movb $1,%cl
    add $1,%dh
    cmp $1,%dh
    jb readloop2         #reading 2 header

    movb $0,%dh         #reading 5 cylinders
    add $1,%ch
    cmp $1,%ch
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
    mov $0x13,%al
    mov $0x00,%ah
    int $0x10
#
    ljmp $INITSEG, $lowlevel_init
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

