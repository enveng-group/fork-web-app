.global div_custom_uint64
div_custom_uint64:
    movl 4(%esp), %eax    # Load numerator low part into EAX
    movl 8(%esp), %edx    # Load numerator high part into EDX
    divl 12(%esp)         # Divide EDX:EAX by denominator low part
    ret
