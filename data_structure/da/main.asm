%include "da.inc"

section .bss
  da resb DA_size         

section .data
  val0  dq 10
  val1  dq 20
  val2  dq 30
  val3  dq 40
  val4  dq 50
  val5  dq 60
  val6  dq 70
  val7  dq 80
  val8  dq 90   
  val9  dq 100

section .text
global _start

_start:
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; init
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  mov rdi, da
  mov rsi, 8          ; esize = 8 octets (qword)
  call _da_init
  cmp rax, -1
  je .panic

  mov rdi, da
  mov rsi, val0
  call _da_push
  cmp rax, -1
  je .panic

  mov rdi, da
  mov rsi, val1
  call _da_push
  cmp rax, -1
  je .panic

  mov rdi, da
  mov rsi, val2
  call _da_push
  cmp rax, -1
  je .panic

  mov rdi, da
  mov rsi, val3
  call _da_push
  cmp rax, -1
  je .panic

  mov rdi, da
  mov rsi, val4
  call _da_push
  cmp rax, -1
  je .panic

  mov rdi, da
  mov rsi, val5
  call _da_push
  cmp rax, -1
  je .panic

  mov rdi, da
  mov rsi, val6
  call _da_push
  cmp rax, -1
  je .panic

  mov rdi, da
  mov rsi, val7
  call _da_push
  cmp rax, -1
  je .panic

  ;; trigger grow
  mov rdi, da
  mov rsi, val8
  call _da_push
  cmp rax, -1
  je .panic

  mov rdi, da
  mov rsi, val9
  call _da_push
  cmp rax, -1
  je .panic

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; get index 0 -> should return 10
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  mov rdi, da
  mov rsi, 0
  call _da_get
  cmp rax, -1
  je .panic
  mov rax, [rax] 
  cmp rax, 10
  jne .panic

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; get index 4 -> should return 50
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  mov rdi, da
  mov rsi, 4
  call _da_get
  cmp rax, -1
  je .panic
  mov rax, [rax]
  cmp rax, 50
  jne .panic

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; get out of bounds index, should return -1
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  mov rdi, da
  mov rsi, 99
  call _da_get
  cmp rax, -1
  jne .panic

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; remove index 0
  ;; after : [20, 30, 40, 50, 60, 70, 80, 90, 100]
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  mov rdi, da
  mov rsi, 0
  call _da_remove
  cmp rax, -1
  je .panic

  ; check that index 0 is now 20
  mov rdi, da
  mov rsi, 0
  call _da_get
  cmp rax, -1
  je .panic
  mov rax, [rax]
  cmp rax, 20
  jne .panic

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; remove last element
  ;; after : [20, 30, 40, 50, 60, 70, 80, 90]
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  mov rdi, da
  mov rsi, 8
  call _da_remove
  cmp rax, -1
  je .panic

  ; check new last index is now 90
  mov rdi, da
  mov rsi, 7
  call _da_get
  cmp rax, -1
  je .panic
  mov rax, [rax]
  cmp rax, 90
  jne .panic

  ; check that index 8 is now out of bounds
  mov rdi, da
  mov rsi, 8
  call _da_get
  cmp rax, -1
  jne .panic

  mov rdi, da
  call _da_free
  cmp rax, -1
  je .panic

  cmp qword [rel da + DA.ptr], 0
  jne .panic
  cmp qword [rel da + DA.count], 0
  jne .panic
  cmp qword [rel da + DA.capacity], 0
  jne .panic
  cmp qword [rel da + DA.esize], 0
  jne .panic

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ;; everything OK, return 0
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  mov rax, 60
  xor rdi, rdi
  syscall

.panic:
  mov rax, 60
  mov rdi, 1
  syscall
