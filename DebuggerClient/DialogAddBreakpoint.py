import GUI


class DialogAddBreakpoint(GUI.AddBreakpointDialog):
    def __init__(self, parent):
        GUI.AddBreakpointDialog.__init__(self, parent)
        self._parent = parent

    def EventABCancel(self, event):
        self.Destroy()

    def EventABAddBreakpoint(self, event):
        type = self.comboType.GetValue()
        param1 = self.textCtrlAddress.GetValue()
        param2 = self.textCtrlValue.GetValue()
        if self._parent.ControllerAddBreakpoint(type, param1, param2):
            self.Destroy()