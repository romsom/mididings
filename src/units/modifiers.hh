/*
 * mididings
 *
 * Copyright (C) 2008-2014  Dominic Sacré  <dominic.sacre@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef MIDIDINGS_UNITS_MODIFIERS_HH
#define MIDIDINGS_UNITS_MODIFIERS_HH

#include "units/base.hh"
#include "units/util.hh"

#include <vector>


namespace mididings {
namespace units {


class Port
  : public Unit
{
  public:
    Port(int port)
      : _port(port)
    { }

    virtual bool process(MidiEvent & ev) const
    {
        ev.port = _port;
        return true;
    }

  private:
    int const _port;
};


class Channel
  : public Unit
{
  public:
    Channel(int channel)
      : _channel(channel)
    { }

    virtual bool process(MidiEvent & ev) const
    {
        if (!(ev.type & (MIDI_EVENT_SYSTEM | MIDI_EVENT_DUMMY))) {
            ev.channel = _channel;
        }
        return true;
    }

  private:
    int const _channel;
};


class Transpose
  : public Unit
{
  public:
    Transpose(int offset)
      : _offset(offset)
    { }

    virtual bool process(MidiEvent & ev) const
    {
        if (ev.type & (MIDI_EVENT_NOTE | MIDI_EVENT_POLY_AFTERTOUCH)) {
            ev.note.note += _offset;
        }
        return true;
    }

  private:
    int const _offset;
};


class Key
  : public Unit
{
  public:
    Key(int note)
      : _note(note)
    { }

    virtual bool process(MidiEvent & ev) const
    {
        if (ev.type & (MIDI_EVENT_NOTE | MIDI_EVENT_POLY_AFTERTOUCH)) {
            ev.note.note = _note;
        }
        return true;
    }

  private:
    int const _note;
};


class Velocity
  : public Unit
{
  public:
    Velocity(float param, TransformMode mode)
      : _param(param)
      , _mode(mode)
    { }

    virtual bool process(MidiEvent & ev) const
    {
        if (ev.type == MIDI_EVENT_NOTEON && ev.note.velocity > 0) {
            ev.note.velocity = apply_transform(ev.note.velocity,
                                               _param, _mode);
        }
        return true;
    }

  private:
    float const _param;
    TransformMode const _mode;
};


class VelocitySlope
  : public Unit
{
  public:
    VelocitySlope(std::vector<int> notes,
                  std::vector<float> params, TransformMode mode)
      : _notes(notes)
      , _params(params)
      , _mode(mode)
    {
        ASSERT(notes.size() == params.size());
        ASSERT(notes.size() > 1);
        for (unsigned int n = 0; n < notes.size() - 1; ++n) {
            ASSERT(notes[n] <= notes[n + 1]);
        }
    }

    virtual bool process(MidiEvent & ev) const
    {
        if (ev.type == MIDI_EVENT_NOTEON && ev.note.velocity > 0) {
            unsigned int n = 0;
            while (n < _notes.size() - 2 && _notes[n + 1] < ev.note.note) ++n;

            ev.note.velocity = apply_transform(
                ev.note.velocity,
                map_range(ev.note.note, _notes[n], _notes[n + 1],
                                        _params[n], _params[n + 1]),
                _mode
            );
        }
        return true;
    }

  private:
    std::vector<int> const _notes;
    std::vector<float> const _params;
    TransformMode const _mode;
};


class CtrlMap
  : public Unit
{
  public:
    CtrlMap(int ctrl_in, int ctrl_out)
      : _ctrl_in(ctrl_in)
      , _ctrl_out(ctrl_out)
    { }

    virtual bool process(MidiEvent & ev) const
    {
        if (ev.type == MIDI_EVENT_CTRL && ev.ctrl.param == _ctrl_in) {
            ev.ctrl.param = _ctrl_out;
        }
        return true;
    }

  private:
    int const _ctrl_in;
    int const _ctrl_out;
};


class CtrlRange
  : public Unit
{
  public:
    CtrlRange(int ctrl, int min, int max, int in_min, int in_max)
      : _ctrl(ctrl)
      , _min(min)
      , _max(max)
      , _in_min(in_min)
      , _in_max(in_max)
    {
        ASSERT(in_min < in_max);
    }

    virtual bool process(MidiEvent & ev) const
    {
        if (ev.type == MIDI_EVENT_CTRL && ev.ctrl.param == _ctrl) {
            ev.ctrl.value = map_range(ev.ctrl.value,
                                      _in_min, _in_max, _min, _max);
        }
        return true;
    }

  private:
    int const _ctrl;
    int const _min;
    int const _max;
    int const _in_min;
    int const _in_max;
};


class CtrlCurve
  : public Unit
{
  public:
    CtrlCurve(int ctrl, float param, TransformMode mode)
      : _ctrl(ctrl)
      , _param(param)
      , _mode(mode)
    { }

    virtual bool process(MidiEvent & ev) const
    {
        if (ev.type == MIDI_EVENT_CTRL && ev.ctrl.param == _ctrl) {
            ev.ctrl.value = apply_transform(ev.ctrl.value, _param, _mode);
        }
        return true;
    }

  private:
    int const _ctrl;
    float const _param;
    TransformMode const _mode;
};


class PitchbendRange
  : public Unit
{
  public:
    PitchbendRange(int min, int max, int in_min, int in_max)
      : _min(min)
      , _max(max)
      , _in_min(in_min)
      , _in_max(in_max)
    { }

    virtual bool process(MidiEvent & ev) const
    {
        if (ev.type == MIDI_EVENT_PITCHBEND) {
            if (ev.ctrl.value >= 0) {
                ev.ctrl.value = map_range(ev.ctrl.value, 0, _in_max, 0, _max);
            } else {
                ev.ctrl.value = map_range(ev.ctrl.value, _in_min, 0, _min, 0);
            }
        }
        return true;
    }

  private:
    int const _min;
    int const _max;
    int const _in_min;
    int const _in_max;
};

class ChannelDistributor
	: public Unit
{
	ChannelDistributor(int from_channel, int to_channel)
		: _from_channel(from_channel)
		, _to_channel(to_channel)
		// , _n_channels(to_channel - from_channel)
		// , _active_notes(_n_channels)
		// , _active_notes_oldest(0)
		// , _active_notes_next(0)
	{
		/*
		_active_notes.reserve(n_channels);
		// raise exception (in python?) if n_channel == 0
		for (int i=0; i<_n_channels; i++) {
			// mark all channels unused
			_active_notes[i] = {0, -1};
		}
		*/
	}

	virtual bool process(MidiEvent & ev) const
	// TODO will non-const work?
	{ /*
		if (ev.type == MIDI_EVENT_NOTEON) {
			int channel = get_next_free_channel();
			if (channel < 0) {
				// steal voice
				auto preempted_note = _active_notes[active_notes_oldest];
				channel = std::get<1>(preempted_note);
				// since we reuse the channel we don't need to update it
				_active_notes[active_notes_oldest] = {ev.note.note, channel};
				// TODO insert note off
				// update pointers
				_active_notes_oldest = (_active_notes_oldest + 1) % _n_channels;
				_active_notes_next = (_active_notes_next + 1) % _n_channels;
			}
			ev.channel = channel;
		} else if (ev.type == MIDI_EVENT_NOTEOFF) {
			int i = note_active(ev.note.note);
			if (i >= 0) { // note is active
				// send note off to correct channel
				ev.channel = std::get<1>(_active_notes[i]);
				// move all later notes one back
				for (int offset=1; offset<_n_channels; offset++) {
					auto note = _active_notes[(i + offset) % n_channels];
					// copy note into last slot
					_active_notes[(i - 1 + offset) % _n_channels] = note;
					// return if we have reached inactive slots
					if (std::get<1>(note) == -1)
						return true;
				}
				// set last note slot inactive explicitly
				_active_notes[_n_channels - 1] = {0, -1};
			} else {
				// drop the note or alternatively send it to all channels
			}
		} else {
			// TODO generate midi events for all channels
		}
	  */
		return true;
	}
private:
	// int channel_active(int ch) {
	// 	for (int i=0; i<_n_channels; i++) {
	// 		if (note == std::get<1>(active_notes[i]))
	// 			return i;
	// 	}
	// 	return -1;
	// }

	// int note_active(int note) {
	// 	for (int i=0; i<_n_channels; i++) {
	// 		if (note == std::get<0>(active_notes[i]))
	// 			return i;
	// 	}
	// 	return -1;
	// }

	// int get_next_free_channel() const {
	// 	for (i=from_channel; i<to_channel; i++) {
	// 		if (channel_active(i) < 0)
	// 			return i;
	// 	}
	// 	return -1;
	// }

	int _from_channel;
	int _to_channel;
	// int _n_channels;
	// std::vector<tuple<int, int>> _active_notes; // <note, channel>
	// int active_note_oldest;
	// int active_note_next;
}
} // units
} // mididings


#endif // MIDIDINGS_UNITS_MODIFIERS_HH
