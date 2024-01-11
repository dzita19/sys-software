.global putchar
.global printdec
.global printstr
.global printf
.global get_variadic_param

.section    .data
buffer:
    .zero   16

.section    .rodata
.LP1:
    .asciz  "    movl    %rbp, %reg0"
.LP2:
    .asciz  "    add     [%rbp-4], %reg0"

.section    .text

putchar:
    pushl   %rbp
    movl    %rsp, %rbp
    movl    [%rbp+8], %reg0
    movl    %reg0, [61448]
    movl    1, %reg0
    movl    %reg0, [61440]
    movl    0, %reg0
    popl    %rbp
    ret     

printdec:
    pushl   %rbp
    movl    %rsp, %rbp
    sub     16, %rsp
    movl    0, %reg0
    movl    %reg0, [%rbp-4]
    movl    0, %reg0
    movl    %reg0, [%rbp-8]
    movl    [%rbp+8], %reg0
    cmp     0, %reg0
    jge     .L1
    movl    1, %reg0
    movl    %reg0, [%rbp-8]
    movl    [%rbp+8], %reg0
    neg     %reg0
    movl    %reg0, [%rbp+8]
.L1:
.L2:
    movl    buffer, %reg0
    movl    [%rbp-4], %reg1
    movl    [%rbp-4], %reg2
    add     1, %reg2
    movl    %reg2, [%rbp-4]
    add     %reg1, %reg0
    movl    %reg0, %reg1
    movl    [%rbp+8], %reg2
    imod    10, %reg2
    add     48, %reg2
    and     255, %reg2
    movb    %reg2, [%reg1]
    movb    [%reg0], %reg0
    movl    [%rbp+8], %reg0
    idiv    10, %reg0
    movl    %reg0, [%rbp+8]
    movl    [%rbp+8], %reg0
    cmp     0, %reg0
    jgt     .L2
    movl    [%rbp-8], %reg0
    cmp     0, %reg0
    jeq     .L3
    movl    buffer, %reg0
    movl    [%rbp-4], %reg1
    movl    [%rbp-4], %reg2
    add     1, %reg2
    movl    %reg2, [%rbp-4]
    add     %reg1, %reg0
    movl    %reg0, %reg1
    movb    45, %reg2
    movb    %reg2, [%reg1]
    movb    [%reg0], %reg0
.L3:
    movl    [%rbp-4], %reg0
    sub     1, %reg0
    movl    %reg0, [%rbp-12]
.L4:
    movl    [%rbp-12], %reg0
    cmp     0, %reg0
    jlt     .L5
    movl    buffer, %reg0
    add     [%rbp-12], %reg0
    movb    [%reg0], %reg0
    sxbl    %reg0
    movl    %reg0, [61448]
    movl    1, %reg0
    movl    %reg0, [61440]
    movl    [%rbp-12], %reg0
    movl    [%rbp-12], %reg1
    sub     1, %reg1
    movl    %reg1, [%rbp-12]
    jmp     .L4
.L5:
    movl    0, %reg0
    add     16, %rsp
    popl    %rbp
    ret     

printstr:
    pushl   %rbp
    movl    %rsp, %rbp
    sub     8, %rsp
    movl    0, %reg0
    movl    %reg0, [%rbp-4]
.L6:
    movl    [%rbp+8], %reg0
    add     [%rbp-4], %reg0
    movb    [%reg0], %reg0
    sxbl    %reg0
    cmp     0, %reg0
    jeq     .L7
    movl    [%rbp+8], %reg0
    add     [%rbp-4], %reg0
    movb    [%reg0], %reg0
    sxbl    %reg0
    movl    %reg0, [61448]
    movl    1, %reg0
    movl    %reg0, [61440]
    movl    [%rbp-4], %reg0
    movl    [%rbp-4], %reg1
    add     1, %reg1
    movl    %reg1, [%rbp-4]
    jmp     .L6
.L7:
    movl    0, %reg0
    add     8, %rsp
    popl    %rbp
    ret     

printf:
    pushl   %rbp
    movl    %rsp, %rbp
    sub     48, %rsp
    movl    0, %reg0
    movl    %reg0, [%rbp-20]
    movl    0, %reg0
    movl    %reg0, [%rbp-24]
    movl    0, %reg0
    movl    %reg0, [%rbp-28]
    movl    0, %reg0
    movl    %reg0, [%rbp-32]
    movl    0, %reg0
    movl    %reg0, [%rbp-36]
    movl    4, %reg0
    movl    %reg0, [%rbp-40]
    movl    0, %reg0
    movl    %reg0, [%rbp-44]
    movl    0, %reg0
    movl    %reg0, [%rbp-48]
