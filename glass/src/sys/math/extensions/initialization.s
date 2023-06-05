
global configure_math_extensions
configure_math_extensions:
    extern enable_fpu
    lea r15, [rel enable_fpu]
    call r15
    extern allow_sse
    lea r15, [rel allow_sse]
    call r15
    ret
    