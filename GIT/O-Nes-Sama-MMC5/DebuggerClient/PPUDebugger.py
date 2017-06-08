import wx

import Breakpoint
import Debugger
import DialogAddBreakpoint
import GUI


class PPUDebuggerFrame(GUI.MainFrame, Debugger.Debugger):
    #constructor
    def __init__(self, parent):
        GUI.MainFrame.__init__(self, parent)
        Debugger.Debugger.__init__(self)
        self.addBreakpointFrame = None
        self.breakpoints = []
        self.paused = False
        self.EventConnect(None)

    def __del__(self):
        self.EventDisconnect(None)

    def EventConnect(self, event):
        if not self._connected:
            self.ConnectToDebugger()
            self.staticTextConnection.SetLabel("Connected")

    def EventDisconnect(self, event):
        if self._connected:
            self.Quit()
            self.staticTextConnection.SetLabel("Disconnected")

    def EventAddBreakpoint(self, event):
        if not self.addBreakpointFrame:
            self.addBreakpointFrame = DialogAddBreakpoint.DialogAddBreakpoint(self)
        self.addBreakpointFrame.Show(True)

    def EventContinue(self, event):
        self.Continue()
        self.staticRunning.SetLabel('Running')
        self.timerContinue.Start()

    def EventRun(self, event):
        self.paused = False
        self.Continue()
        self.staticRunning.SetLabel('Running')
        self.timerRun.Start()

    def EventPause(self, event):
        self.paused = True

    def ControllerAddBreakpoint(self, type, param1, param2):
        typeList = ['address', 'value', 'time']
        typeLower = type.lower()
        if typeLower in typeList:
            if typeLower == 'time':
                bpNumber = self.AddBreakpointPPUByTime(param1, param2)
                if bpNumber is not False:
                    breakpoint = Breakpoint.BreakpointPPUByTime(bpNumber, param1, param2)
                    self.breakpoints.append(breakpoint)
                    self.listBoxBreakpoints.Append(breakpoint.toString())
            elif typeLower == 'address':
                bpNumber = self.AddBreakpointPPUByAddress(param1)
                if bpNumber is not False:
                    breakpoint = Breakpoint.BreakpointPPUByAddress(bpNumber, param1)
                    self.breakpoints.append(breakpoint)
                    self.listBoxBreakpoints.Append(breakpoint.toString())
            elif typeLower == 'value':
                bpNumber = self.AddBreakpointPPUByValue(param1, param2)
                if bpNumber is not False:
                    breakpoint = Breakpoint.BreakpointPPUByValue(bpNumber, param1, param2)
                    self.breakpoints.append(breakpoint)
                    self.listBoxBreakpoints.Append(breakpoint.toString())
            return True
        else:
            return False

    def EventDeleteBreakpoint(self, event):
        sel = self.listBoxBreakpoints.GetSelection()
        if(sel >= 0):
            dlg = wx.MessageDialog(parent=None, message="Are you sure you want to delete it?", caption="Question",
                                   style=wx.YES_NO | wx.ICON_QUESTION)
            result = dlg.ShowModal()
            if result == wx.ID_YES:
                index = self.listBoxBreakpoints.GetSelection()
                string = self.listBoxBreakpoints.GetStringSelection()
                bpNumber = -1
                for elem in self.breakpoints:
                    if elem.toString() == string:
                        bpNumber = elem.breakpointNumber
                        self.listBoxBreakpoints.Delete(index)
                        self.breakpoints.remove(elem)
                self.DeleteBreakpoint(bpNumber)
                pass
            else:
                pass
            dlg.Destroy()
        #self.listBoxBreakpoints.Delete()

    def _StoppedAt(self, number):
        string = 'None'
        for elem in self.breakpoints:
            if elem.breakpointNumber == number:
                string = elem.toString()
        self.staticRunning.SetLabel('Stopped at: {string:s}'.format(**locals()))

    def EventTimerContinue(self, event):
        number = self.Reached()
        if number is not False:
            if number == "ERROR":
                self.staticRunning.SetLabel('ERROR')
            else:
                self._StoppedAt(number)
                self.timerContinue.Stop()

    def EventTimerRun(self, event):
        number = self.Reached()
        if number is not False:
            if number == "ERROR":
                self.staticRunning.SetLabel('ERROR')
            else:
                if not self.paused:
                    self.Continue()
                else:
                    self._StoppedAt(number)
                    self.timerRun.Stop()


    def EventUpdateCHR(self, event):
        self.GenerateCHR0()
        self.GenerateCHR1()
        self.panelCHR0.Refresh()
        self.panelCHR1.Refresh()

    def EventUpdateNametables(self, event):
        self.GenerateNametable1()
        self.panelNametable1.Refresh()
        self.GenerateNametable2()
        self.panelNametable2.Refresh()
        self.GenerateNametable3()
        self.panelNametable3.Refresh()
        self.GenerateNametable4()
        self.panelNametable4.Refresh()

    def EventUpdateOam(self, event):
        self.GenerateOams()
        self.panelOam.Refresh()

    def EventCHRMotion(self, event):
        x = event.GetPosition()[0] >> 3 >> 1
        y = event.GetPosition()[1] >> 3 >> 1
        self.labelCHR.SetLabel("Position: ${y:X}{x:X}".format(**locals()))

    def EventNTMotion(self, event):
        x = event.GetPosition()[0] >> 3
        y = event.GetPosition()[1] >> 3
        pos = 0x2000 | (y << 5) | x
        self.labelNT.SetLabel("Address: ${pos:X}, Tile: $00, X: {x}, Y: {y}".format(**locals()))

    def EventOamMotion(self, event):
        pass

    def _PaintFromMemory(self, width, height, data, panel, scaling):
        img = wx.EmptyImage(width, height)
        img.SetData(data)
        if scaling:
            bmp = img.Scale(width*2, height*2).ConvertToBitmap()
        else:
            bmp = img.ConvertToBitmap()
        dc = wx.PaintDC(panel)
        dc.DrawBitmap(bmp, 0, 0)

    def _PaintFromImage(self, width, height, image, panel, scaling):
        if scaling:
            bmp = image.Scale(width*scaling, height*scaling).ConvertToBitmap()
        else:
            bmp = image.ConvertToBitmap()
        dc = wx.PaintDC(panel)
        dc.DrawBitmap(bmp, 0, 0)

    def EventPaintCHR0(self, event):
        self._PaintFromImage(self.CHR_WIDTH, self.CHR_HEIGHT, self.imageCHR0, self.panelCHR0, 2)

    def EventPaintCHR1(self, event):
        self._PaintFromImage(self.CHR_WIDTH, self.CHR_HEIGHT, self.imageCHR1, self.panelCHR1, 2)

    def EventPaintNametable1(self, event):
        self._PaintFromImage(self.NAMETABLE_WIDTH, self.NAMETABLE_HEIGHT, self.imageNametable1, self.panelNametable1, 0)

    def EventPaintNametable2(self, event):
        self._PaintFromImage(self.NAMETABLE_WIDTH, self.NAMETABLE_HEIGHT, self.imageNametable2, self.panelNametable2, 0)

    def EventPaintNametable3(self, event):
        self._PaintFromImage(self.NAMETABLE_WIDTH, self.NAMETABLE_HEIGHT, self.imageNametable3, self.panelNametable3, 0)

    def EventPaintNametable4(self, event):
        self._PaintFromImage(self.NAMETABLE_WIDTH, self.NAMETABLE_HEIGHT, self.imageNametable4, self.panelNametable4, 0)

    def EventPaintOam(self, event):
        self._PaintFromImage(256, 240, self.imageOam, self.panelOam, 2)

    def EventEraseBackground(self, event):
        pass

    def EventNT1LeftDown(self, event):
        print event.GetPosition()[0] >> 3
        print event.GetPosition()[1] >> 3




#mandatory in wx, create an app, False stands for not deteriction stdin/stdout
#refer manual for details
app = wx.App(False)
 
#create an object of CalcFrame
frame = PPUDebuggerFrame(None)
#show the frame
frame.Show(True)
#start the applications
app.MainLoop()
