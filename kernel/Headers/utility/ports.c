/**
 * Read a byte from the specified port
 */

// 8bits
unsigned char port_byte_in (unsigned short port) {
    unsigned char result;
    /* Inline assembler syntax
     * !! Notice how the source and destination registers are switched from NASM !!
     *
     * '"=a" (result)'; set '=' the C variable '(result)' to the value of register e'a'x
     * '"d" (port)': map the C variable '(port)' into e'd'x register
     *
     * Inputs and outputs are separated by colons
     */
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void port_byte_out (unsigned short port, unsigned char data) {
    /* Notice how here both registers are mapped to C variables and
     * nothing is returned, thus, no equals '=' in the asm syntax
     * However we see a comma since there are two variables in the input area
     * and none in the 'return' area
     */
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

// 16bits
unsigned short port_word_in (unsigned short port) {
    unsigned short result;
    __asm__("inw %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void port_word_out (unsigned short port, unsigned short data) {
    __asm__("outw %%ax, %%dx" : : "a" (data), "d" (port));
}

// 32bits
unsigned int port_int_in (unsigned short port){
    unsigned int result;
    __asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
    return result;
}
void port_int_out (unsigned short port, unsigned int data){
    __asm__ volatile("outl %0, %1" : : "a"(data), "Nd" (port));
}

