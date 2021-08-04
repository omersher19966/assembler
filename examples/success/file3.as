;----------------------------------
;All Instruction commands are in the file.
;----------------------------------

;Start


.extern ReperesntMaxValidLabelLength31
.extern globalExternalLabel

label1: .entry reperesntmaxvalidlabellength31
label2: .entry instruction1
.entry instruction2
.entry instruction3

reperesntmaxvalidlabellength31: .db 1, 2, 30000

;All assembly instructions:

; J instructions:
instruction1: call globalExternalLabel
la reperesntmaxvalidlabellength31
jmp $31

; R instructions:
instruction2: add $5, $24, $24
sub $1, $2, $3
and $1, $2, $3
or $3, $19, $8
nor $3, $19, $8
move $23, $2
mvhi $23, $2
mvlo $23, $2

;I instructions
instruction3: addi $9, -45, $8
subi $9, -45, $8
andi $9, -45, $8
ori $9, -45, $8
nori $9, -45, $8

blt $5, $24, instruction1
bgt $5, $24, instruction2
beq $5, $24, instruction3
bne $5, $24, test


lb $7, -28, $18
test: lh $7, -28, $18
lw $7, -28, $18
sb $9, 34, $2
sh $9, 34, $2
sw $9, 34, $2

jmp END
END: stop
.entry END


; no effect for label which is defined twich as entry or external.
;for example:
.entry reperesntmaxvalidlabellength31
.extern ReperesntMaxValidLabelLength31
