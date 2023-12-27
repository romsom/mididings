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

#ifndef MIDIDINGS_UNITS_STORAGE_HH
#define MIDIDINGS_UNITS_STORAGE_HH

#include "midi_event.hh"

#include "units/util.hh"

#include "util/storage.hh"
#include "util/debug.hh"


namespace mididings {
namespace units {

// Unit which stores the input value for later use indexed by name
class WriteGlobalRegister
  : public Unit
{
public:
	WriteGlobalRegister(std::string identifier) {
		this->identifier = identifier;
	}

	virtual bool process(MidiEvent & e) const
	{
		// TODO: Optimization: enumerate registers and preallocate memory
		// TODO: check return value
		// TODO: make data element selectable
		util::storage::get_storage().store_global_register(identifier, e.data1);
		return true;
	}

private:
	std::string identifier;
};

// Unit which reads back a previously stored value indexed by name
class ReadGlobalRegister
  : public Unit
{
public:
	ReadGlobalRegister(std::string identifier) {
		this->identifier = identifier;
	}

	virtual bool process(MidiEvent & e) const
	{
		// TODO: Optimization: enumerate registers and preallocate memory
		// TODO: check return value
		// TODO: make data element selectable
		util::storage::get_storage().read_global_register(identifier, e.data1);
		return true;
	}

private:
	std::string identifier;
};

} // units
} // mididings

#endif // MIDIDINGS_UNITS_STORAGE_HH