.L8:
    movl    [%rbp+8], %reg0
    add     [%rbp-48], %reg0
    movb    [%reg0], %reg0
    sxbl    %reg0
    cmp     0, %reg0
    jeq     .L9
    movl    [%rbp-24], %reg0
.section    .rodata
.LT1:
    .long   .L10
    .long   .L11
    .long   .L12
    .long   .L13
    .long   .L14
    .long   .L15
    .long   .L16
.section    .text
    cmp     7, %reg0
    jae     .L17
    lsl     2, %reg0
    add     .LT1, %reg0
    jmp     [%reg0]
.L10:
    movl    [%rbp+8], %reg0
    add     [%rbp-48], %reg0
    movb    [%reg0], %reg0
    sxbl    %reg0
.section    .rodata
.LT2:
    .long   .L18
.section    .text
    sub     37, %reg0
    cmp     1, %reg0
    jae     .L19
    lsl     2, %reg0
    add     .LT2, %reg0
    jmp     [%reg0]
.L18:
    movl    1, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L20
.L19:
    sub     16, %rsp
    movl    [%rbp+8], %reg0
    add     [%rbp-48], %reg0
    movb    [%reg0], %reg0
    sxbl    %reg0
    movl    %reg0, [%rsp]
    call    putchar
    add     16, %rsp
    jmp     .L20
.L20:
    jmp     .L17
.L11:
    movl    [%rbp+8], %reg0
    add     [%rbp-48], %reg0
    movb    [%reg0], %reg0
    sxbl    %reg0
.section    .rodata
.LT3:
    .long   .L21
.section    .text
    sub     32, %reg0
    cmp     1, %reg0
    jae     .L22
    lsl     2, %reg0
    add     .LT3, %reg0
    jmp     [%reg0]
.L22:
.section    .rodata
.LT4:
    .long   .L23
.section    .text
    sub     3, %reg0
    cmp     1, %reg0
    jae     .L24
    lsl     2, %reg0
    add     .LT4, %reg0
    jmp     [%reg0]
.L24:
.section    .rodata
.LT5:
    .long   .L25
    .long   .L26
    .long   .L26
    .long   .L26
    .long   .L27
    .long   .L26
    .long   .L28
    .long   .L26
    .long   .L26
    .long   .L29
    .long   .L30
    .long   .L30
    .long   .L30
    .long   .L30
    .long   .L30
    .long   .L30
    .long   .L30
    .long   .L30
    .long   .L30
.section    .text
    sub     4, %reg0
    cmp     19, %reg0
    jae     .L31
    lsl     2, %reg0
    add     .LT5, %reg0
    jmp     [%reg0]
.L31:
.section    .rodata
.LT6:
    .long   .L32
.section    .text
    sub     49, %reg0
    cmp     1, %reg0
    jae     .L33
    lsl     2, %reg0
    add     .LT6, %reg0
    jmp     [%reg0]
.L33:
.section    .rodata
.LT7:
    .long   .L32
    .long   .L32
.section    .text
    sub     11, %reg0
    cmp     2, %reg0
    jae     .L34
    lsl     2, %reg0
    add     .LT7, %reg0
    jmp     [%reg0]
.L34:
.section    .rodata
.LT8:
    .long   .L35
    .long   .L32
.section    .text
    sub     5, %reg0
    cmp     2, %reg0
    jae     .L36
    lsl     2, %reg0
    add     .LT8, %reg0
    jmp     [%reg0]
.L36:
.section    .rodata
.LT9:
    .long   .L37
.section    .text
    sub     4, %reg0
    cmp     1, %reg0
    jae     .L38
    lsl     2, %reg0
    add     .LT9, %reg0
    jmp     [%reg0]
.L38:
.section    .rodata
.LT10:
    .long   .L32
    .long   .L32
.section    .text
    sub     3, %reg0
    cmp     2, %reg0
    jae     .L39
    lsl     2, %reg0
    add     .LT10, %reg0
    jmp     [%reg0]
.L39:
.section    .rodata
.LT11:
    .long   .L32
    .long   .L26
    .long   .L32
.section    .text
    sub     4, %reg0
    cmp     3, %reg0
    jae     .L40
    lsl     2, %reg0
    add     .LT11, %reg0
    jmp     [%reg0]
.L40:
.section    .rodata
.LT12:
    .long   .L32
