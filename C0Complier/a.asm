.data
    global_var: .space 820
    str_0: .asciiz "empty parameter list & condition is a single expression"
    str_1: .asciiz "as plain as daylight 1\\n"
    str_2: .asciiz "as plain as daylight 2"
    str_3: .asciiz "Print an expression alone & print string with expression & all the <zi fu>"
    str_4: .asciiz "apples[0] is"
    str_5: .asciiz " !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
    str_6: .asciiz "Some empty sentences"
    str_7: .asciiz "sreg[count] is"
    str_8: .asciiz "if & switch & condition is a relation expression & all valid string & valid char,num"
    str_9: .asciiz "A valid <zi fu>"
    str_10: .asciiz "char 34 is not included!"
    str_11: .asciiz "Input two numbers to calculate GCD"
    str_12: .asciiz "Two input for GCD "
    str_13: .asciiz "Greatest Common Division of "
    str_14: .asciiz "is "
    str_15: .asciiz "100"
    str_16: .asciiz "Hello Worl"
    str_17: .asciiz "Empty Default Case"


.text
    addiu $fp, $sp, -4
    addiu $sp, $sp, -12
    jal func_5
    li $v0, 10
    syscall


func_1:
    li $t0, 2
    sw $t0, -0($fp)
    label_0:
    lw $t0, -0($fp)
    li $t1, 2
    mul $t0, $t0, $t1
    li $t1, 3
    mul $t0, $t0, $t1
    li $t1, 4
    div $t0, $t0, $t1
    beq $t0, $0, label_1
    la $t0, str_0
    lw $t1, -0($fp)
    move $a0, $t0
    li $v0, 4
    syscall
    move $a0, $t1
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $t0, -0($fp)
    li $t1, 1
    subu $t0, $t0, $t1
    sw $t0, -0($fp)
    j label_0
    label_1:
    li $t0, 1
    li $t1, 1
    addu $t0, $t0, $t1
    li $t1, 3
    slt $t0, $t0, $t1
    beq $t0, $0, label_2
    la $t0, str_1
    move $a0, $t0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    label_2:
    li $t0, 1
    li $t1, 1
    addu $t0, $t0, $t1
    li $t1, 1
    sgt $t0, $t0, $t1
    beq $t0, $0, label_3
    la $t0, str_2
    move $a0, $t0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    label_3:
    la $t0, str_3
    move $a0, $t0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $t0, 0
    li $t1, 1
    sll $t0, $t0, 2
    sw $t1, global_var+20($t0)
    li $t0, 1
    li $t1, 12340
    sll $t0, $t0, 2
    sw $t1, global_var+20($t0)
    li $t0, 2
    li $t1, 56789
    sll $t0, $t0, 2
    sw $t1, global_var+20($t0)
    li $t0, 97
    li $t1, 97
    li $t2, 0
    addu $t1, $t1, $t2
    seq $t0, $t0, $t1
    beq $t0, $0, label_4
    la $t0, str_4
    li $t1, 0
    sll $t1, $t1, 2
    lw $t1, global_var+20($t1)
    move $a0, $t0
    li $v0, 4
    syscall
    move $a0, $t1
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    label_4:
    li $t0, -1
    neg $t0, $t0
    li $t1, 10
    mul $t0, $t0, $t1
    li $t1, 10
    div $t0, $t0, $t1
    li $t1, 97
    li $t2, 10
    mul $t1, $t1, $t2
    li $t2, 97
    div $t1, $t1, $t2
    addu $t0, $t0, $t1
    li $t1, 0
    sll $t1, $t1, 2
    lw $t1, global_var+20($t1)
    addu $t0, $t0, $t1
    li $t1, 95
    li $t2, 43
    mul $t1, $t1, $t2
    li $t2, 45
    li $t3, 42
    addu $t2, $t2, $t3
    div $t1, $t1, $t2
    addu $t0, $t0, $t1
    li $t1, 47
    addu $t0, $t0, $t1
    li $t1, 65
    addu $t0, $t0, $t1
    li $t1, 66
    addu $t0, $t0, $t1
    li $t1, 67
    addu $t0, $t0, $t1
    li $t1, 68
    addu $t0, $t0, $t1
    li $t1, 69
    addu $t0, $t0, $t1
    li $t1, 70
    addu $t0, $t0, $t1
    li $t1, 71
    addu $t0, $t0, $t1
    li $t1, 72
    addu $t0, $t0, $t1
    li $t1, 73
    addu $t0, $t0, $t1
    li $t1, 74
    addu $t0, $t0, $t1
    li $t1, 75
    addu $t0, $t0, $t1
    li $t1, 76
    addu $t0, $t0, $t1
    li $t1, 77
    addu $t0, $t0, $t1
    li $t1, 78
    addu $t0, $t0, $t1
    li $t1, 79
    addu $t0, $t0, $t1
    li $t1, 80
    addu $t0, $t0, $t1
    li $t1, 81
    addu $t0, $t0, $t1
    li $t1, 82
    addu $t0, $t0, $t1
    li $t1, 83
    addu $t0, $t0, $t1
    li $t1, 84
    addu $t0, $t0, $t1
    li $t1, 85
    addu $t0, $t0, $t1
    li $t1, 86
    addu $t0, $t0, $t1
    li $t1, 87
    addu $t0, $t0, $t1
    li $t1, 88
    addu $t0, $t0, $t1
    li $t1, 89
    addu $t0, $t0, $t1
    li $t1, 90
    addu $t0, $t0, $t1
    li $t1, 97
    addu $t0, $t0, $t1
    li $t1, 98
    addu $t0, $t0, $t1
    li $t1, 99
    addu $t0, $t0, $t1
    li $t1, 100
    addu $t0, $t0, $t1
    li $t1, 101
    addu $t0, $t0, $t1
    li $t1, 102
    addu $t0, $t0, $t1
    li $t1, 103
    addu $t0, $t0, $t1
    li $t1, 104
    addu $t0, $t0, $t1
    li $t1, 105
    addu $t0, $t0, $t1
    li $t1, 106
    addu $t0, $t0, $t1
    li $t1, 107
    addu $t0, $t0, $t1
    li $t1, 108
    addu $t0, $t0, $t1
    li $t1, 109
    addu $t0, $t0, $t1
    li $t1, 110
    addu $t0, $t0, $t1
    li $t1, 111
    addu $t0, $t0, $t1
    li $t1, 112
    addu $t0, $t0, $t1
    li $t1, 113
    addu $t0, $t0, $t1
    li $t1, 114
    addu $t0, $t0, $t1
    li $t1, 115
    addu $t0, $t0, $t1
    li $t1, 116
    addu $t0, $t0, $t1
    li $t1, 117
    addu $t0, $t0, $t1
    li $t1, 118
    addu $t0, $t0, $t1
    li $t1, 119
    addu $t0, $t0, $t1
    li $t1, 120
    addu $t0, $t0, $t1
    li $t1, 121
    addu $t0, $t0, $t1
    li $t1, 122
    addu $t0, $t0, $t1
    move $a0, $t0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    la $t0, str_5
    move $a0, $t0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    jr $ra
    jr $ra


