#pragma once
#include <stdint.h>

inline
__attribute__((always_inline)) 
void outb(uint16_t port, uint8_t val)
{
    asm volatile("outb %0, %1"
                :
                : "a"(val), "Nd"(port));
}

inline
__attribute__((always_inline)) 
uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0"
                : "=a"(ret)
                : "Nd"(port));
    return ret;
}
inline
__attribute__((always_inline)) 
void outw(uint16_t port, uint16_t val)
{
    asm volatile("outw %0, %1"
                :
                : "a"(val), "Nd"(port));
}

inline
__attribute__((always_inline)) 
uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile("inw %1, %0"
                : "=a"(ret)
                : "Nd"(port));
    return ret;
}

inline
__attribute__((always_inline)) 
void outl(uint16_t port, uint32_t val)
{
    asm volatile("outl %0, %1"
                :
                : "a"(val), "Nd"(port));
}

inline
__attribute__((always_inline)) 
uint32_t inl(uint16_t port)
{
    uint32_t ret;
    asm volatile("inl %1, %0"
                : "=a"(ret)
                : "Nd"(port));
    return ret;
}
