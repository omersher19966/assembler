;file ps.as
    ;sample source code
.entry Next

add $3, $5, $9
sw $0, 4, $10
jmp Next
.db 259, -76, 32
Next: move $20,$4
.asciz "strlen"
.asciz "abcd"
