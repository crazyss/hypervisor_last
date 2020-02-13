
.text
.code32
.global inthandler20
.global inthandler21
.global inthandler24
.global inthandler27
.global inthandler2c


inthandler24:
    push    %es
    push    %ds
    pushal
    movl    %esp,%eax
    pushl    %eax
    mov     %ss,%ax
    mov     %ax,%ds
    mov     %ax,%es
    call    _inthandler24
    popl     %eax
    popal
    pop     %ds
    pop     %es
    iretl

#for timer
inthandler20:
    push    %es
    push    %ds
    pushal
    movl    %esp,%eax
    pushl    %eax
    mov     %ss,%ax
    mov     %ax,%ds
    mov     %ax,%es
    call    _inthandler20
    popl     %eax
    popal
    pop     %ds
    pop     %es
    iretl

#for keyboard
inthandler21:
    push    %es
    push    %ds
    pushal
    movl    %esp,%eax
    pushl    %eax
    mov     %ss,%ax
    mov     %ax,%ds
    mov     %ax,%es
    call    _inthandler21
    popl     %eax
    popal
    pop     %ds
    pop     %es
    iretl

inthandler27:
    push    %es
    push    %ds
    pushal
    movl    %esp,%eax
    pushl    %eax
    mov     %ss,%ax
    mov     %ax,%ds
    mov     %ax,%es
    call    _inthandler27
    popl     %eax
    popal
    pop     %ds
    pop     %es
    iretl

    
#for mouse
inthandler2c:
    push    %es
    push    %ds
    pushal
    movl    %esp,%eax
    pushl    %eax
    mov     %ss,%ax
    mov     %ax,%ds
    mov     %ax,%es
    call    _inthandler2c
    popl     %eax
    popal
    pop     %ds
    pop     %es
    iretl
  