.section    .text
    sub     5, %reg0
    cmp     1, %reg0
    jae     .L26
    lsl     2, %reg0
    add     .LT12, %reg0
    jmp     [%reg0]
.L28:
    movl    [%rbp-32], %reg0
    or      1, %reg0
    movl    %reg0, [%rbp-32]
    jmp     .L41
.L27:
    movl    [%rbp-32], %reg0
    or      2, %reg0
    movl    %reg0, [%rbp-32]
    jmp     .L41
.L29:
    movl    [%rbp-32], %reg0
    or      4, %reg0
    movl    %reg0, [%rbp-32]
    jmp     .L41
.L23:
    movl    [%rbp-32], %reg0
    or      8, %reg0
    movl    %reg0, [%rbp-32]
    jmp     .L41
.L21:
    movl    [%rbp-32], %reg0
    or      16, %reg0
    movl    %reg0, [%rbp-32]
    jmp     .L41
.L25:
    movl    [%rbp-32], %reg0
    or      32, %reg0
    movl    %reg0, [%rbp-32]
    jmp     .L41
.L30:
    movl    2, %reg0
    movl    %reg0, [%rbp-24]
    movl    [%rbp-48], %reg0
    movl    [%rbp-48], %reg1
    sub     1, %reg1
    movl    %reg1, [%rbp-48]
    jmp     .L41
.L35:
    movl    3, %reg0
    movl    %reg0, [%rbp-24]
    movl    [%rbp-48], %reg0
    movl    [%rbp-48], %reg1
    sub     1, %reg1
    movl    %reg1, [%rbp-48]
    jmp     .L41
.L37:
    movl    5, %reg0
    movl    %reg0, [%rbp-24]
    movl    [%rbp-48], %reg0
    movl    [%rbp-48], %reg1
    sub     1, %reg1
    movl    %reg1, [%rbp-48]
    jmp     .L41
.L32:
    movl    6, %reg0
    movl    %reg0, [%rbp-24]
    movl    [%rbp-48], %reg0
    movl    [%rbp-48], %reg1
    sub     1, %reg1
    movl    %reg1, [%rbp-48]
    jmp     .L41
.L26:
    movl    7, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L41
.L41:
    jmp     .L17
.L12:
    movl    [%rbp+8], %reg0
    add     [%rbp-48], %reg0
    movb    [%reg0], %reg0
    sxbl    %reg0
.section    .rodata
.LT13:
    .long   .L42
    .long   .L42
    .long   .L42
    .long   .L42
    .long   .L42
    .long   .L42
    .long   .L42
    .long   .L42
    .long   .L42
    .long   .L42
.section    .text
    sub     48, %reg0
    cmp     10, %reg0
    jae     .L43
    lsl     2, %reg0
    add     .LT13, %reg0
    jmp     [%reg0]
.L43:
.section    .rodata
.LT14:
    .long   .L44
.section    .text
    sub     40, %reg0
    cmp     1, %reg0
    jae     .L45
    lsl     2, %reg0
    add     .LT14, %reg0
    jmp     [%reg0]
.L45:
.section    .rodata
.LT15:
    .long   .L44
    .long   .L44
.section    .text
    sub     11, %reg0
    cmp     2, %reg0
    jae     .L46
    lsl     2, %reg0
    add     .LT15, %reg0
    jmp     [%reg0]
.L46:
.section    .rodata
.LT16:
    .long   .L47
    .long   .L44
.section    .text
    sub     5, %reg0
    cmp     2, %reg0
    jae     .L48
    lsl     2, %reg0
    add     .LT16, %reg0
    jmp     [%reg0]
.L48:
.section    .rodata
.LT17:
    .long   .L49
.section    .text
    sub     4, %reg0
    cmp     1, %reg0
    jae     .L50
    lsl     2, %reg0
    add     .LT17, %reg0
    jmp     [%reg0]
.L50:
.section    .rodata
.LT18:
    .long   .L44
    .long   .L44
.section    .text
    sub     3, %reg0
    cmp     2, %reg0
    jae     .L51
    lsl     2, %reg0
    add     .LT18, %reg0
    jmp     [%reg0]
.L51:
.section    .rodata
.LT19:
    .long   .L44
    .long   .L52
    .long   .L44
.section    .text
    sub     4, %reg0
    cmp     3, %reg0
    jae     .L53
    lsl     2, %reg0
    add     .LT19, %reg0
    jmp     [%reg0]
.L53:
.section    .rodata
.LT20:
    .long   .L44
