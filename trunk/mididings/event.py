# -*- coding: utf-8 -*-
#
# mididings
#
# Copyright (C) 2008-2009  Dominic Sacré  <dominic.sacre@gmx.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

import _mididings
import main as _main
import util as _util
from misc import NamedFlag as _NamedFlag, NamedBitMask as _NamedBitMask


NONE        = _NamedBitMask(0, 'NONE')
NOTEON      = _NamedBitMask(1 << 0, 'NOTEON')
NOTEOFF     = _NamedBitMask(1 << 1, 'NOTEOFF')
NOTE        = _NamedBitMask(NOTEON | NOTEOFF, 'NOTE')
CTRL        = _NamedBitMask(1 << 2, 'CTRL')
PITCHBEND   = _NamedBitMask(1 << 3, 'PITCHBEND')
AFTERTOUCH  = _NamedBitMask(1 << 4, 'AFTERTOUCH')
PROGRAM     = _NamedBitMask(1 << 5, 'PROGRAM')
DUMMY       = _NamedBitMask(1 << 6, 'DUMMY')
ANY         = _NamedBitMask(~0, 'ANY')


EVENT_PORT      = _NamedFlag(-1, 'EVENT_PORT')
EVENT_CHANNEL   = _NamedFlag(-2, 'EVENT_CHANNEL')
# generic
EVENT_DATA1     = _NamedFlag(-3, 'EVENT_DATA1')
EVENT_DATA2     = _NamedFlag(-4, 'EVENT_DATA2')
# note
EVENT_NOTE      = _NamedFlag(-3, 'EVENT_NOTE')
EVENT_VELOCITY  = _NamedFlag(-4, 'EVENT_VELOCITY')
# controller
EVENT_PARAM     = _NamedFlag(-3, 'EVENT_PARAM')
EVENT_VALUE     = _NamedFlag(-4, 'EVENT_VALUE')
# program change
EVENT_PROGRAM   = _NamedFlag(-4, 'EVENT_PROGRAM')


def _make_get_set(type_, data, offset=lambda: 0):
    def getter(self):
        if not self.type_ & type_ and not type_ == ANY:
            print "midi event attribute error"
        return getattr(self, data) + offset()

    def setter(self, value):
        if not self.type_ & type_ and not type_ == ANY:
            print "midi event attribute error"
        setattr(self, data, value - offset())

    return (getter, setter)


class MidiEvent(_mididings.MidiEvent):
    def __init__(self, type_=0, port_=0, channel_=0, data1=0, data2=0):
        _mididings.MidiEvent.__init__(self)
        self.type_ = type_
        self.port_ = port_
        self.channel_ = channel_
        self.data1 = data1
        self.data2 = data2

    def __str__(self):
        return self.to_string()

    def to_string(self, portnames=[], portname_length=0):
        if len(portnames) > self.port_:
            port = portnames[self.port_]
        else:
            port = str(self.port)

        channel = self.channel

        if self.type_ == NOTEON:
            s = 'Note on:  %3d %3d  (%s)' % (self.note, self.velocity, _util.note_name(self.note))
        elif self.type_ == NOTEOFF:
            s = 'Note off: %3d %3d  (%s)' % (self.note, self.velocity, _util.note_name(self.note))
        elif self.type_ == CTRL:
            s = 'Control:  %3d %3d' % (self.param, self.value)
            n = _util.controller_name(self.param)
            if n: s += '  (%s)' % n
        elif self.type_ == PITCHBEND:
            s = 'Pitch bend: %+5d' % self.value
        elif self.type_ == AFTERTOUCH:
            s = 'Aftertouch:   %3d' % self.value
        elif self.type_ == PROGRAM:
            s = 'Program:      %3d' % self.program
        elif self.type_ == DUMMY:
            s = 'Dummy'
        else:
            s = 'None'

        return '[%*s, %2d] %s' % (max(portname_length, 2), port, channel, s)

    type      = property(*_make_get_set(ANY, 'type_'))
    port      = property(*_make_get_set(ANY, 'port_', lambda: _main._config['data_offset']))
    channel   = property(*_make_get_set(ANY, 'channel_', lambda: _main._config['data_offset']))

    note      = property(*_make_get_set(NOTE, 'data1'))
    velocity  = property(*_make_get_set(NOTE, 'data2'))
    param     = property(*_make_get_set(CTRL, 'data1'))
    value     = property(*_make_get_set(CTRL | PITCHBEND | AFTERTOUCH, 'data2'))
    program   = property(*_make_get_set(PROGRAM, 'data2', lambda: _main._config['data_offset']))


class NoteonEvent(MidiEvent):
    def __init__(self, port, channel, note, velocity):
        MidiEvent.__init__(
            self, NOTEON,
            _util.port_number(port),
            _util.channel_number(channel),
            _util.note_number(note),
            _util.velocity_value(velocity)
        )

class NoteoffEvent(MidiEvent):
    def __init__(self, port, channel, note, velocity=0):
        MidiEvent.__init__(
            self, NOTEOFF,
            _util.port_number(port),
            _util.channel_number(channel),
            _util.note_number(note),
            _util.velocity_value(velocity)
        )

class ControlEvent(MidiEvent):
    def __init__(self, port, channel, param, value):
        MidiEvent.__init__(
            self, CTRL,
            _util.port_number(port),
            _util.channel_number(channel),
            _util.ctrl_number(param),
            _util.ctrl_value(value)
        )

class ProgramEvent(MidiEvent):
    def __init__(self, port, channel, program):
        MidiEvent.__init__(
            self, PROGRAM,
            _util.port_number(port),
            _util.channel_number(channel),
            0,
            _util.program_number(program)
        )


__all__ = [x for x in dir() if not x.startswith('_')]
