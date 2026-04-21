;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; get the length of a null terminated string
;; rdi -> char *
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_strlen:
  push rcx

  xor rcx, rcx

.count_loop: 
  cmp byte [rdi + rcx], 0
  je .count_end

  inc rcx
  jmp .count_loop

.count_end:
  mov rax, rcx
  pop rcx
  ret
