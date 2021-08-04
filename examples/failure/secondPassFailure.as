;-------------------------------------
;Assembly File for Second Pass Error Messages
;-------------------------------------



;label errors

.extern externalLabel
beq $3, $4, externalLabel

jmp nonExistingLabel

;symbol cannot be defined as external and entry at the same time.
.entry externalLabel


;invalid operands number in entry command
.entry test, test1

