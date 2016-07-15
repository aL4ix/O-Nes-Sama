# -*- coding: utf-8 -*- 

###########################################################################
## Python code generated with wxFormBuilder (version Jun 17 2015)
## http://www.wxformbuilder.org/
##
## PLEASE DO "NOT" EDIT THIS FILE!
###########################################################################

import wx
import wx.xrc
import wx.grid

TIMER_ID1 = 1000
TIMER_ID2 = 1001

###########################################################################
## Class AddBreakpointDialog
###########################################################################

class AddBreakpointDialog ( wx.Dialog ):
	
	def __init__( self, parent ):
		wx.Dialog.__init__ ( self, parent, id = wx.ID_ANY, title = u"Add Breakpoint", pos = wx.DefaultPosition, size = wx.DefaultSize, style = wx.DEFAULT_DIALOG_STYLE )
		
		self.SetSizeHintsSz( wx.DefaultSize, wx.DefaultSize )
		
		bSizer4 = wx.BoxSizer( wx.VERTICAL )
		
		bSizer11 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.m_staticText1 = wx.StaticText( self, wx.ID_ANY, u"Type:", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText1.Wrap( -1 )
		bSizer11.Add( self.m_staticText1, 1, wx.TOP|wx.RIGHT|wx.LEFT, 5 )
		
		self.m_staticText5 = wx.StaticText( self, wx.ID_ANY, u"Address:", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText5.Wrap( -1 )
		bSizer11.Add( self.m_staticText5, 1, wx.TOP|wx.RIGHT|wx.LEFT, 5 )
		
		self.m_staticText6 = wx.StaticText( self, wx.ID_ANY, u"Value:", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText6.Wrap( -1 )
		bSizer11.Add( self.m_staticText6, 1, wx.TOP|wx.RIGHT|wx.LEFT, 5 )
		
		
		bSizer4.Add( bSizer11, 0, wx.EXPAND, 5 )
		
		bSizer10 = wx.BoxSizer( wx.HORIZONTAL )
		
		comboTypeChoices = [ u"Address", u"Value", u"Time" ]
		self.comboType = wx.ComboBox( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, comboTypeChoices, 0 )
		bSizer10.Add( self.comboType, 0, wx.ALL, 5 )
		
		self.textCtrlAddress = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
		self.textCtrlAddress.SetMaxLength( 4 ) 
		bSizer10.Add( self.textCtrlAddress, 0, wx.ALL, 5 )
		
		self.textCtrlValue = wx.TextCtrl( self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
		self.textCtrlValue.SetMaxLength( 4 ) 
		bSizer10.Add( self.textCtrlValue, 0, wx.ALL, 5 )
		
		
		bSizer4.Add( bSizer10, 1, wx.EXPAND, 5 )
		
		bSizer12 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.m_button4 = wx.Button( self, wx.ID_ANY, u"Add", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer12.Add( self.m_button4, 0, wx.ALL, 5 )
		
		self.m_button10 = wx.Button( self, wx.ID_ANY, u"Cancel", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer12.Add( self.m_button10, 0, wx.ALL, 5 )
		
		
		bSizer4.Add( bSizer12, 1, wx.ALIGN_RIGHT, 5 )
		
		
		self.SetSizer( bSizer4 )
		self.Layout()
		bSizer4.Fit( self )
		
		self.Centre( wx.BOTH )
		
		# Connect Events
		self.m_button4.Bind( wx.EVT_BUTTON, self.EventABAddBreakpoint )
		self.m_button10.Bind( wx.EVT_BUTTON, self.EventABCancel )
	
	def __del__( self ):
		pass
	
	
	# Virtual event handlers, overide them in your derived class
	def EventABAddBreakpoint( self, event ):
		event.Skip()
	
	def EventABCancel( self, event ):
		event.Skip()
	

###########################################################################
## Class MainFrame
###########################################################################

class MainFrame ( wx.Frame ):
	
	def __init__( self, parent ):
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"PPU Debugger", pos = wx.DefaultPosition, size = wx.Size( 830,580 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )
		
		self.SetSizeHintsSz( wx.DefaultSize, wx.DefaultSize )
		
		bSizer1 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.panelMain = wx.Panel( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer81 = wx.BoxSizer( wx.VERTICAL )
		
		self.staticRunning = wx.StaticText( self.panelMain, wx.ID_ANY, u"Stopped", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.staticRunning.Wrap( -1 )
		self.staticRunning.SetFont( wx.Font( wx.NORMAL_FONT.GetPointSize(), 70, 90, 92, False, wx.EmptyString ) )
		
		bSizer81.Add( self.staticRunning, 0, wx.ALL, 5 )
		
		bSizer13 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.buttonContinue = wx.Button( self.panelMain, wx.ID_ANY, u"Next BP", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer13.Add( self.buttonContinue, 0, wx.ALL|wx.ALIGN_RIGHT, 5 )
		
		self.buttonRun = wx.Button( self.panelMain, wx.ID_ANY, u"Run", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer13.Add( self.buttonRun, 0, wx.ALL, 5 )
		
		self.buttonPause = wx.Button( self.panelMain, wx.ID_ANY, u"Pause", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer13.Add( self.buttonPause, 0, wx.ALL, 5 )
		
		
		bSizer81.Add( bSizer13, 0, 0, 5 )
		
		self.m_panel7 = wx.Panel( self.panelMain, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.SIMPLE_BORDER|wx.TAB_TRAVERSAL )
		bSizer9 = wx.BoxSizer( wx.VERTICAL )
		
		self.m_staticText2 = wx.StaticText( self.m_panel7, wx.ID_ANY, u"Breakpoints", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText2.Wrap( -1 )
		bSizer9.Add( self.m_staticText2, 0, wx.ALL, 5 )
		
		listBoxBreakpointsChoices = []
		self.listBoxBreakpoints = wx.ListBox( self.m_panel7, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, listBoxBreakpointsChoices, 0 )
		bSizer9.Add( self.listBoxBreakpoints, 0, wx.ALL|wx.EXPAND, 5 )
		
		bSizer8 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.m_button5 = wx.Button( self.m_panel7, wx.ID_ANY, u"Add", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer8.Add( self.m_button5, 0, wx.ALL, 5 )
		
		self.m_button6 = wx.Button( self.m_panel7, wx.ID_ANY, u"Edit", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer8.Add( self.m_button6, 0, wx.ALL, 5 )
		
		self.m_button7 = wx.Button( self.m_panel7, wx.ID_ANY, u"Delete", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer8.Add( self.m_button7, 0, wx.ALL, 5 )
		
		
		bSizer9.Add( bSizer8, 1, wx.EXPAND, 5 )
		
		
		self.m_panel7.SetSizer( bSizer9 )
		self.m_panel7.Layout()
		bSizer9.Fit( self.m_panel7 )
		bSizer81.Add( self.m_panel7, 0, wx.ALL|wx.EXPAND, 5 )
		
		self.m_panel8 = wx.Panel( self.panelMain, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.SIMPLE_BORDER|wx.TAB_TRAVERSAL )
		bSizer10 = wx.BoxSizer( wx.VERTICAL )
		
		self.m_staticText3 = wx.StaticText( self.m_panel8, wx.ID_ANY, u"Watch", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText3.Wrap( -1 )
		bSizer10.Add( self.m_staticText3, 0, wx.ALL, 5 )
		
		self.m_grid1 = wx.grid.Grid( self.m_panel8, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.HSCROLL )
		
		# Grid
		self.m_grid1.CreateGrid( 5, 2 )
		self.m_grid1.EnableEditing( True )
		self.m_grid1.EnableGridLines( True )
		self.m_grid1.EnableDragGridSize( False )
		self.m_grid1.SetMargins( 0, 0 )
		
		# Columns
		self.m_grid1.EnableDragColMove( False )
		self.m_grid1.EnableDragColSize( True )
		self.m_grid1.SetColLabelSize( 30 )
		self.m_grid1.SetColLabelValue( 0, u"Variable" )
		self.m_grid1.SetColLabelValue( 1, u"Value" )
		self.m_grid1.SetColLabelAlignment( wx.ALIGN_CENTRE, wx.ALIGN_CENTRE )
		
		# Rows
		self.m_grid1.EnableDragRowSize( True )
		self.m_grid1.SetRowLabelSize( 80 )
		self.m_grid1.SetRowLabelAlignment( wx.ALIGN_CENTRE, wx.ALIGN_CENTRE )
		
		# Label Appearance
		
		# Cell Defaults
		self.m_grid1.SetDefaultCellAlignment( wx.ALIGN_LEFT, wx.ALIGN_TOP )
		bSizer10.Add( self.m_grid1, 0, wx.ALL, 5 )
		
		bSizer11 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.m_button8 = wx.Button( self.m_panel8, wx.ID_ANY, u"Add", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer11.Add( self.m_button8, 0, wx.ALL, 5 )
		
		self.m_button9 = wx.Button( self.m_panel8, wx.ID_ANY, u"Edit", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer11.Add( self.m_button9, 0, wx.ALL, 5 )
		
		self.m_button10 = wx.Button( self.m_panel8, wx.ID_ANY, u"Delete", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer11.Add( self.m_button10, 0, wx.ALL, 5 )
		
		
		bSizer10.Add( bSizer11, 1, wx.EXPAND, 5 )
		
		
		self.m_panel8.SetSizer( bSizer10 )
		self.m_panel8.Layout()
		bSizer10.Fit( self.m_panel8 )
		bSizer81.Add( self.m_panel8, 1, wx.EXPAND |wx.ALL, 5 )
		
		bSizer101 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.staticTextConnection = wx.StaticText( self.panelMain, wx.ID_ANY, u"Disconnected", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.staticTextConnection.Wrap( -1 )
		self.staticTextConnection.SetFont( wx.Font( wx.NORMAL_FONT.GetPointSize(), 70, 90, 92, False, wx.EmptyString ) )
		
		bSizer101.Add( self.staticTextConnection, 0, wx.ALL, 5 )
		
		self.buttonConnect = wx.Button( self.panelMain, wx.ID_ANY, u"Connect", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer101.Add( self.buttonConnect, 0, wx.ALL, 5 )
		
		self.buttonDisconnect = wx.Button( self.panelMain, wx.ID_ANY, u"Disconnect", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer101.Add( self.buttonDisconnect, 0, wx.ALL, 5 )
		
		
		bSizer81.Add( bSizer101, 1, wx.EXPAND, 5 )
		
		
		self.panelMain.SetSizer( bSizer81 )
		self.panelMain.Layout()
		bSizer81.Fit( self.panelMain )
		bSizer1.Add( self.panelMain, 0, wx.ALL, 5 )
		
		self.m_panel3 = wx.Panel( self, wx.ID_ANY, wx.DefaultPosition, wx.Size( -1,-1 ), wx.TAB_TRAVERSAL )
		bSizer5 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.m_notebook2 = wx.Notebook( self.m_panel3, wx.ID_ANY, wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
		self.m_panel42 = wx.Panel( self.m_notebook2, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer16 = wx.BoxSizer( wx.VERTICAL )
		
		bSizer17 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.buttonCHR = wx.Button( self.m_panel42, wx.ID_ANY, u"Update", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer17.Add( self.buttonCHR, 0, wx.ALL, 5 )
		
		self.labelCHR = wx.StaticText( self.m_panel42, wx.ID_ANY, u"Position:", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.labelCHR.Wrap( -1 )
		bSizer17.Add( self.labelCHR, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		
		bSizer16.Add( bSizer17, 0, wx.EXPAND, 5 )
		
		self.m_panel11 = wx.Panel( self.m_panel42, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer18 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.panelCHR0 = wx.Panel( self.m_panel11, wx.ID_ANY, wx.DefaultPosition, wx.Size( 256,256 ), wx.TAB_TRAVERSAL )
		self.panelCHR0.SetMinSize( wx.Size( 256,256 ) )
		self.panelCHR0.SetMaxSize( wx.Size( 256,256 ) )
		
		bSizer18.Add( self.panelCHR0, 0, wx.ALL, 1 )
		
		self.panelCHR1 = wx.Panel( self.m_panel11, wx.ID_ANY, wx.DefaultPosition, wx.Size( 256,256 ), wx.TAB_TRAVERSAL )
		self.panelCHR1.SetMinSize( wx.Size( 256,256 ) )
		self.panelCHR1.SetMaxSize( wx.Size( 256,256 ) )
		
		bSizer18.Add( self.panelCHR1, 0, wx.ALL, 1 )
		
		
		self.m_panel11.SetSizer( bSizer18 )
		self.m_panel11.Layout()
		bSizer18.Fit( self.m_panel11 )
		bSizer16.Add( self.m_panel11, 1, wx.EXPAND |wx.ALL, 5 )
		
		
		self.m_panel42.SetSizer( bSizer16 )
		self.m_panel42.Layout()
		bSizer16.Fit( self.m_panel42 )
		self.m_notebook2.AddPage( self.m_panel42, u"Pattern", False )
		self.m_panel43 = wx.Panel( self.m_notebook2, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer14 = wx.BoxSizer( wx.VERTICAL )
		
		bSizer15 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.buttonNametables = wx.Button( self.m_panel43, wx.ID_ANY, u"Update", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer15.Add( self.buttonNametables, 0, wx.ALL, 5 )
		
		self.labelNT = wx.StaticText( self.m_panel43, wx.ID_ANY, u"Position:", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.labelNT.Wrap( -1 )
		bSizer15.Add( self.labelNT, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		
		bSizer14.Add( bSizer15, 0, wx.EXPAND, 5 )
		
		self.m_panel111 = wx.Panel( self.m_panel43, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		gSizer1 = wx.GridSizer( 0, 2, 2, 2 )
		
		self.panelNametable1 = wx.Panel( self.m_panel111, wx.ID_ANY, wx.DefaultPosition, wx.Size( 256,240 ), wx.TAB_TRAVERSAL )
		self.panelNametable1.SetMinSize( wx.Size( 256,240 ) )
		self.panelNametable1.SetMaxSize( wx.Size( 256,240 ) )
		
		gSizer1.Add( self.panelNametable1, 0, wx.ALL, 0 )
		
		self.panelNametable2 = wx.Panel( self.m_panel111, wx.ID_ANY, wx.DefaultPosition, wx.Size( 256,240 ), wx.TAB_TRAVERSAL )
		self.panelNametable2.SetMinSize( wx.Size( 256,240 ) )
		self.panelNametable2.SetMaxSize( wx.Size( 256,240 ) )
		
		gSizer1.Add( self.panelNametable2, 0, wx.ALL, 0 )
		
		self.panelNametable3 = wx.Panel( self.m_panel111, wx.ID_ANY, wx.DefaultPosition, wx.Size( 256,240 ), wx.TAB_TRAVERSAL )
		self.panelNametable3.SetMinSize( wx.Size( 256,240 ) )
		self.panelNametable3.SetMaxSize( wx.Size( 256,240 ) )
		
		gSizer1.Add( self.panelNametable3, 0, wx.ALL, 0 )
		
		self.panelNametable4 = wx.Panel( self.m_panel111, wx.ID_ANY, wx.DefaultPosition, wx.Size( 256,240 ), wx.TAB_TRAVERSAL )
		self.panelNametable4.SetMinSize( wx.Size( 256,240 ) )
		self.panelNametable4.SetMaxSize( wx.Size( 256,240 ) )
		
		gSizer1.Add( self.panelNametable4, 0, wx.ALL, 0 )
		
		
		self.m_panel111.SetSizer( gSizer1 )
		self.m_panel111.Layout()
		gSizer1.Fit( self.m_panel111 )
		bSizer14.Add( self.m_panel111, 0, wx.ALL, 6 )
		
		
		self.m_panel43.SetSizer( bSizer14 )
		self.m_panel43.Layout()
		bSizer14.Fit( self.m_panel43 )
		self.m_notebook2.AddPage( self.m_panel43, u"Nametables", True )
		self.m_panel17 = wx.Panel( self.m_notebook2, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer22 = wx.BoxSizer( wx.VERTICAL )
		
		bSizer23 = wx.BoxSizer( wx.HORIZONTAL )
		
		self.ButtonSprites = wx.Button( self.m_panel17, wx.ID_ANY, u"Update", wx.DefaultPosition, wx.DefaultSize, 0 )
		bSizer23.Add( self.ButtonSprites, 0, wx.ALL, 5 )
		
		self.m_staticText10 = wx.StaticText( self.m_panel17, wx.ID_ANY, u"Position:", wx.DefaultPosition, wx.DefaultSize, 0 )
		self.m_staticText10.Wrap( -1 )
		bSizer23.Add( self.m_staticText10, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5 )
		
		
		bSizer22.Add( bSizer23, 0, wx.EXPAND, 5 )
		
		bSizer24 = wx.BoxSizer( wx.VERTICAL )
		
		self.panelOam = wx.Panel( self.m_panel17, wx.ID_ANY, wx.DefaultPosition, wx.Size( 512,480 ), wx.TAB_TRAVERSAL )
		bSizer24.Add( self.panelOam, 0, wx.ALL, 5 )
		
		
		bSizer22.Add( bSizer24, 1, wx.EXPAND, 5 )
		
		
		self.m_panel17.SetSizer( bSizer22 )
		self.m_panel17.Layout()
		bSizer22.Fit( self.m_panel17 )
		self.m_notebook2.AddPage( self.m_panel17, u"Oam", False )
		
		bSizer5.Add( self.m_notebook2, 1, wx.EXPAND |wx.ALL, 0 )
		
		
		self.m_panel3.SetSizer( bSizer5 )
		self.m_panel3.Layout()
		bSizer5.Fit( self.m_panel3 )
		bSizer1.Add( self.m_panel3, 1, wx.EXPAND |wx.ALL, 0 )
		
		
		self.SetSizer( bSizer1 )
		self.Layout()
		self.timerContinue = wx.Timer()
		self.timerContinue.SetOwner( self, TIMER_ID1 )
		self.timerRun = wx.Timer()
		self.timerRun.SetOwner( self, TIMER_ID2 )
		
		self.Centre( wx.BOTH )
		
		# Connect Events
		self.buttonContinue.Bind( wx.EVT_BUTTON, self.EventContinue )
		self.buttonRun.Bind( wx.EVT_BUTTON, self.EventRun )
		self.buttonPause.Bind( wx.EVT_BUTTON, self.EventPause )
		self.m_button5.Bind( wx.EVT_BUTTON, self.EventAddBreakpoint )
		self.m_button7.Bind( wx.EVT_BUTTON, self.EventDeleteBreakpoint )
		self.buttonConnect.Bind( wx.EVT_BUTTON, self.EventConnect )
		self.buttonDisconnect.Bind( wx.EVT_BUTTON, self.EventDisconnect )
		self.buttonCHR.Bind( wx.EVT_BUTTON, self.EventUpdateCHR )
		self.panelCHR0.Bind( wx.EVT_ERASE_BACKGROUND, self.EventEraseBackground )
		self.panelCHR0.Bind( wx.EVT_MOTION, self.EventCHRMotion )
		self.panelCHR0.Bind( wx.EVT_PAINT, self.EventPaintCHR0 )
		self.panelCHR1.Bind( wx.EVT_ERASE_BACKGROUND, self.EventEraseBackground )
		self.panelCHR1.Bind( wx.EVT_MOTION, self.EventCHRMotion )
		self.panelCHR1.Bind( wx.EVT_PAINT, self.EventPaintCHR1 )
		self.buttonNametables.Bind( wx.EVT_BUTTON, self.EventUpdateNametables )
		self.panelNametable1.Bind( wx.EVT_ERASE_BACKGROUND, self.EventEraseBackground )
		self.panelNametable1.Bind( wx.EVT_LEFT_DOWN, self.EventNT1LeftDown )
		self.panelNametable1.Bind( wx.EVT_MOTION, self.EventNTMotion )
		self.panelNametable1.Bind( wx.EVT_PAINT, self.EventPaintNametable1 )
		self.panelNametable2.Bind( wx.EVT_ERASE_BACKGROUND, self.EventEraseBackground )
		self.panelNametable2.Bind( wx.EVT_PAINT, self.EventPaintNametable2 )
		self.panelNametable3.Bind( wx.EVT_ERASE_BACKGROUND, self.EventEraseBackground )
		self.panelNametable3.Bind( wx.EVT_PAINT, self.EventPaintNametable3 )
		self.panelNametable4.Bind( wx.EVT_ERASE_BACKGROUND, self.EventEraseBackground )
		self.panelNametable4.Bind( wx.EVT_PAINT, self.EventPaintNametable4 )
		self.ButtonSprites.Bind( wx.EVT_BUTTON, self.EventUpdateOam )
		self.panelOam.Bind( wx.EVT_ERASE_BACKGROUND, self.EventEraseBackground )
		self.panelOam.Bind( wx.EVT_PAINT, self.EventPaintOam )
		self.Bind( wx.EVT_TIMER, self.EventTimerContinue, id=TIMER_ID1 )
		self.Bind( wx.EVT_TIMER, self.EventTimerRun, id=TIMER_ID2 )
	
	def __del__( self ):
		pass
	
	
	# Virtual event handlers, overide them in your derived class
	def EventContinue( self, event ):
		event.Skip()
	
	def EventRun( self, event ):
		event.Skip()
	
	def EventPause( self, event ):
		event.Skip()
	
	def EventAddBreakpoint( self, event ):
		event.Skip()
	
	def EventDeleteBreakpoint( self, event ):
		event.Skip()
	
	def EventConnect( self, event ):
		event.Skip()
	
	def EventDisconnect( self, event ):
		event.Skip()
	
	def EventUpdateCHR( self, event ):
		event.Skip()
	
	def EventEraseBackground( self, event ):
		event.Skip()
	
	def EventCHRMotion( self, event ):
		event.Skip()
	
	def EventPaintCHR0( self, event ):
		event.Skip()
	
	
	
	def EventPaintCHR1( self, event ):
		event.Skip()
	
	def EventUpdateNametables( self, event ):
		event.Skip()
	
	
	def EventNT1LeftDown( self, event ):
		event.Skip()
	
	def EventNTMotion( self, event ):
		event.Skip()
	
	def EventPaintNametable1( self, event ):
		event.Skip()
	
	
	def EventPaintNametable2( self, event ):
		event.Skip()
	
	
	def EventPaintNametable3( self, event ):
		event.Skip()
	
	
	def EventPaintNametable4( self, event ):
		event.Skip()
	
	def EventUpdateOam( self, event ):
		event.Skip()
	
	
	def EventPaintOam( self, event ):
		event.Skip()
	
	def EventTimerContinue( self, event ):
		event.Skip()
	
	def EventTimerRun( self, event ):
		event.Skip()
	