func_2:
    la $t0, str_6
    move $a0, $t0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $t0, 100
    move $v0, $t0
    jr $ra
    jr $ra


func_3:
    li $t0, 0
    lw $t1, -20($fp)
    sll $t0, $t0, 2
    subu $t0, $fp, $t0
    sw $t1, -40($t0)
    li $t0, 1
    sw $t0, -36($fp)
    label_5:
    lw $t0, -36($fp)
    li $t1, 10
    slt $t0, $t0, $t1
    beq $t0, $0, label_6
    lw $t0, -36($fp)
    lw $t1, -36($fp)
    li $t2, 1
    subu $t1, $t1, $t2
    sll $t1, $t1, 2
    subu $t1, $fp, $t1
    lw $t1, -40($t1)
    li $t2, 1
    addu $t1, $t1, $t2
    sll $t0, $t0, 2
    subu $t0, $fp, $t0
    sw $t1, -40($t0)
    lw $t0, -36($fp)
    li $t1, 1
    addu $t0, $t0, $t1
    sw $t0, -36($fp)
    j label_5
    label_6:
    li $t0, 0
    sw $t0, -36($fp)
    label_7:
    lw $t0, -36($fp)
    li $t1, 10
    slt $t0, $t0, $t1
    beq $t0, $0, label_8
    la $t0, str_7
    lw $t1, -36($fp)
    sll $t1, $t1, 2
    subu $t1, $fp, $t1
    lw $t1, -40($t1)
    move $a0, $t0
    li $v0, 4
    syscall
    move $a0, $t1
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $t0, -36($fp)
    li $t1, 1
    addu $t0, $t0, $t1
    sw $t0, -36($fp)
    j label_7
    label_8:
    li $t0, 0
    sw $t0, -32($fp)
    lw $t0, -8($fp)
    sw $t0, -24($fp)
    lw $t0, -16($fp)
    lw $t1, -20($fp)
    lw $t2, -12($fp)
    div $t1, $t1, $t2
    addu $t0, $t0, $t1
    sw $t0, global_var+4
    lb $t0, -0($fp)
    sb $t0, -28($fp)
    la $t0, str_8
    move $a0, $t0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    label_9:
    lb $t0, -28($fp)
    lb $t1, -4($fp)
    sle $t0, $t0, $t1
    beq $t0, $0, label_10
    lb $t0, -28($fp)
    li $t1, 0
    addu $t0, $t0, $t1
    li $t1, 34
    sne $t0, $t0, $t1
    beq $t0, $0, label_11
    lb $t0, -28($fp)
    li $t1, 0
    addu $t0, $t0, $t1
    li $t1, 32
    sge $t0, $t0, $t1
    beq $t0, $0, label_12
    lb $t0, -28($fp)
    li $t1, 95
    seq $t1, $t0, $t1
    beq $t1, $0, label_14
    li $t0, 1
    sw $t0, -32($fp)
    j label_13
    label_14:
    li $t1, 43
    seq $t1, $t0, $t1
    beq $t1, $0, label_15
    li $t0, 1
    sw $t0, -32($fp)
    j label_13
    label_15:
    li $t1, 45
    seq $t1, $t0, $t1
    beq $t1, $0, label_16
    li $t0, 1
    sw $t0, -32($fp)
    j label_13
    label_16:
    li $t1, 42
    seq $t1, $t0, $t1
    beq $t1, $0, label_17
    li $t0, 1
    sw $t0, -32($fp)
    j label_13
    label_17:
    li $t1, 47
    seq $t1, $t0, $t1
    beq $t1, $0, label_18
    li $t0, 1
    sw $t0, -32($fp)
    j label_13
    label_18:
    lb $t0, -28($fp)
    li $t1, 48
    subu $t0, $t0, $t1
    li $t1, 0
    sge $t0, $t0, $t1
    beq $t0, $0, label_19
    lb $t0, -28($fp)
    li $t1, 57
    subu $t0, $t0, $t1
    li $t1, 0
    sle $t0, $t0, $t1
    beq $t0, $0, label_20
    li $t0, 1
    sw $t0, -32($fp)
    label_20:
    label_19:
    lb $t0, -28($fp)
    li $t1, 65
    subu $t0, $t0, $t1
    li $t1, 0
    sge $t0, $t0, $t1
    beq $t0, $0, label_21
    lb $t0, -28($fp)
    li $t1, 90
    subu $t0, $t0, $t1
    li $t1, 0
    sle $t0, $t0, $t1
    beq $t0, $0, label_22
    li $t0, 1
    sw $t0, -32($fp)
    label_22:
    label_21:
    lb $t0, -28($fp)
    li $t1, 97
    subu $t0, $t0, $t1
    li $t1, 0
    sge $t0, $t0, $t1
    beq $t0, $0, label_23
    lb $t0, -28($fp)
    li $t1, 122
    subu $t0, $t0, $t1
    li $t1, 0
    sle $t0, $t0, $t1
    beq $t0, $0, label_24
    li $t0, 1
    sw $t0, -32($fp)
    label_24:
    label_23:
    label_13:
    label_12:
    lb $t0, -28($fp)
    lb $t1, -0($fp)
    subu $t0, $t0, $t1
    lb $t1, -28($fp)
    sll $t0, $t0, 2
    sb $t1, global_var+420($t0)
    lw $t0, -32($fp)
    li $t1, 1
    seq $t0, $t0, $t1
    beq $t0, $0, label_25
    la $t0, str_9
    lb $t1, -28($fp)
    lb $t2, -0($fp)
    subu $t1, $t1, $t2
    sll $t1, $t1, 2
    lb $t1, global_var+420($t1)
    move $a0, $t0
    li $v0, 4
    syscall
    move $a0, $t1
    li $v0, 11
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $t0, 0
    sw $t0, -32($fp)
    label_25:
    label_11:
    lb $t0, -28($fp)
    li $t1, 0
    addu $t0, $t0, $t1
    li $t1, 34
    li $t2, -34
    addu $t1, $t1, $t2
    li $t2, 34
    addu $t1, $t1, $t2
    seq $t0, $t0, $t1
    beq $t0, $0, label_26
    la $t0, str_10
    move $a0, $t0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    label_26:
    li $t0, 99
    sb $t0, -28($fp)
    j label_9
    label_10:
    lw $t0, global_var+4
    li $t1, 1
    addu $t0, $t0, $t1
    sw $t0, global_var+4
    li $t0, 97
    sb $t0, global_var+8
    lb $t0, global_var+8
    li $t1, 1
    addu $t0, $t0, $t1
    sw $t0, global_var+4
    li $t0, 98
    sb $t0, global_var+12
    lb $t0, global_var+12
    li $t1, 2
    addu $t0, $t0, $t1
    sw $t0, global_var+4
    li $t0, 99
    sb $t0, global_var+12
    li $t0, 99
    sb $t0, global_var+8
    jr $ra
    jr $ra