.section    .text
    sub     5, %reg0
    cmp     1, %reg0
    jae     .L52
    lsl     2, %reg0
    add     .LT20, %reg0
    jmp     [%reg0]
.L42:
    movl    [%rbp-40], %reg0
    imul    10, %reg0
    movl    %reg0, [%rbp-40]
    movl    [%rbp-40], %reg0
    movl    [%rbp+8], %reg1
    add     [%rbp-48], %reg1
    movb    [%reg1], %reg1
    sxbl    %reg1
    sub     48, %reg1
    add     %reg1, %reg0
    movl    %reg0, [%rbp-40]
    jmp     .L54
.L47:
    movl    3, %reg0
    movl    %reg0, [%rbp-24]
    movl    [%rbp-48], %reg0
    movl    [%rbp-48], %reg1
    sub     1, %reg1
    movl    %reg1, [%rbp-48]
    jmp     .L54
.L49:
    movl    5, %reg0
    movl    %reg0, [%rbp-24]
    movl    [%rbp-48], %reg0
    movl    [%rbp-48], %reg1
    sub     1, %reg1
    movl    %reg1, [%rbp-48]
    jmp     .L54
.L44:
    movl    6, %reg0
    movl    %reg0, [%rbp-24]
    movl    [%rbp-48], %reg0
    movl    [%rbp-48], %reg1
    sub     1, %reg1
    movl    %reg1, [%rbp-48]
    jmp     .L54
.L52:
    movl    7, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L54
.L54:
    jmp     .L17
.L13:
    movl    [%rbp+8], %reg0
    add     [%rbp-48], %reg0
    movb    [%reg0], %reg0
    sxbl    %reg0
.section    .rodata
.LT21:
    .long   .L55
.section    .text
    sub     88, %reg0
    cmp     1, %reg0
    jae     .L56
    lsl     2, %reg0
    add     .LT21, %reg0
    jmp     [%reg0]
.L56:
.section    .rodata
.LT22:
    .long   .L55
.section    .text
    sub     12, %reg0
    cmp     1, %reg0
    jae     .L57
    lsl     2, %reg0
    add     .LT22, %reg0
    jmp     [%reg0]
.L57:
.section    .rodata
.LT23:
    .long   .L58
    .long   .L55
.section    .text
    sub     4, %reg0
    cmp     2, %reg0
    jae     .L59
    lsl     2, %reg0
    add     .LT23, %reg0
    jmp     [%reg0]
.L59:
.section    .rodata
.LT24:
    .long   .L55
.section    .text
    sub     7, %reg0
    cmp     1, %reg0
    jae     .L60
    lsl     2, %reg0
    add     .LT24, %reg0
    jmp     [%reg0]
.L60:
.section    .rodata
.LT25:
    .long   .L55
.section    .text
    sub     6, %reg0
    cmp     1, %reg0
    jae     .L61
    lsl     2, %reg0
    add     .LT25, %reg0
    jmp     [%reg0]
.L61:
.section    .rodata
.LT26:
    .long   .L55
.section    .text
    sub     3, %reg0
    cmp     1, %reg0
    jae     .L62
    lsl     2, %reg0
    add     .LT26, %reg0
    jmp     [%reg0]
.L58:
    movl    1, %reg0
    movl    %reg0, [%rbp-40]
    movl    4, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L63
.L55:
    movl    6, %reg0
    movl    %reg0, [%rbp-24]
    movl    [%rbp-48], %reg0
    movl    [%rbp-48], %reg1
    sub     1, %reg1
    movl    %reg1, [%rbp-48]
    jmp     .L63
.L62:
    movl    7, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L63
.L63:
    jmp     .L17
.L14:
    movl    [%rbp+8], %reg0
    add     [%rbp-48], %reg0
    movb    [%reg0], %reg0
    sxbl    %reg0
.section    .rodata
.LT27:
    .long   .L64
.section    .text
    sub     88, %reg0
    cmp     1, %reg0
    jae     .L65
    lsl     2, %reg0
    add     .LT27, %reg0
    jmp     [%reg0]
.L65:
.section    .rodata
.LT28:
    .long   .L64
.section    .text
    sub     12, %reg0
    cmp     1, %reg0
    jae     .L66
    lsl     2, %reg0
    add     .LT28, %reg0
    jmp     [%reg0]
.L66:
.section    .rodata
.LT29:
    .long   .L67
    .long   .L64
