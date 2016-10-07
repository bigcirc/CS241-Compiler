lis $4
.word 1
lis $3
.word -1
beq $2, $0, 7
lis $4
.word 4
mult $2, $4
mflo $5
sub $5, $5, $4
add $6, $1, $5
lw $3, 0($6)
jr $31
	