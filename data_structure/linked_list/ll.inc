%define PROT_READ     0x01
%define PROT_WRITE    0x02 

struc NODE
  .ptr:   resq 1
  .next:  resq 1
endstruc

struc LL
  .head:  resq 1
  .esize: resq 1
endstruc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; init a linked list
;; rdi -> pointer to the LL structure
;; rsi -> element size in bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_ll_init:
  push rbx

  mov rbx, rdi
  mov qword [rbx + LL.head], 0
  mov [rbx + LL.esize], rsi

  xor rax, rax
  pop rbx
  ret

;; TODO: fix this
_ll_push:
  push rbx
  push r12
  push r13

  mov rbx, rdi
  mov r12, rsi

  ; malloc
  mov rax, 9
  mov rdi, 0
  mov rsi, r12
  mov rdx, PROT_READ | PROT_WRITE
  mov r10, 0x22
  mov r8, -1
  mov r9, 0
  syscall

  cmp rax, -1
  je .error
  
  mov qword [r13 + NODE.ptr], rax
  mov qword [r13 + NODE.next], -1

  mov [rbx + LL.head], r13
  mov qword [rbx + LL.esize], r12

  xor rax, rax
  pop r13
  pop r12
  pop rbx
  ret

.error:
  mov rax, -1
  pop r13
  pop r12
  pop rbx
  ret


