/*
 * mididings
 *
 * Copyright (C) 2023 Roman Sommer <roman@resonant-bytes.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef MIDIDINGS_UTIL_STORAGE_HH
#define MIDIDINGS_UTIL_STORAGE_HH

#include <unordered_map>
#include <string>

namespace mididings {
namespace util {
namespace storage {
class RegisterStorage {
public:
	RegisterStorage() : global_registers()
	{}

	bool store_global_register(std::string identifier, int value);
	bool read_global_register(std::string identifier, int &value);

	private:
	std::unordered_map<std::string, int> global_registers;
};
RegisterStorage &get_storage();
} // storage
} // util
} // mididings

#endif // MIDIDINGS_UTIL_STORAGE_HH
