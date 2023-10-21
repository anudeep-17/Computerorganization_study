.data
prompt: .asciiz "Enter a string to post fix it: "
input_buffer:   .space 64 # 64 bytes for input buffer
output_buffer:  .space 64 # 64 bytes for output buffer
outputvalue:   .space 4  # 4 bytes for output value
leftparenthesis: .ascii "("
rightparenthesis: .ascii ")"
plus: .ascii "+"
minus: .ascii "-"
newline: .asciiz "\n"
outputprompt1: .asciiz "Expression to be evaluated: \n"
outputprompt2: .asciiz " = "

.text
.globl main

    main:
        # print prompt
        li $v0, 4
        la $a0, prompt
        syscall
        
        # read input
        li $v0, 8
        la $a0, input_buffer
        li $a1, 64
        syscall

        # print input
        li $v0, 4
        la $a0, input_buffer
        syscall

        la $a0, input_buffer # move input_buffer to $a0
        jal convertopostfix # jump to convertopostfix
        
        # print output
        li $v0, 4
        la $a0, output_buffer
        syscall

        # exit
        li $v0, 10
        syscall

    convertopostfix:
        move $t0, $a0 # move input_buffer to $t0
        sub $sp, $sp, 4 # allocate 4 bytes on stack
        sw $ra, 0($sp) # save return address
        la $t8, output_buffer # load output_buffer to $t8

        read_each_char:

            lb $t7, 0($t0) # load first char of input_buffer to $t1
            
            li $t2, 10 # load 10 to $t2
            beq $t7, $t2, endofstring # if $t1 == 10 => newline in ascii, exit loop

            move $a0, $t7 # load first char of output_buffer to $a0
            jal isdigit
            
            beq $v0, -1, checkforparenthesis # if $v0 == -1 => not a digit, check for parenthesis
            
           
            sb $t7, 0($t8) # store $v0 to output_buffer

            addi $t8, $t8, 1 # increment output_buffer pointer
            addi $t0, $t0, 1 # increment input_buffer pointer
            j read_each_char # jump to convertopostfix

            checkforparenthesis:
                move $a0, $t7
                jal isparenthesis

                beq $v0, -1, checkoperator # if $v0 == -1 => not a parenthesis, check for operator
                
                beq $v0, 0, isleftparenthesis # if $v0 == 0 => leftparenthesis, jump to isleftparenthesis
                beq $v0, 1, isrightparenthesis # if $v0 == 1 => rightparenthesis, jump to isrightparenthesis

                isleftparenthesis:
                    lb $v0, leftparenthesis # load leftparenthesis to $v0
                    sub $sp, $sp, 1 # increment stack pointer
                    sb $v0, 0($sp)

                    addi $t0, $t0, 1 # increment input_buffer pointer
                    j read_each_char # jump to read_each_char

                isrightparenthesis:
                
                    pop:
                        lb $v0, leftparenthesis # load leftparenthesis to $v0
                      
                        lb $t1, 0($sp) # load first char of stack to $t1
                        
                        beq $t1, $v0, endofpop # if $t1 == $v0 => leftparenthesis, jump to endofpop
                        
                        sb $t1, 0($t8) # store $t1 to output_buffer
                        
                        addi $sp, $sp, 1 # decrement stack pointer
                        addi $t8, $t8, 1 # increment output_buffer pointer
                        j pop # jump to pop
                    
                    endofpop:
                        addi $sp, $sp, 1 # decrement stack pointer
                        addi $t0, $t0, 1 # increment input_buffer pointer
                        j read_each_char # jump to read_each_char
            
            checkoperator:
                move $a0, $t7
                jal isoperator

                beq $v0, -1, endofstring # if $v0 == -1 => not a parenthesis, check for operator

                beq $v0, 0, isplus # if $v0 == 0 => plus, jump to isplus
                beq $v0, 1, isminus # if $v0 == 1 => minus, jump to isminus


                isplus:
                    lb $v0, plus # load plus to $v0
                    sub $sp, $sp, 1 # increment stack pointer
                    sb $v0, 0($sp)

                    addi $t0, $t0, 1 # increment input_buffer pointer
                    j read_each_char # jump to read_each_char

                isminus:
                    lb $v0, minus # load plus to $v0
                    sub $sp, $sp, 1 # increment stack pointer
                    sb $v0, 0($sp)
                   
                    
                    addi $t0, $t0, 1 # increment input_buffer pointer
                    j read_each_char # jump to read_each_char

        endofstring:
            #return 
            lw $ra, 0($sp) # load return address
            addi $sp, $sp, 4 # deallocate 4 bytes on stack
            jr $ra # jump to return address


    isdigit:
        move $t1, $a0 # load first char of output_buffer to $t1
        li $t2, 48 # load 48 => ascii(0) to $t1
        blt $t1, $t2, return_notdigit # if $t1 < 0 => not a digit, jump to isoperator
        li $t2, 57 # load 57 => ascii(9) to $t1
        bgt $t1, $t2, return_notdigit # if $t1 > 9 => not a digit, jump to isoperator

        # if $t1 is a digit, store it to output_buffer
        li $t2, 48 # load 48 => ascii(0) to $t1
        sub $t1, $t1, $t2 # convert ascii to digit
        move $v0, $t1 
        jr $ra # jump to return address

        return_notdigit:
            # if $t1 is not a digit, jump to isoperator
            li $v0, -1
            jr $ra # jump to return address
    
    isparenthesis:
        move $t1, $a0 # load first char of output_buffer to $t1
        lb $t2, leftparenthesis # load leftparenthesis to $t2
        bne $t1, $t2, checkforrightparenthesis # if $t1 != $t2 => not leftparenthesis, jump to checkforrightparenthesis
        
        li $v0, 0 # if $t1 == $t2 => leftparenthesis
        jr $ra # jump to return address

        checkforrightparenthesis:
            lb $t2, rightparenthesis # load rightparenthesis to $t2
            bne $t1, $t2, return_notparenthesis # if $t1 != $t2 => not rightparenthesis 

            li $v0, 1 # if $t1 == $t2 => rightparenthesis
            jr $ra # jump to return address

        return_notparenthesis:
            li $v0, -1
            jr $ra # jump to return address
    
    isoperator:
        move $t1, $a0 # load first char of output_buffer to $t1
        lb, $t2, plus # load plus to $t2
        bne $t1, $t2, checkforminus # if $t1 != $t2 => not plus, jump to checkforminus

        li $v0, 0 # if $t1 == $t2 => plus
        jr $ra # jump to return address
        
        checkforminus:
            lb $t2, minus # load minus to $t2
            bne $t1, $t2, return_notoperator # if $t1 != $t2 => not minus
            li $v0, 1   # if $t1 == $t2 => minus
            jr $ra # jump to return address

        return_notoperator:
            li $v0, -1
            jr $ra # jump to return address