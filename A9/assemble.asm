;Prologue
.import print
lis $4
.word 4
lis $11
.word 1
sub $29, $30, $4
sw $1, -4($30)
sub $30, $30, $4
sw $2, -4($30)
sub $30, $30, $4
;GenCode
lw $3, 0($29)
sw $3, -4($30)
sub $30, $30, $4
lw $3, -4($29)
add $30, $30, $4
lw $5, -4($30)
slt $6, $5, $3
slt $7, $3, $5
add $3, $6, $7
sub $3, $11, $3
beq $3, $0, else0
lis $3
.word 1
sw $3, 0($29)         ;variable: a
beq $0, $0, endif0
else0:
lis $3
.word 0
sw $3, 0($29)         ;variable: a
endif0:
lw $3, 0($29)
;Epilogue
add $30, $29, $4
jr $31