func_4:
    lw $t0, -4($fp)
    li $t1, 0
    seq $t0, $t0, $t1
    beq $t0, $0, label_27
    lw $t0, -0($fp)
    move $v0, $t0
    jr $ra
    label_27:
    lw $t0, -4($fp)
    li $t1, 0
    sne $t0, $t0, $t1
    beq $t0, $0, label_28
    lw $t0, -4($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
    lw $t0, -0($fp)
    lw $t1, -0($fp)
    lw $t2, -4($fp)
    div $t1, $t1, $t2
    lw $t2, -4($fp)
    mul $t1, $t1, $t2
    subu $t0, $t0, $t1
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
    addiu $sp, $sp, -48
    sw $fp, 4($sp)
    sw $ra, 0($sp)
    sw $t0, 8($sp)
    sw $t1, 12($sp)
    sw $t2, 16($sp)
    sw $t3, 20($sp)
    sw $t4, 24($sp)
    sw $t5, 28($sp)
    sw $t6, 32($sp)
    sw $t7, 36($sp)
    sw $t8, 40($sp)
    sw $t9, 44($sp)
    addiu $fp, $sp, 52
    jal func_4
    lw $fp, 4($sp)
    lw $ra, 0($sp)
    lw $t0, 8($sp)
    lw $t1, 12($sp)
    lw $t2, 16($sp)
    lw $t3, 20($sp)
    lw $t4, 24($sp)
    lw $t5, 28($sp)
    lw $t6, 32($sp)
    lw $t7, 36($sp)
    lw $t8, 40($sp)
    lw $t9, 44($sp)
    addiu $sp, $sp, 56
    move $t0, $v0
    move $v0, $t0
    jr $ra
    label_28:
    jr $ra


func_5:
    la $t0, str_11
    move $a0, $t0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $v0, 5
    syscall
    sw $v0, -0($fp)
    li $v0, 5
    syscall
    sw $v0, -4($fp)
    la $t0, str_12
    lw $t1, -0($fp)
    move $a0, $t0
    li $v0, 4
    syscall
    move $a0, $t1
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $t0, -4($fp)
    move $a0, $t0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $t0, -0($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
    lw $t0, -4($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
    addiu $sp, $sp, -48
    sw $fp, 4($sp)
    sw $ra, 0($sp)
    sw $t0, 8($sp)
    sw $t1, 12($sp)
    sw $t2, 16($sp)
    sw $t3, 20($sp)
    sw $t4, 24($sp)
    sw $t5, 28($sp)
    sw $t6, 32($sp)
    sw $t7, 36($sp)
    sw $t8, 40($sp)
    sw $t9, 44($sp)
    addiu $fp, $sp, 52
    jal func_4
    lw $fp, 4($sp)
    lw $ra, 0($sp)
    lw $t0, 8($sp)
    lw $t1, 12($sp)
    lw $t2, 16($sp)
    lw $t3, 20($sp)
    lw $t4, 24($sp)
    lw $t5, 28($sp)
    lw $t6, 32($sp)
    lw $t7, 36($sp)
    lw $t8, 40($sp)
    lw $t9, 44($sp)
    addiu $sp, $sp, 56
    move $t0, $v0
    sw $t0, -8($fp)
    la $t0, str_13
    lw $t1, -0($fp)
    move $a0, $t0
    li $v0, 4
    syscall
    move $a0, $t1
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $t0, -4($fp)
    move $a0, $t0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    la $t0, str_14
    lw $t1, -8($fp)
    move $a0, $t0
    li $v0, 4
    syscall
    move $a0, $t1
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $t0, -0($fp)
    li $t1, 100
    seq $t1, $t0, $t1
    beq $t1, $0, label_30
    la $t0, str_15
    move $a0, $t0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    addiu $sp, $sp, -52
    sw $fp, 4($sp)
    sw $ra, 0($sp)
    sw $t0, 8($sp)
    sw $t1, 12($sp)
    sw $t2, 16($sp)
    sw $t3, 20($sp)
    sw $t4, 24($sp)
    sw $t5, 28($sp)
    sw $t6, 32($sp)
    sw $t7, 36($sp)
    sw $t8, 40($sp)
    sw $t9, 44($sp)
    addiu $fp, $sp, 48
    jal func_1
    lw $fp, 4($sp)
    lw $ra, 0($sp)
    lw $t0, 8($sp)
    lw $t1, 12($sp)
    lw $t2, 16($sp)
    lw $t3, 20($sp)
    lw $t4, 24($sp)
    lw $t5, 28($sp)
    lw $t6, 32($sp)
    lw $t7, 36($sp)
    lw $t8, 40($sp)
    lw $t9, 44($sp)
    addiu $sp, $sp, 52
    la $t0, str_16
    addiu $sp, $sp, -48
    sw $fp, 4($sp)
    sw $ra, 0($sp)
    sw $t0, 8($sp)
    sw $t1, 12($sp)
    sw $t2, 16($sp)
    sw $t3, 20($sp)
    sw $t4, 24($sp)
    sw $t5, 28($sp)
    sw $t6, 32($sp)
    sw $t7, 36($sp)
    sw $t8, 40($sp)
    sw $t9, 44($sp)
    addiu $fp, $sp, 44
    jal func_2
    lw $fp, 4($sp)
    lw $ra, 0($sp)
    lw $t0, 8($sp)
    lw $t1, 12($sp)
    lw $t2, 16($sp)
    lw $t3, 20($sp)
    lw $t4, 24($sp)
    lw $t5, 28($sp)
    lw $t6, 32($sp)
    lw $t7, 36($sp)
    lw $t8, 40($sp)
    lw $t9, 44($sp)
    addiu $sp, $sp, 48
    move $t1, $v0
    move $a0, $t0
    li $v0, 4
    syscall
    move $a0, $t1
    li $v0, 11
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $t0, 97
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
    li $t0, 98
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
    li $t0, 9
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
    li $t0, 10
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
    li $t0, 11
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
    li $t0, 12
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
    li $t0, 13
    addiu $sp, $sp, -4
    sw $t0, 0($sp)
    addiu $sp, $sp, -100
    sw $fp, 4($sp)
    sw $ra, 0($sp)
    sw $t0, 8($sp)
    sw $t1, 12($sp)
    sw $t2, 16($sp)
    sw $t3, 20($sp)
    sw $t4, 24($sp)
    sw $t5, 28($sp)
    sw $t6, 32($sp)
    sw $t7, 36($sp)
    sw $t8, 40($sp)
    sw $t9, 44($sp)
    addiu $fp, $sp, 124
    jal func_3
    lw $fp, 4($sp)
    lw $ra, 0($sp)
    lw $t0, 8($sp)
    lw $t1, 12($sp)
    lw $t2, 16($sp)
    lw $t3, 20($sp)
    lw $t4, 24($sp)
    lw $t5, 28($sp)
    lw $t6, 32($sp)
    lw $t7, 36($sp)
    lw $t8, 40($sp)
    lw $t9, 44($sp)
    addiu $sp, $sp, 128
    j label_29
    label_30:
    li $t1, -1
    seq $t1, $t0, $t1
    beq $t1, $0, label_31
    la $t0, str_17
    move $a0, $t0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    j label_29
    label_31:
    label_29:
    jr $ra
    jr $ra
