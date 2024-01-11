.global x
.global main

.extern y
.extern f

.section    .data
x:
    .long   1

.section    .text

main:
    pushl   %rbp
    movl    %rsp, %rbp
    movl    [x], %reg0
    add     [y], %reg0
    sub     16, %rsp
    movl    %reg0, [%rbp-4]
    call    f
    movl    %reg0, %reg1
    movl    [%rbp-4], %reg0
    add     16, %rsp
    add     %reg1, %reg0
    popl    %rbp
    ret     

.end