.section    .text
    sub     4, %reg0
    cmp     2, %reg0
    jae     .L68
    lsl     2, %reg0
    add     .LT29, %reg0
    jmp     [%reg0]
.L68:
.section    .rodata
.LT30:
    .long   .L64
.section    .text
    sub     7, %reg0
    cmp     1, %reg0
    jae     .L69
    lsl     2, %reg0
    add     .LT30, %reg0
    jmp     [%reg0]
.L69:
.section    .rodata
.LT31:
    .long   .L64
.section    .text
    sub     6, %reg0
    cmp     1, %reg0
    jae     .L70
    lsl     2, %reg0
    add     .LT31, %reg0
    jmp     [%reg0]
.L70:
.section    .rodata
.LT32:
    .long   .L64
.section    .text
    sub     3, %reg0
    cmp     1, %reg0
    jae     .L71
    lsl     2, %reg0
    add     .LT32, %reg0
    jmp     [%reg0]
.L67:
    movl    2, %reg0
    movl    %reg0, [%rbp-40]
    movl    6, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L72
.L64:
    movl    6, %reg0
    movl    %reg0, [%rbp-24]
    movl    [%rbp-48], %reg0
    movl    [%rbp-48], %reg1
    sub     1, %reg1
    movl    %reg1, [%rbp-48]
    jmp     .L72
.L71:
    movl    7, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L72
.L72:
    jmp     .L17
.L15:
    movl    [%rbp+8], %reg0
    add     [%rbp-48], %reg0
    movb    [%reg0], %reg0
    sxbl    %reg0
.section    .rodata
.LT33:
    .long   .L73
.section    .text
    sub     88, %reg0
    cmp     1, %reg0
    jae     .L74
    lsl     2, %reg0
    add     .LT33, %reg0
    jmp     [%reg0]
.L74:
.section    .rodata
.LT34:
    .long   .L73
.section    .text
    sub     12, %reg0
    cmp     1, %reg0
    jae     .L75
    lsl     2, %reg0
    add     .LT34, %reg0
    jmp     [%reg0]
.L75:
.section    .rodata
.LT35:
    .long   .L73
.section    .text
    sub     5, %reg0
    cmp     1, %reg0
    jae     .L76
    lsl     2, %reg0
    add     .LT35, %reg0
    jmp     [%reg0]
.L76:
.section    .rodata
.LT36:
    .long   .L77
.section    .text
    sub     3, %reg0
    cmp     1, %reg0
    jae     .L78
    lsl     2, %reg0
    add     .LT36, %reg0
    jmp     [%reg0]
.L78:
.section    .rodata
.LT37:
    .long   .L73
.section    .text
    sub     3, %reg0
    cmp     1, %reg0
    jae     .L79
    lsl     2, %reg0
    add     .LT37, %reg0
    jmp     [%reg0]
.L79:
.section    .rodata
.LT38:
    .long   .L73
.section    .text
    sub     6, %reg0
    cmp     1, %reg0
    jae     .L80
    lsl     2, %reg0
    add     .LT38, %reg0
    jmp     [%reg0]
.L80:
.section    .rodata
.LT39:
    .long   .L73
.section    .text
    sub     3, %reg0
    cmp     1, %reg0
    jae     .L81
    lsl     2, %reg0
    add     .LT39, %reg0
    jmp     [%reg0]
.L77:
    movl    4, %reg0
    movl    %reg0, [%rbp-40]
    movl    6, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L82
.L73:
    movl    6, %reg0
    movl    %reg0, [%rbp-24]
    movl    [%rbp-48], %reg0
    movl    [%rbp-48], %reg1
    sub     1, %reg1
    movl    %reg1, [%rbp-48]
    jmp     .L82
.L81:
    movl    7, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L82
.L82:
    jmp     .L17
.L16:
    movl    [%rbp+8], %reg0
    add     [%rbp-48], %reg0
    movb    [%reg0], %reg0
    sxbl    %reg0
.section    .rodata
.LT40:
    .long   .L83
.section    .text
    sub     88, %reg0
    cmp     1, %reg0
    jae     .L84
    lsl     2, %reg0
    add     .LT40, %reg0
    jmp     [%reg0]
.L84:
.section    .rodata
.LT41:
    .long   .L85
    .long   .L86
.section    .text
    sub     11, %reg0
    cmp     2, %reg0
    jae     .L87
    lsl     2, %reg0
    add     .LT41, %reg0
    jmp     [%reg0]
.L87:
.section    .rodata
.LT42:
    .long   .L86
