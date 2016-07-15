class Breakpoint():
    def __init__(self, breakpointNumber):
        self.breakpointNumber = breakpointNumber


class BreakpointPPUByTime(Breakpoint):
    def __init__(self, breakpointNumber, scanline, tick):
        Breakpoint.__init__(self, breakpointNumber)
        self._scanline = scanline
        self._tick = tick

    def toString(self):
        return 'Scanline = {self._scanline:s}, Tick = {self._tick:s}'.format(**locals())


class BreakpointPPUByAddress(Breakpoint):
    def __init__(self, breakpointNumber, address):
        Breakpoint.__init__(self, breakpointNumber)
        self._address = address

    def toString(self):
        return 'Address = {self._address:s}'.format(**locals())


class BreakpointPPUByValue(Breakpoint):
    def __init__(self, breakpointNumber, address, value):
        Breakpoint.__init__(self, breakpointNumber)
        self._address = address
        self._value = value

    def toString(self):
        return 'Address = {self._address:s}, Value = {self._value:s}'.format(**locals())