;----------------------------------
;Instruction commands file
;----------------------------------

;Start


.extern ReperesntMaxValidLabelLength31
.extern globalExternalLabel

label1: .entry reperesntmaxvalidlabellength31
label2: .entry instruction1
.entry instruction2
.entry instruction3

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
blt $5, $24, instruction3
beq $5, $24, instruction1


lb $7, -28, $18
lh $7, -28, $18
lw $7, -28, $18
sb $9, 34, $2
sh $9, 34, $2
sw $9, 34, $2

jmp END
END: stop
.entry END
reperesntmaxvalidlabellength31: .db 30000

; no effect for label which is defined twich as entry or external.
;for example:
.extern reperesntmaxvalidlabellength31
.extern ReperesntMaxValidLabelLength31
