lis $5
.word 4
lis $7
.word 1
add $4, $2, $0
lw $3, 0($1)
add $6, $1, $0
add $6, $6, $5
sub $4, $4, $7
beq $4, $0, 5
lw $8, 0($6)
slt $9, $3, $8
beq $9, $0, 1
lw $3, 0($6)
beq $0, $0, -8
jr $31