.global add_custom_uint64
add_custom_uint64:
    movl 4(%esp), %eax    # Load low part of first operand
    addl 12(%esp), %eax   # Add low part of second operand
    movl %eax, 16(%esp)   # Store result's low part
    movl 8(%esp), %eax    # Load high part of first operand
    adcl 16(%esp), %eax   # Add high part with carry
    movl %eax, 20(%esp)   # Store result's high part
    ret
