%include "strcat.inc"
%include "strlen.inc"

section .data
  hello db "Hello", 0
  world db "World", 0

section .text
global _start

_start:
  mov rdi, hello
  mov rsi, world

  call _strcat
  mov r10, rax

  mov rdi, r10
  call _strlen
  mov r11, rax

  mov rax, 1
  mov rdi, 1
  mov rsi, r10
  mov rdx, r11
  syscall

_program_end:
  mov rax, 60
  mov rdi, 0
  syscall
