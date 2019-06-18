.code32
.global hypervisor
.section .text
memcpy:
    movl    (%esi),%eax
    addl    $4,     %esi
    movl    %eax, (%edi)
    addl    $4,     %edi
    subl    $1,     %ecx
    jnz memcpy
    ret

hypervisor:
    movl    $(3 * 8), %eax
    movl    %eax, %fs
    movl    %eax, %ds
    movl    %eax, %gs
    movl    %eax, %es
    movl    %eax, %ss

#init stack
    movl    $0xFFFF0,  %esp
#jump into the C

    jmp     kernelstart

#
#show_logo:
#    movl    $0xb8000,    %edi
#
#    movw    $0x38,  %ax
#
#set_line_mem:
#    je  2f
#    movw    %ax,        (%edi)
#2:
#    incl     %edi
#    incl     %eax
#    loop    set_line_mem
#
#3:
#    jmp 3b
#

msg:
    .asciz "Hypervisor"
len_msg:
    .int . - msg

bootpack:
