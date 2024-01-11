.global y
.global f

.extern x

.section    .data
y:
    .word   4
    .zero   2

.section    .text

f:
    pushl   %rbp
    movl    %rsp, %rbp
    movw    [y], %reg0
    sxwl    %reg0
    add     [x], %reg0
    popl    %rbp
    ret     

.end