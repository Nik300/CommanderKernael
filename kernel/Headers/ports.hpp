#ifndef __ASSEMBLY_PORTS_PORTS_HPP
#define __ASSEMBLY_PORTS_PORTS_HPP

//8bit ports
extern "C" unsigned char port_byte_in (unsigned short);
extern "C" void port_byte_out (unsigned short, unsigned char);
//16bit ports
extern "C" unsigned short port_word_in (unsigned short);
extern "C" void port_word_out (unsigned short, unsigned short);
//32bit ports
extern "C" unsigned int port_int_in(unsigned short);
extern "C" void port_int_out(unsigned short, unsigned int);

namespace assembly{
	namespace ports{
		class port8bit{
		private:
			unsigned short port;
		public:
			port8bit(unsigned short port);
			~port8bit();
			virtual void write(unsigned char data);
			virtual unsigned char read();
		};
		class port16bit{
		private:
			unsigned short port;
		public:
			port16bit(unsigned short port);
			~port16bit();
			virtual void write(unsigned short data);
			virtual unsigned short read();
		};
		class port32bit{
        private:
            unsigned short port;
        public:
            port32bit(unsigned short port);
			~port32bit();
			virtual void write(unsigned int data);
			virtual unsigned int read();
		};
	}
}

#endif //__ASSEMBLY_PORTS_PORTS_HPP
