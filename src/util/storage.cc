#include "storage.hh"

namespace mididings {
namespace util {
namespace storage {
bool RegisterStorage::store_global_register(std::string identifier, int value)
{
	global_registers[identifier] = value;
	return true;
}

bool RegisterStorage::read_global_register(std::string identifier, int &value)
{
	if (global_registers.find(identifier) != global_registers.end())
	{
		value = global_registers[identifier];
		return true;
	}
	return false;
}

RegisterStorage register_storage;
RegisterStorage &get_storage()
{
	return register_storage;
}
} // storage
} // util
} // mididings
