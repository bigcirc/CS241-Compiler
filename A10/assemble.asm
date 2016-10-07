;Prologue
.import print
.import init
.import new
.import delete
lis $4
.word 4
lis $11
.word 1
sub $29, $30, $4
sw $1, -4($30)
sub $30, $30, $4
sw $2, -4($30)
sub $30, $30, $4
;Prologue init
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word init
jalr $5
add $30, $30, $4
lw $31, -4($30)
;GenCode
add $3, $11, $0
sw $3, -8($29)         ;variable pointer: c
lis $3
.word 1
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word new
jalr $5
add $30, $30, $4
lw $31, -4($30)
bne $3, $0, 1
add $3, $11, $0              ;SET POINTER TO NULL
sw $3, -8($29)         ;variable: c
lis $3
.word 1
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word print
jalr $5
add $30, $30, $4
lw $31, -4($30)
lis $3
.word 5
lw $3, -8($29)
add $6, $3, $0
lis $3
.word 5
sw $3, 0($6)
lis $3
.word 1
add $1, $3, $0
sw $31, -4($30)
sub $30, $30, $4
lis $5
.word print
jalr $5
add $30, $30, $4
lw $31, -4($30)
lw $3, -8($29)
lw $3, 0($3)
;Epilogue
add $30, $29, $4
jr $31
