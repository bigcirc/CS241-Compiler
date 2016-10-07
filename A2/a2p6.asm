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
print:	
	add $5, $5, $7
	sw $5, 0($8)
	beq $0, $0, loop
checklead:
	beq $5, $0, loop	;check for leading 0, if true, keep looping
	add $14, $14, $10	;else change boolean
	beq $0, $0, print	;and print number, then go back to loop

endzero:
	sw $7, 0($8)
end:
	sw $10, 0($8)
	jr $31

