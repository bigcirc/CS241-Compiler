
	lis $10
	.word -1
	lis $11
	.word 1
	
	lw $5, 0($1)
	beq $5, $10, end


gotonode:


end:
	jr $31