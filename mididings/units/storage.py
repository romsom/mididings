# -*- coding: utf-8 -*-
#
# mididings
#
# Copyright (C) 2023 Roman Sommer <roman@resonant-bytes.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

import _mididings
from mididings.units.base import _Unit
import mididings.unitrepr as _unitrepr
import mididings.overload as _overload

@_unitrepr.accept(str, int)
def WriteGlobalRegister(identifier, index):
    """
    WriteGlobalregister(identifier, index)

    Store an event's data value into a named register for later use.
    The index indicates which data byte should be stored. It must be 0 or 1 for regular events. For sysex events it must be greater or equal to 0 and smaller than the number of sysex bytes.
    """
    return _Unit(_mididings.WriteGlobalRegister(identifier, index))

@_unitrepr.accept(str, int)
def ReadGlobalRegister(identifier, index):
    """
    ReadGlobalregister(identifier, index)

    Replace the event's data value with the one that has been saved into the named register.
    The index indicates which data byte should be read. It must be 0 or 1 for regular events. For sysex events it must be greater or equal to 0 and smaller than the number of sysex bytes.
    """
    return _Unit(_mididings.ReadGlobalRegister(identifier, index))



@_overload.mark(
    """
    GateTrigger(initial_state, activate)
    GateTrigger(linked)
    """
)
@_unitrepr.accept(bool, bool)
def GateTrigger(initial_state, activate):
    if activate:
        return _Unit(_mididings.GateTriggerActivate(initial_state))
    return _Unit(_mididings.GateTriggerDeactivate(initial_state))

@_overload.mark
@_unitrepr.accept(_mididings.Gate)
def GateTrigger(linked):
    if activate:
        return _Unit(_mididings.GateTriggerActivate(linked))
    return _Unit(_mididings.GateTriggerDeactivate(linked))

@_overload.mark(
    """
    GateFilter(initial_state, activate)
    GateFilter(linked)
    """
)
@_unitrepr.accept(bool, bool)
def GateFilter(initial_state, activate):
    return _Unit(_mididings.GateFilter(initial_state))

@_overload.mark
@_unitrepr.accept(_mididings.Gate)
def GateFilter(linked):
    return _Unit(_mididings.GateFilter(linked))
