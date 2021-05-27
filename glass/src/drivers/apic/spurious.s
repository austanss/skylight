extern apic_local_send_eoi

global spurious_handler
spurious_handler:
    iretq