.code32
.text

.global io_in8
io_in8:
    jmp _io_in8
.global io_out8
io_out8:
    jmp _io_out8
.global load_gdtr
load_gdtr:
    jmp _load_gdtr
.global load_idtr
load_idtr:
    jmp _load_idtr
.global io_sti
io_sti:
    jmp _io_sti





















_io_sti:
    sti
    ret


_load_idtr:
    movw    4(%esp),    %ax
    movw    %ax,    6(%esp)
    lidt    6(%esp)
    ret

_load_gdtr:

    movw 4(%esp),    %ax
    movw %ax,     6(%esp)
    lgdt    6(%esp)
    ret


_io_in8:
    movw    4(%esp),    %dx
    movw    $0x0,    %ax
    in      %dx, %al
    nop
    nop
    ret

_io_out8:
    movw    4(%esp),    %dx
    movb    8(%esp),    %al
    out     %al,%dx
    nop
    nop
    ret
