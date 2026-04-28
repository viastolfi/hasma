%include "ll.inc"

section .data
  val0 dq 10

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

  cmp qword [rel ll + LL.head], 0
  jne .panic

  cmp qword [rel ll + LL.esize], 8
  jne .panic

  mov rdi, ll
  mov rsi, val0
  call _ll_push

  cmp qword [rel ll + LL.head], 0
  je .panic

  mov r12, qword [rel ll + LL.head]
  mov r12, qword [r12 + NODE.ptr]
  cmp qword [r12], 10
  jne .panic
  
  mov rax, 60
  mov rdi, 0
  syscall

.panic:
  mov rax, 60
  mov rdi, 1
  syscall

