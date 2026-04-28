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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; add new element at the end of a linked list
;; rdi -> pointer to the LL structure
;; rsi -> datas
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_ll_push:
  push rbx
  push r12
  push r13
  push r14

  push rbp
  mov rbp, rsp
  sub rsp, 32

  mov rbx, rdi
  mov [rbp - 8], rsi

  ; first we allocate place for the NODE struc
  mov rax, 9
  mov rdi, 0
  mov rsi, NODE_size
  mov rdx, PROT_READ | PROT_WRITE
  mov r10, 0x22
  mov r8, -1
  mov r9, 0
  syscall

  cmp rax, -1
  je .error

  mov [rbp - 16], rax

  mov r13, qword [rbp - 16]
  mov [rbx + LL.head], r13

  ; allocate place for the datas
  mov rax, 9
  mov rdi, 0
  mov rsi, qword [rbx + LL.esize]
  mov rdx, PROT_READ | PROT_WRITE
  mov r10, 0x22
  mov r8, -1
  mov r9, 0
  syscall

  cmp rax, -1
  je .error

  mov r12, qword [rbp - 8] 
  mov r13, [rbx + LL.esize]
  xor rdx, rdx

.copy:
  cmp rdx, r13
  je .done

  mov r14b, byte [r12 + rdx]
  mov byte [rax + rdx], r14b

  inc rdx
  jmp .copy

.done:
  mov r13, [rbx + LL.head]
  mov [r13 + NODE.ptr], rax
  mov qword [r13 + NODE.next], 0
  add rsp, 32
  xor rax, rax

  pop rbp
  pop r14
  pop r13
  pop r12
  pop rbx

  ret
.error:
  add rsp, 32
  mov rax, -1
  pop rbp
  pop r14
  pop r13
  pop r12
  pop rbx
  ret
