;----------------------------------
;All Directive commands are in the file.
;----------------------------------

;Start

.entry str
.entry STR
.entry numbers1
.entry numbers3

;numbers2 is not defined as entry record but 
;it still can be used in this file. for example:

call numbers2

str: .asciz "abcdefghijklmnopqrstuvwxyz"
STR: .asciz "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

numbers1: .dh 	32767, 	255, 512
;notice that 257 is bigger byte scope and it will be cut to 1.
numbers2: .db   30, 40, 257
numbers3: .dw -30000, 96789

