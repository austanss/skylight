extern enable_fpu
extern allow_sse

global configure_math_extensions
configure_math_extensions:
    call enable_fpu
    call allow_sse
    ret