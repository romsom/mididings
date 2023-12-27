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

@_unitrepr.accept(str)
def WriteGlobalRegister(identifier):
    """
    WriteGlobalregister(identifier)

    Store an event's data value into a named register for later use.
    """
    return _Unit(_mididings.WriteGlobalRegister(identifier))

@_unitrepr.accept(str)
def ReadGlobalRegister(identifier):
    """
    ReadGlobalregister(identifier)

    Replace the event's data value with the one that has been saved into the named register.
    """
    return _Unit(_mididings.ReadGlobalRegister(identifier))
