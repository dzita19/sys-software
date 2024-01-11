.section ivt

.long _start
.long _exit
.long _exit
.long _exit
.skip 48

.text

.extern main

_start: # _start is the entry point, set inside the reset handler
    movl    0, %rbp              # effectively RBP := 0, mark the end of stack frames
    call    main                 # start executing main
_exit:
    hlt                          # terminate the program