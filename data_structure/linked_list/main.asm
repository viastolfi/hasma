%include "ll.inc"

section .bss
  ll resb LL_size

section .text
global _start

_start:
  mov rdi, ll
  mov rsi, 8
  call _ll_init 

  cmp rax, -1
  je .panic

  cmp qword [ll + LL.head], 0
  jne .panic

  cmp qword [ll + LL.esize], 8
  jne .panic
  
  mov rax, 60
  mov rdi, 0
  syscall

.panic:
  mov rax, 60
  mov rdi, 1
  syscall

