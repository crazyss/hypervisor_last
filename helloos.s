CYLS = 10
BOOTSEG = 0x0
.global debug
.code16
.section .data
start:
#.byte 0xeb,0x4e,0x90
jmp main_start
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
    movl $0,%eax
    movl $0x7c00,%esp
    mov %ax,%ss
    mov %ax,%ds
    mov %ax,%es
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



    movw $msg,%si
putloop:
    lodsb
    cmp $0,%al
    je read_disk
    movb $0x0e, %ah
    int $0x10
    jmp putloop
    

read_disk:
#read disk to ram
    movw $0x07e0,%ax
    movw %ax,%es
    movb $0, %ch #cylinder
    movb $0, %dh #header
    movb $2, %cl #sector


readloop:
    mov $0,%si #record error times
retry:
    movb $2, %ah    #read operation
    movb $8, %al    #a number of "sector moving" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

    movb $0,%dh         #reading 10 cylinders
    add $1,%ch
    cmp $10,%ch
    jb readloop

#
    jmp fin
#
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





fin:
    jmp lowlevel_init
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
