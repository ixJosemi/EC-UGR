.data                               # data
msg:   .string "hello world!\n"
tam:   .quad . - msg                # string size

.text                               # code
       .global _start               # starts here

_start:                             # label

write: mov $1,    %rax              # write
       mov $1,    %rdi              # stdout; pointer to buff
       mov $msg,  %rsi              # text; no access memory
       mov tam,   %rdx              # size; access memory
       syscall                      # system call

exit:  mov %rax,  %rdi              # return value; rax because we're in 64b
       mov $60,   %rax              # exit; decimal; usually = exit 0
       syscall                      # system call
