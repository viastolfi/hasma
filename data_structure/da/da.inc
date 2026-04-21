%define PROD_READ     0x01
%define PROD_WRITE    0x02 

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
  push r11

  mov rbx, rdi
  mov r11, rsi

  mov rax, 9
  mov rdi, 0

  ; allocate 8 * esize bytes in memory
  mov rsi, 8 
  imul rsi, r11 

  mov rdx, PROD_READ | PROD_WRITE
  mov r10, 0x22
  mov r8, -1
  mov r9, 0
  syscall

  cmp rax, -1
  je .error

  mov [rbx + DA.ptr], rax
  mov [rbx + DA.count], 0
  mov [rbx + DA.capacity], 8
  mov [rbx + DA.esize], r11

  xor rax, rax
  pop r11
  pop rbx
  ret
 
.error:
  mov rax, -1
  pop r11
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
  mov rdx, PROD_READ | PROD_WRITE
  mov r10, 0x22
  mov r8, -1
  mov r9, 0
  syscall

  cmp rax, -1
  je .fail
  mov r13, rax
  mov [rbx + DA.capacity], r14

  mov rdi, r12
  mov rsi, r13
  mov rdx, [rbx + DA.count]
  imul rdx, [rbx + DA.esize]
  call _mem_copy
  cmp rax, -1
  je .fail

  mov [rbx + DA.ptr], r13

  mov rdi, r12
  mov rsi, r15
  imul rsi, [rbx + DA.esize]
  call _da_free
  cmp rax, -1
  je .fail
 
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
