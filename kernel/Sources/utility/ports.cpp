#include "Headers/ports.hpp"
#include "Headers/types.hpp"

using namespace assembly::types;
using namespace assembly::ports;

//8bit
port8bit::port8bit(uint16_t port){
	this->port = port;
}
port8bit::~port8bit(){}
uint8_t port8bit::read(){
	return port_byte_in(this->port);
}
void port8bit::write(uint8_t data){
	port_byte_out(this->port, data);
}

//16bit
port16bit::port16bit(uint16_t port){
	this->port = port;
}
port16bit::~port16bit(){}
uint16_t port16bit::read(){
	return port_word_in(this->port);
}
void port16bit::write(uint16_t data){
	port_word_out(this->port, data);
}

//32bit
port32bit::port32bit(uint16_t port){
	this->port = port;
}
port32bit::~port32bit(){}
uint32_t port32bit::read(){
	return port_int_in(this->port);
}
void port32bit::write(uint32_t data){
	port_int_out(this->port, data);
}
