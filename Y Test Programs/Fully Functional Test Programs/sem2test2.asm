			.text
			.globl			__start
__start:
			jal			_main
			li			$v0, 			10
			syscall
_main:
			li			$v0, 			4
			la			$a0, 			L5
			syscall
			li			$v0, 			4
			la			$a0, 			L6
			syscall
			li			$v0, 			4
			la			$a0, 			L7
			syscall
			li			$v0, 			5
			syscall
			move			$t0, 			$v0
			sw			$t0, 			_x
			li			$v0, 			4
			la			$a0, 			L8
			syscall
			li			$v0, 			5
			syscall
			move			$t0, 			$v0
			sw			$t0, 			_y
			lw			$t0, 			_y
			lw			$t1, 			_x
			sub			$t2, 			$t0, 			$t1
			sw			$t2, 			_start
			li			$t0, 			0
			sw			$t0, 			_counter
L21:
			lw			$t0, 			_counter
			lw			$t1, 			_start
			bne			$t0, 			$t1, 			L9
			b			L22
L9:
			lw			$t0, 			_counter
			lw			$t1, 			L2
			bne			$t0, 			$t1, 			L10
			li			$v0, 			4
			la			$a0, 			L11
			syscall
			b			L20
L10:
			lw			$t0, 			_counter
			lw			$t1, 			L3
			bne			$t0, 			$t1, 			L12
			li			$v0, 			4
			la			$a0, 			L13
			syscall
			b			L19
L12:
			lw			$t0, 			_counter
			lw			$t1, 			L1
			bne			$t0, 			$t1, 			L14
			li			$v0, 			4
			la			$a0, 			L15
			syscall
			b			L18
L14:
			lw			$t0, 			_counter
			lw			$t1, 			L4
			bne			$t0, 			$t1, 			L16
			li			$v0, 			4
			la			$a0, 			L17
			syscall
L16:
L18:
L19:
L20:
			lw			$t0, 			_counter
			addi			$t0, 			$t0, 			1
			sw			$t0, 			_counter
			b			L21
L22:
			lw			$t0, 			_counter
			li			$t1, 			1000
			slt			$t0, 			$t0, 			$t1
			beq			$t0, 			$0, 			L23
			lw			$t0, 			_counter
			li			$t1, 			0
			sgt			$t0, 			$t0, 			$t1
			beq			$t0, 			$0, 			L23
			li			$v0, 			4
			la			$a0, 			L25
			syscall
			lw			$t0, 			_counter
			li			$v0, 			1
			move			$a0, 			$t0
			syscall
			li			$v0, 			4
			la			$a0, 			L26
			syscall
			b			L28
L23:
			li			$v0, 			4
			la			$a0, 			L27
			syscall
L28:
			jr			$ra
			.data
_x:			.word			0
_y:			.word			0
_start:			.word			0
_counter:			.word			0
__iobuf:			.space			4
L13:			.asciiz			"In total another decade has passed!\n"
L8:			.asciiz			"Input an end year:\n"
L27:			.asciiz			"You went around the sun so many times I lost track....\n"
L6:			.asciiz			"You are the earth, slowly revolving around the sun\n"
L7:			.asciiz			"Input a start year:\n"
L26:			.asciiz			" times\n"
L11:			.asciiz			"In total Another century has passed!\n"
L25:			.asciiz			"You went around the sun a total of "
L5:			.asciiz			"Welcome to years around the sun!\n"
L15:			.asciiz			"In total another millenium has passed!\n"
L17:			.asciiz			"First year went by quick!\n"
L4:			.word			1
L3:			.word			10
L2:			.word			100
L1:			.word			1000
