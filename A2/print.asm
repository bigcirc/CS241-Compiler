print:
	sw $1, -4($30)
	sw $2, -8($30)
	sw $3, -12($30)
	sw $4, -16($30)
	sw $5, -20($30)
	sw $6, -24($30)
	sw $7, -28($30)
	sw $8, -32($30)
	sw $9, -36($30)
	sw $10, -40($30)
	sw $11, -44($30)
	sw $12, -48($30)
	sw $13, -52($30)
	sw $14, -56($30)	
	lis $2
	.word 56
	sub $30, $30, $2
	
	lis $4
	.word 1000000000	;divisor
	lis $7
	.word 48		;offset dec to ascii
	lis $8
	.word 0xffff000c	;print address
	lis $9			;2^32
	.word 0xffffffff
	lis $10
	.word 10		;divide divisor and newline
	lis $11
	.word 1
	lis $13
	.word 45

	beq $1, $0, endzero
	
	add $6, $1, $0		;copy number to $6
	slt $12, $6, $0		;sets $12 to 1 if number is negative
	beq $12, $0, loop 
	
negative:	
	sub $6, $9, $6		;convert negative number
	add $6, $6, $11		;have to add 1 since $9 cannot store max value
	sw $13, 0($8)
	
loop:	
	beq $4, $0, end
	div $6, $4		
	mflo $5
	mfhi $6

	div $4, $10
	mflo $4
	
	beq $14, $0, checklead
printdigit:	
	add $5, $5, $7
	sw $5, 0($8)
	beq $0, $0, loop
checklead:
	beq $5, $0, loop	;check for leading 0, if true, keep looping
	add $14, $14, $10	;else change boolean
	beq $0, $0, printdigit	;and print number, then go back to loop

endzero:
	sw $7, 0($8)
end:
	sw $10, 0($8)

	add $30, $30, $2
        lw $1, -4($30)
	lw $2, -8($30)
	lw $3, -12($30)
	lw $4, -16($30)
	lw $5, -20($30)
	lw $6, -24($30)
	lw $7, -28($30)
	lw $8, -32($30)
	lw $9, -36($30)
	lw $10, -40($30)
	lw $11, -44($30)
	lw $12, -48($30)
	lw $13, -52($30)
	lw $14, -56($30)
	
	jr $31

