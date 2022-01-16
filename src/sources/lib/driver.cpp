#include <lib/driver.hpp>
#include <lib/memory.h>

using namespace System::HAL;

InterruptDriver::InterruptDriver(uint8_t irq, handler_t handler)
: irq(irq), handler(handler)
{
	Init();
	register_int_handler(irq, handler);
}
InterruptDriver::~InterruptDriver()
{
	Deactivate();	
}
void InterruptDriver::Activate()
{
	register_int_handler(irq, handler);	
}
void InterruptDriver::Deactivate()
{
	register_int_handler(irq, nullptr);
}
void InterruptDriver::Reset()
{
	Deactivate();
	Init();
	Activate();
}
void InterruptDriver::Fire()
{
	handler(nullptr);
}
void InterruptDriver::Init()
{	
}

DriverManager::DriverManager()
: loaded_drivers(nullptr), drivers_count(0)
{
}

Driver* &DriverManager::Register(Driver *driver)
{
	realloc(loaded_drivers, drivers_count * sizeof(Driver*));
	loaded_drivers[drivers_count++] = driver;
	return driver;
}
bool DriverManager::Unregister(int indx)
{
	if (indx < 0 || indx >= drivers_count)
		return false;
	
	for (int i = indx; i < drivers_count - 1; i++)
		loaded_drivers[i] = loaded_drivers[i + 1];
	
	drivers_count--;
	return true;
}

Driver* &DriverManager::operator [](uint8_t index)
{
	return loaded_drivers[index];
}
const Driver* &DriverManager::operator [](uint8_t index) const
{
	return (const Driver*&)loaded_drivers[index];
}