;file ps.as
;sample source code

.entry Next
.extern wNumber
STR: .asciz "aBcdgsfjklkjsdfl;kjsd;fll;sfdgl;ksjdfgl;kjl;sfdjgl;kdjfslksfdlkjsfdl;"
MAIN: add $3, $5, $9
LOOP: ori $9, -5, $2
la val1
jmp Next
Next: move $20, $4
LIST: .db 6.321,-9
bgt $4,$2,END
la K
sw 0, 4.6, $10
bne $31 ,$9,LOOP
call val1
jmpp $4
laa wNumber
.extern val1
.dh 27056
K: .dw 31,-12
END: stop
.entry K