.section    .text
    sub     6, %reg0
    cmp     1, %reg0
    jae     .L88
    lsl     2, %reg0
    add     .LT42, %reg0
    jmp     [%reg0]
.L88:
.section    .rodata
.LT43:
    .long   .L89
    .long   .L90
.section    .text
    sub     6, %reg0
    cmp     2, %reg0
    jae     .L91
    lsl     2, %reg0
    add     .LT43, %reg0
    jmp     [%reg0]
.L91:
.section    .rodata
.LT44:
    .long   .L92
    .long   .L93
    .long   .L94
.section    .text
    sub     4, %reg0
    cmp     3, %reg0
    jae     .L95
    lsl     2, %reg0
    add     .LT44, %reg0
    jmp     [%reg0]
.L95:
.section    .rodata
.LT45:
    .long   .L96
.section    .text
    sub     5, %reg0
    cmp     1, %reg0
    jae     .L93
    lsl     2, %reg0
    add     .LT45, %reg0
    jmp     [%reg0]
.L86:
    movl    0, %reg0
    movl    %reg0, [%rbp-44]
    movl    0, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L97
.L94:
    movl    1, %reg0
    movl    %reg0, [%rbp-44]
    movl    0, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L97
.L96:
    movl    2, %reg0
    movl    %reg0, [%rbp-44]
    movl    0, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L97
.L83:
    movl    3, %reg0
    movl    %reg0, [%rbp-44]
    movl    0, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L97
.L89:
    movl    4, %reg0
    movl    %reg0, [%rbp-44]
    movl    0, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L97
.L85:
    movl    5, %reg0
    movl    %reg0, [%rbp-44]
    movl    0, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L97
.L92:
    movl    6, %reg0
    movl    %reg0, [%rbp-44]
    movl    0, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L97
.L90:
    movl    7, %reg0
    movl    %reg0, [%rbp-44]
    movl    0, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L97
.L93:
    movl    7, %reg0
    movl    %reg0, [%rbp-24]
    jmp     .L97
.L97:
    sub     16, %rsp
    movl    37, %reg0
    movl    %reg0, [%rsp]
    call    putchar
    add     16, %rsp
    sub     16, %rsp
    movl    [%rbp-32], %reg0
    movl    %reg0, [%rsp]
    call    printdec
    add     16, %rsp
    sub     16, %rsp
    movl    32, %reg0
    movl    %reg0, [%rsp]
    call    putchar
    add     16, %rsp
    sub     16, %rsp
    movl    [%rbp-36], %reg0
    movl    %reg0, [%rsp]
    call    printdec
    add     16, %rsp
    sub     16, %rsp
    movl    32, %reg0
    movl    %reg0, [%rsp]
    call    putchar
    add     16, %rsp
    sub     16, %rsp
    movl    [%rbp-40], %reg0
    movl    %reg0, [%rsp]
    call    printdec
    add     16, %rsp
    sub     16, %rsp
    movl    32, %reg0
    movl    %reg0, [%rsp]
    call    putchar
    add     16, %rsp
    sub     16, %rsp
    movl    [%rbp-44], %reg0
    movl    %reg0, [%rsp]
    call    printdec
    add     16, %rsp
    sub     16, %rsp
    movl    32, %reg0
    movl    %reg0, [%rsp]
    call    putchar
    add     16, %rsp
    sub     16, %rsp
    movl    37, %reg0
    movl    %reg0, [%rsp]
    call    putchar
    add     16, %rsp
    jmp     .L17
.L17:
    movl    [%rbp-48], %reg0
    movl    [%rbp-48], %reg1
    add     1, %reg1
    movl    %reg1, [%rbp-48]
    jmp     .L8
.L9:
    movl    [%rbp-20], %reg0
    add     48, %rsp
    popl    %rbp
    ret     

get_variadic_param:
    pushl   %rbp
    movl    %rsp, %rbp
    sub     8, %rsp
    movl    [%rbp+8], %reg0
    movl    [%reg0], %reg0
    add     [%rbp+16], %reg0
    sub     1, %reg0
    idiv    [%rbp+16], %reg0
    imul    [%rbp+16], %reg0
    movl    %reg0, [%rbp-4]
    movl    [%rbp+8], %reg0
    movl    [%rbp+8], %reg1
    movl    [%reg1], %reg1
    add     [%rbp+12], %reg1
    movl    %reg1, [%reg0]
    movl    %rbp, %reg0
    add     [%rbp-4], %reg0
    add     8, %rsp
    popl    %rbp
    ret     

.end