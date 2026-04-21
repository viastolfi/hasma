;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; concatenate two null terminated string
;; rdi -> char *
;; rsi -> char *
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_strcat:
  push r9

  mov r9, rdi
.find_end:
  cmp byte [rdi], 0
  je .start_concat

  inc rdi
  jmp .find_end

.start_concat:
  mov cl, byte [rsi]
  mov byte [rdi], cl

  cmp cl, 0
  je .end_concat

  inc rdi
  inc rsi
  jmp .start_concat

.end_concat:
  mov rax, r9
  pop r9
  ret 
