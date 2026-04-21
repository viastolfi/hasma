%define PROT_READ     0x01
%define PROT_WRITE    0x02 

struc DA
  .ptr:        resq 1
  .count:      resq 1
  .capacity:   resq 1 
  .esize:      resq 1
endstruc

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; init a da
;; rdi -> pointer to the DA structure
;; rsi -> element size in bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_da_init:
  push rbx
  push r12

  mov rbx, rdi
  mov r12, rsi

  mov rax, 9
  mov rdi, 0

  ; allocate 8 * esize bytes in memory
  mov rsi, 8 
  imul rsi, r12

  mov rdx, PROT_READ | PROT_WRITE
  mov r10, 0x22
  mov r8, -1
  mov r9, 0
  syscall

  cmp rax, -1
  je .error

  mov [rbx + DA.ptr], rax
  mov qword [rbx + DA.count], 0
  mov qword [rbx + DA.capacity], 8
  mov [rbx + DA.esize], r12

  xor rax, rax
  pop r12
  pop rbx
  ret
 
.error:
  mov rax, -1
  pop r12
  pop rbx
  ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; push element to a da
;; rdi -> pointer to the da
;; rsi -> pointer to the data we want to push
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_da_push:
  push rbx
  push r12
  push r13

  mov rbx, rdi
  mov r12, rsi

  mov rax, [rbx + DA.count]
  cmp rax, [rbx + DA.capacity]
  jl .has_space

  mov rdi, rbx 
  call _da_grow
  cmp rax, -1
  je .fail

.has_space:
  ; rax = ptr of the next element
  mov rax, [rbx + DA.count]
  imul rax, [rbx + DA.esize]
  add rax, [rbx + DA.ptr]

  mov rcx, [rbx + DA.esize]
  xor rdx, rdx
  
.copy:
  cmp rdx, rcx
  jge .done

  mov r13b, byte [r12 + rdx]
  mov byte [rax + rdx], r13b
  
  inc rdx 
  jmp .copy

.done:
  inc qword [rbx + DA.count]
  xor rax, rax

  pop r13
  pop r12
  pop rbx
  ret

.fail:
  mov rax, -1
  pop r13
  pop r12
  pop rbx
  ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; get element from a da at index i
;; rdi -> pointer to the DA structure
;; rsi -> index
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_da_get:
  push rbx

  mov rbx, rdi
  cmp rsi, [rbx + DA.count]
  jge .fail

  mov rax, rsi
  imul rax, [rbx + DA.esize]
  add rax, [rbx + DA.ptr]
  pop rbx
  ret

.fail:
  mov rax, -1
  pop rbx
  ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; remove element from da
;; rdi -> pointer to the DA structure
;; rsi -> index of the element we want to remove
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_da_remove:
  push rbx
  push r12
  push r13

  mov rbx, rdi
  mov r12, rsi

  call _da_get ; since _da_get already checks index out of bound
  cmp rax, -1
  je .fail

  mov r13, rax

  mov rdi, r13
  mov rsi, r13
  add rsi, [rbx + DA.esize]
  mov rdx, [rbx + DA.count]
  sub rdx, r12
  dec rdx
  imul rdx, [rbx + DA.esize]
  call _mem_copy

  dec qword [rbx + DA.count]

  xor rax, rax
  pop r13
  pop r12
  pop rbx
  ret

.fail:
  mov rax, -1
  pop r13
  pop r12
  pop rbx
  ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; grow a da
;; rdi -> pointer to the DA structure
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_da_grow:
  push rbx
  push r12
  push r13
  push r14
  push r15
  
  mov rbx, rdi  
  mov r12, [rbx + DA.ptr]

  mov r14, [rbx + DA.capacity]
  mov r15, [rbx + DA.capacity]
  sal r14, 1

  mov rax, 9
  mov rdi, 0
  mov rsi, r14
  imul rsi, [rbx + DA.esize]
  mov rdx, PROT_READ | PROT_WRITE
  mov r10, 0x22
  mov r8, -1
  mov r9, 0
  syscall

  cmp rax, -1
  je .fail
  mov r13, rax
  mov [rbx + DA.capacity], r14

  mov rdi, r13
  mov rsi, r12
  mov rdx, [rbx + DA.count]
  imul rdx, [rbx + DA.esize]
  call _mem_copy
  cmp rax, -1
  je .fail

  mov [rbx + DA.ptr], r13

  mov rax, 11
  mov rdi, r12
  mov rsi, r15
  imul rsi, [rbx + DA.esize]
  syscall
 
 .done:
  xor rax, rax
  pop r15
  pop r14
  pop r13
  pop r12
  pop rbx
  ret 

.fail:
  mov rax, -1
  pop r15
  pop r14
  pop r13
  pop r12
  pop rbx
  ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; copy a chunck of memory
;; rdi -> pointer to the start were we will copy
;; rsi -> pointer to the start of what we copy
;; rdx -> amount of bytes we are going to copy
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_mem_copy:
  cmp rdx, 32
  jl .qwords

  movdqu xmm0, [rsi]
  movdqu xmm1, [rsi + 16]
  movdqu [rdi], xmm0
  movdqu [rdi + 16], xmm1
  add rsi, 32
  add rdi, 32
  sub rdx, 32
  jmp _mem_copy

.qwords:
  cmp rdx, 8
  jl .bytes

  mov rax, [rsi]
  mov [rdi], rax
  add rsi, 8
  add rdi, 8
  sub rdx, 8
  jmp .qwords

.bytes:
  cmp rdx, 0
  je .done
  
  mov al, byte [rsi]
  mov byte [rdi], al
  inc rsi
  inc rdi
  dec rdx
  jmp .bytes

.done:
  xor rax, rax
  ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; free a da
;; rdi -> pointer to the DA structure
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_da_free:
  push rbx

  mov rbx, rdi
  mov rax, 11
  mov rdi, rbx
  mov rsi, [rbx + DA.capacity]
  imul rsi, [rbx + DA.esize]
  syscall

  cmp rax, -1
  je .fail

  mov qword [rbx + DA.capacity], 0
  mov qword [rbx + DA.count], 0
  mov qword [rbx + DA.esize], 0
  mov qword [rbx + DA.ptr], 0
  
  xor rax, rax
  pop rbx
  ret
.fail:
  pop rbx
  ret
