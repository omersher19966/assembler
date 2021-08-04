;-------------------------------------
;Assembly File for First Pass Error Messages
;-------------------------------------

;line errors
move $3, $20 ,
.db 70000, 800000, 123010230120301230, 321509824135001431234, 2134980213740821375

;label errors

123label: add $5, $31, $1
aboveeee_maxxxx_labellllll_length: add $5, $31, $1
asciz: add $5, $31, $1
test: add $5, $31, $1
test: add $5, $31, $1

; Operands errors
jmp 12number
.asciz "abcd", "abcd"
add $2, $3
sub
add $3, ,$20

; Register errors
add 31, $5, $41
add $50, $5, $1

; Strings and Numbers errors
.asciz "abec""asdqwe"
.db 9.5

;Unrecognized commands
label:add $3,$4, $7
test $4, $9


