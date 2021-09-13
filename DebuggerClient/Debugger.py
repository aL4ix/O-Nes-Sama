import SocketClient
import wx

class Debugger(SocketClient.SocketClient):
    def __init__(self):
        SocketClient.SocketClient.__init__(self)
        self.CHR_WIDTH = 128
        self.CHR_HEIGHT = 128
        self.NAMETABLE_WIDTH = 256
        self.NAMETABLE_HEIGHT = 240
        self.imageCHR0 = wx.EmptyImage(self.CHR_WIDTH, self.CHR_HEIGHT)
        self.imageCHR1 = wx.EmptyImage(self.CHR_WIDTH, self.CHR_HEIGHT)
        self.imageNametable1 = wx.EmptyImage(self.NAMETABLE_WIDTH, self.NAMETABLE_HEIGHT)
        self.imageNametable2 = wx.EmptyImage(self.NAMETABLE_WIDTH, self.NAMETABLE_HEIGHT)
        self.imageNametable3 = wx.EmptyImage(self.NAMETABLE_WIDTH, self.NAMETABLE_HEIGHT)
        self.imageNametable4 = wx.EmptyImage(self.NAMETABLE_WIDTH, self.NAMETABLE_HEIGHT)
        self.oam = []
        self.imageOam = wx.EmptyImage(self.NAMETABLE_WIDTH, self.NAMETABLE_HEIGHT)

    def ConnectToDebugger(self):
        self.ConnectToServer("localhost", 0xBA70)

    def Quit(self):
        self.Send("QUIT")
        self.Receive()
        self.CloseSocket()

    def AddBreakpoint(self, instruction):
        if not self.Send(instruction):
            return False
        reply = self.Receive()
        if reply.startswith("REPLY"):
            bpNumber = int(reply.split(" ")[1], 16)
            return bpNumber

    def AddBreakpointPPUByTime(self, scanline, tick):
        return self.AddBreakpoint('SET breakpoint new ppu time {scanline:s} {tick:s}'.format(**locals()))

    def AddBreakpointPPUByAddress(self, address):
        return self.AddBreakpoint('SET breakpoint new ppu address {address:s}'.format(**locals()))

    def AddBreakpointPPUByValue(self, address, value):
        return self.AddBreakpoint('SET breakpoint new ppu value {address:s} {value:s}'.format(**locals()))

    def DeleteBreakpoint(self, number):
        if self.Send('SET breakpoint {number:s} delete'.format(**locals())):
            return self.Receive() == "OK"

    def Continue(self):
        return self.Send('CONTINUE')

    def Reached(self):
        if self._connected:
            try:
                self.SetBlocking(0)
                reply = self.Receive()
                self.SetBlocking(1)
                if reply.startswith('REACHED'):
                    return int(reply.split(' ')[1], 16)
                return "ERROR"
            except:
                    pass
        return False

    def GetData(self, inst):
        self.Send(inst)
        reply = self.Receive()
        if reply.startswith("REPLY"):
            return int(reply.split(' ')[1], 16)
        return None

    def GetPPUMem(self, address):
        return self.GetData('GET ppumem {address:X}'.format(**locals()))

    def GetPPUOam(self, address):
        return self.GetData('GET ppuoam {address:X}'.format(**locals()))

    def GetBitmapChr(self, address):
        self.Send('GET bitmap chr {address:X}'.format(**locals()))
        reply = self.Receive()
        if reply.startswith('REPLY'):
            return reply.split(' ')[1]

    def GetRenderedNt(self, address):
        self.Send('GET rendered nt {address:X}'.format(**locals()))
        reply = self.Receive()
        if reply.startswith('REPLY'):
            data = reply.split(' ')[1]
            ret = []
            for i in range(8*8*3):
                value = int('{:s}{:s}'.format(data[i*2+0], data[i*2+1]), 16)
                ret.append(value)
            return ret

    def GetRenderedOam(self, address):
        self.Send('GET rendered oam {address:X}'.format(**locals()))
        reply = self.Receive()
        if reply.startswith('REPLY'):
            data = reply.split(' ')[1]
            ret = []
            for i in range(8*8*3):
                value = int('{:s}{:s}'.format(data[i*2+0], data[i*2+1]), 16)
                ret.append(value)
            return ret

    def GenerateCHR0(self):
        palette = [(0, 0, 0), (0x30, 0x30, 0xFF), (0x7F, 0x7F, 0xFF), (0xFF, 0xFF, 0xFF)]
        for ix in range(0x10):
            for iy in range(0x10):
                bitmap = self.GetBitmapChr(iy*16+ix)

                for y in range(8):
                    for x in range(8):
                        self.imageCHR0.SetRGB(ix*8+x, iy*8+y, *palette[int(bitmap[y*8+x])])

    def GenerateCHR1(self):
        palette = [(0, 0, 0), (0x30, 0x30, 0xFF), (0x7F, 0x7F, 0xFF), (0xFF, 0xFF, 0xFF)]
        for ix in range(0x10):
            for iy in range(0x10):
                bitmap = self.GetBitmapChr(iy*16+ix+0x100)

                for y in range(8):
                    for x in range(8):
                        self.imageCHR1.SetRGB(ix*8+x, iy*8+y, *palette[int(bitmap[y*8+x])])

    def _RenderCHRToImageXY(self, image, rendered, px, py):
        for y in range(8):
            for x in range(8):
                p = (y << 3) | x
                try:
                    image.SetRGB(px | x, py | y, rendered[p*3+0], rendered[p*3+1], rendered[p*3+2])
                except:
                    pass

    def _RenderCHRToImage(self, image, rendered, i):
        ix = i & 0x1F
        iy = (i >> 5) & 0x1F
        self._RenderCHRToImageXY(image, rendered, ix << 3, iy << 3)

    def GenerateNametable1(self):
        for i in range(0x3C0):
            rendered = self.GetRenderedNt(i)
            self._RenderCHRToImage(self.imageNametable1, rendered, i)

    def GenerateNametable2(self):
        for i in range(0x400, 0x7C0):
            rendered = self.GetRenderedNt(i)
            self._RenderCHRToImage(self.imageNametable2, rendered, i)

    def GenerateNametable3(self):
        for i in range(0x800, 0xBC0):
            rendered = self.GetRenderedNt(i)
            self._RenderCHRToImage(self.imageNametable3, rendered, i)

    def GenerateNametable4(self):
        for i in range(0xC00, 0xFC0):
            rendered = self.GetRenderedNt(i)
            self._RenderCHRToImage(self.imageNametable4, rendered, i)

    def GenerateOams(self):
        self.oam = []
        for i in range(0, 64):
            oam = {'y': self.GetPPUOam(i*4+0), 'tile': self.GetPPUOam(i*4+1), 'attribute': self.GetPPUOam(i*4+2),
                   'x': self.GetPPUOam(i*4+3)}
            self.oam.append(oam)
        tile = self.oam[0]['tile']
        rendered = self.GetRenderedOam(0)
        for i in range(len(rendered)):
            if rendered[i] != 0:
                rendered[i] = 0xFF
        self._RenderCHRToImageXY(self.imageOam, rendered, self.oam[0]['x'], self.oam[0]['y'])
        print(self.oam[0]['x'])
        print(self.oam[0]['y'])
        try:
            self.imageOam.SetRGB(self.oam[0]['x'], self.oam[0]['y'], *(0xFF, 0x69, 0xB4))
        except:
            pass



