
.text
.code32
.global load_cr0
.global store_cr0
.global io_load_eflags
.global io_store_eflags

io_load_eflags:
    pushfl
    pop %eax
    ret

io_store_eflags:
    movl 4(%esp), %eax
    pushl %eax
    popfl
    ret
    

load_cr0:
    movl %cr0, %eax
    ret

store_cr0:
    movl 4(%esp), %eax
    movl %eax, %cr0
    ret

