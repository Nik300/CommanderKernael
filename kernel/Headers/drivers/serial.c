/* vim: tabstop=4 shiftwidth=4 noexpandtab
 *
 * Serial Port Driver
 */
#include <Headers/utility/ports.h>
#include <Headers/utility/types.h>
#include <Headers/stdio/stdio.h>

 /* Serial */
#define SERIAL_PORT_A 0x3F8
#define SERIAL_PORT_B 0x2F8
#define SERIAL_PORT_C 0x3E8
#define SERIAL_PORT_D 0x2E8

void serial_enable(int device) {
	port_byte_out(device + 1, 0x00);
	port_byte_out(device + 3, 0x80); /* Enable divisor mode */
	port_byte_out(device + 0, 0x03); /* Div Low:  03 Set the port to 38400 bps */
	port_byte_out(device + 1, 0x00); /* Div High: 00 */
	port_byte_out(device + 3, 0x03);
	port_byte_out(device + 2, 0xC7);
	port_byte_out(device + 4, 0x0B);
}

void serial_install() {

	serial_enable(SERIAL_PORT_A);
	serial_enable(SERIAL_PORT_B);
}

int serial_rcvd(int device) {
	return port_byte_in(device + 5) & 1;
}

char serial_recv(int device) {
	while (serial_rcvd(device) == 0) ;
	return port_byte_in(device);
}

char serial_recv_async(int device) {
	return port_byte_in(device);
}

int serial_transmit_empty(int device) {
	return port_byte_in(device + 5) & 0x20;
}

void serial_send(int device, char out) {
	while (serial_transmit_empty(device) == 0);
	port_byte_out(device, out);
}

void serial_string(int device, char * out) {
	for (uint32_t i = 0; i < strlen(out); ++i) {
		serial_send(device, out[i]);
	}
}
