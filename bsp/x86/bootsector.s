CYLS = 10
BOOTSEG =   0x07C0
INITSEG =   0x9000
SYSSEG  =   0x1000
.code16
.section ".bstext", "ax"
start:
#.byte 0xeb,0x4e,0x90
ljmp $BOOTSEG, $main_start
#jmp main_start
.ascii "HELLOIPL"
.word 512
.byte 1
.word 1
.byte 2
.word 224
.word 2880
.byte 0xf0
.word 9
.word 18
.word 2
.word 0 
.word 0 
.word 2880
.word 0 
.byte 0,0,0x29
.word 0xffff
.word 0xffff
.ascii "HELLO-OS   "
.ascii "FAT12   "

myvar:
.skip 18
#int $0x19

#    movb $2,%ah
#    movb $10,%dh
#    movb $20,%dl
#    movb $0,bh
#    int $0x10
#

#    movl 0xe820,%eax
#    int $0x15


#    movl $msg,%esi
#  this statments not normal work.

main_start:
	movw	%cs, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	xorw	%sp, %sp


#
#    je fin
#
loadmsg:
#    mov $msg,%si
#
##    movb $10,%cl
#putloop:
#    movb 0x7c99,%al
##   add $1,%si
##    cmp $0,%al
##    je fin
##
##    movb %ds:(%si),%al
#    movb $0x0e, %ah
##    movw $15,%bx
#    int $0x10
##   jmp putloop
#
##
##    

#    movb 0x7cb3,%al
#    movb $0x0e, %ah
#    int $0x10
#    movb 0x7cb4,%al
#    movb $0x0e, %ah
#    int $0x10
#    movb 0x7cb5,%al
#    movb $0x0e, %ah
#    int $0x10
#    movb 0x7cb6,%al
#    movb $0x0e, %ah
#    int $0x10
#    movb 0x7cb7,%al
#    movb $0x0e, %ah
#    int $0x10

##    movb $0x0e, %ah
##    movb (%esi),%al
##    int $0x10
##    movb 1(%esi),%al
##    int $0x10
##    movb 2(%esi),%al
##    int $0x10
##    #movb (%si),%al
#    int $0x10

#display welcome string 



    
read_disk:
##move BootSector to 0x9000(INITSEG)
#    movw $BOOTSEG, %ax
#    movw %ax,   %ds
#    movw $INITSEG, %ax
#    movw %ax,   %es
#    movw $256,  %cx
#    xorw %si,   %si
#    xorw %di,   %di
#    rep; movsw
#    
#
#read disk to ram
    movw $INITSEG, %ax
    movw %ax,%es
    movb $0, %ch #cylinder
    movb $0, %dh #header
    movb $1, %cl #sector


readloop:
    mov $0,%si #record error times
retry:
    movb $2, %ah    #read operation
    movb $1, %al    #a number of "sector moving" !.
    movw $0, %bx    #clear bx
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
    mov %es,%ax         #move ES to point the next segment start-point
    add $0x20,%ax
    mov %ax,%es
#    
    add $1,%cl
    cmp $18,%cl
    jbe readloop        #reading 18 sectors
#    
    movb $1,%cl
    add $1,%dh
    cmp $2,%dh
    jb readloop         #reading 2 header

    movb $0,%dh         #reading 5 cylinders
    add $1,%ch
    cmp $5,%ch
    jb readloop

#
##
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

debug:
    movw    $0,%ax          #restore ES register
    movw    %ax,%es
    movw    $debug_msg,   %bp
    movw    len_debug,    %cx
    movb    $0x00,  %al
    movb    $0x13,  %ah

    movb    $0x03,  %bl
    movb    $0x00,  %bh

    movb    $0x10,  %dh  #cl
    movb    $0x03,  %dl  #line
    int $0x10
#
#
fin:

#setting video mode
#    mov $0x13,%al
#    mov $0x00,%ah
#    int $0x10
#
#
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

debug_msg:
.asciz "Debug info"
len_debug:
    .int . - debug_msg

.org 0x1fe, 0x90

.byte 0x55,0xaa

#.byte 0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
#.skip 4600
#.byte 0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
#
#.skip 1469433
