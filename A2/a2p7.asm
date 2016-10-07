	sw $31, -4($30)
	sw $1, -8($30)
	sw $3, -12($30)
	sw $4, -16($30)
	sw $5, -20($30)
	sw $6, -24($30)
	sw $11, -28($30)
	lis $31
	.word -28
	add $30, $30, $31

	lis $4
	.word 4
	lis $11
	.word 1
	
	add $5, $5, $1
	add $6, $6, $2

looparray:	
	beq $6, $0, endarray
	lw $1, 0($5)
	sub $6, $6, $11
	add $5, $5, $4
	
	lis $3
	.word print
	jalr $3
	beq $0, $0, looparray
	
endarray:	
	lis $31
	.word 28
	add $30, $30, $31
	lw $31, -4($30)
	lw $1, -8($30)
	lw $3, -12($30)
	lw $4, -16($30)
	lw $5, -20($30)
	lw $6, -24($30)
	lw $11, -28($30)
	
	jr $31