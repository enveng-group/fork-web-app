.global compatStrtol64_asm
compatStrtol64_asm:
    movl $0, %eax          // Initialize result to 0
    movl $0, %ebx          // Initialize sign to positive
    movl 4(%esp), %ecx     // Load str (first argument)
    movl 8(%esp), %edx     // Load base (second argument)
    movl 12(%esp), %esi    // Load endptr (third argument)

    // Skip leading whitespace
skip_whitespace:
    movb (%ecx), %bl
    cmpb $32, %bl
    je skip_whitespace
    cmpb $0, %bl
    je end_conversion

    // Check for sign
    cmpb $45, %bl          // Check if character is '-'
    jne check_plus
    movl $1, %ebx          // Set sign to negative
    incl %ecx
    jmp parse_digits

check_plus:
    cmpb $43, %bl          // Check if character is '+'
    jne parse_digits
    incl %ecx

parse_digits:
    movb (%ecx), %bl
    cmpb $0, %bl
    je end_conversion

    // Convert character to digit
    subb $48, %bl          // Convert '0'-'9' to 0-9
    cmpb $9, %bl
    jbe valid_digit
    subb $7, %bl           // Convert 'A'-'F' to 10-15
    cmpb $15, %bl
    jbe valid_digit
    subb $32, %bl          // Convert 'a'-'f' to 10-15
    cmpb $15, %bl
    jbe valid_digit
    jmp end_conversion

valid_digit:
    imull %edx, %eax       // Multiply result by base
    addl %ebx, %eax        // Add digit to result
    incl %ecx
    jmp parse_digits

end_conversion:
    testl %ebx, %ebx       // Check if sign is negative
    jz positive_result
    negl %eax              // Negate result if negative

positive_result:
    movl %ecx, (%esi)      // Set endptr to point to the end of the parsed string
    ret
