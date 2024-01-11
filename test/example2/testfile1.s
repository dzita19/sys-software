.global main

.section    .text

main:
    pushl   %rbp
    movl    %rsp, %rbp
    sub     8, %rsp
    movl    4, %reg0
    movl    %reg0, [%rbp-4]
    movl    [%rbp-4], %reg0
.section    .rodata
.LT1:
    .long   .L1
    .long   .L1
    .long   .L1
.section    .text
    sub     1, %reg0
    cmp     3, %reg0
    jae     .L2
    lsl     2, %reg0
    add     .LT1, %reg0
    jmp     [%reg0]
.L2:
.section    .rodata
.LT2:
    .long   .L3
.section    .text
    sub     6, %reg0
    cmp     1, %reg0
    jae     .L4
    lsl     2, %reg0
    add     .LT2, %reg0
    jmp     [%reg0]
.L1:
    jmp     .L5
.L3:
    movl    2, %reg0
    jmp     .L6
.L4:
    movl    0, %reg0
    movl    %reg0, [%rbp-8]
.L7:
    movl    [%rbp-8], %reg0
    cmp     3, %reg0
    jge     .L5
    movl    [%rbp-4], %reg0
    add     1, %reg0
    movl    %reg0, [%rbp-4]
    movl    [%rbp-8], %reg0
    add     1, %reg0
    movl    %reg0, [%rbp-8]
    jmp     .L7
.L5:
    movl    [%rbp-4], %reg0
.L6:
    add     8, %rsp
    popl    %rbp
    ret     

.end