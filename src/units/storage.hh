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

#include <memory>

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
	WriteGlobalRegister(std::string identifier, int index) : identifier(identifier), index(index)
	{ }

	virtual bool process(MidiEvent & e) const
	{
		// TODO: Optimization: enumerate registers and preallocate memory
		// TODO: check return value
		if (e.type & MIDI_EVENT_SYSEX) {
			if (index < 0 || (size_t)index >= e.sysex->size())
				return false;
			int value = (*e.sysex)[index];
			util::storage::get_storage().store_global_register(identifier, value);
		} else {
			if (index < 0 || index >= 2)
				return false;
			util::storage::get_storage().store_global_register(identifier, e.data[index]);
		}
		return true;
	}

private:
	std::string identifier;
	int index;
};

// Unit which reads back a previously stored value indexed by name
class ReadGlobalRegister
  : public Unit
{
public:
	ReadGlobalRegister(std::string identifier, int index) : identifier(identifier), index(index)
	{ }

	virtual bool process(MidiEvent & e) const
	{
		// TODO: Optimization: enumerate registers and preallocate memory
		// TODO: check return value
		if (e.type & MIDI_EVENT_SYSEX) {
			if (index < 0 || (size_t)index >= e.sysex->size())
				return false;
			int value;
			util::storage::get_storage().read_global_register(identifier, value);
			SysExDataPtr new_sysex(new SysExData(*e.sysex));
			(*new_sysex)[index] = value;
			e.sysex = new_sysex;
		} else {
			if (index < 0 || index >= 2)
				return false;
			util::storage::get_storage().read_global_register(identifier, e.data[index]);
		}
		return true;
	}

private:
	std::string identifier;
	int index;
};

class Gate
  : public Unit
{
public:
	Gate(bool initial_state) : active(new bool) { *active = initial_state; };
	Gate(Gate &linked) : active(linked.get_ref()) {};
	std::shared_ptr<bool> get_ref() { return active; }

	virtual bool process(MidiEvent & e) const {
		return true;
	}
protected:
	std::shared_ptr<bool> active;
};


template<bool set_active> class GateTrigger
  : public Gate
{
public:
	GateTrigger(bool initial_state) : Gate(initial_state) {}
	GateTrigger(Gate &linked) : Gate(linked) {}
	virtual bool process(MidiEvent & e) const
	{
		*active = set_active;
		return true;
	}
};

using GateTriggerActivate=GateTrigger<true>;
using GateTriggerDeactivate=GateTrigger<false>;

class GateFilter
	: public Gate
{
public:
	GateFilter(bool initial_state) : Gate(initial_state) {}
	GateFilter(Gate &linked) : Gate(linked) {}
	virtual bool process(MidiEvent & e) const
	{
		return *active;
	}
};
} // units
} // mididings

#endif // MIDIDINGS_UNITS_STORAGE_HH
