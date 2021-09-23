.SUFFIXES: .rc .res

# use "x86 Native Tools Command Prompt for VS 2019"

TARGET = dnote.exe
CC      = "cl.exe"
RC      = "rc.exe"
LINK    = "link.exe"

CFLAGS = //I ../inc //W4 //O2 //MT //EHsc

LDFLAGS = /SUBSYSTEM:WINDOWS
LDLIBS  = ComCtl32.Lib ComDlg32.Lib User32.Lib Gdi32.Lib kernel32.Lib Imm32.Lib shell32.lib

OBJECTS = \
          ..\\develop\\DebugWnd\\DebugWnd.o       \
          ..\\develop\\DebugWnd\\DebugWndMenu.o	  \
          ..\\develop\\DebugWnd\\DebugWndFont.o	  \
          ..\\develop\\DebugWnd\\DebugWndConfig.o \
          ..\\develop\\DebugWnd\\DebugWndSomeCtrl.o \
                                                  \
          ..\\sys\\Config\\Config.o				  \
          ..\\sys\\DateTime\\DateTime.o			  \
          ..\\sys\\File\\File.o					  \
          ..\\sys\\Font\\Font.o					  \
          ..\\sys\\LinkedList\\LinkedList.o		  \
          ..\\sys\\Version\\Version.o			  \
          ..\\sys\\WinMain\\WinMain.o			  \
                                                  \
          ..\\wnd\\EditWnd\\EditWnd.o			  \
          ..\\wnd\\EditWnd\\EditWndBuffer.o		  \
          ..\\wnd\\EditWnd\\EditWndBufferData.o	  \
          ..\\wnd\\EditWnd\\EditWndBufferUndoData.o \
          ..\\wnd\\MainWnd\\MainWnd.o			  \
          ..\\wnd\\MainWnd\\MainWndMenu.o		  \
          ..\\wnd\\ModalDlg\\ModalDialog.o		  \
          ..\\wnd\\ModalDlg\\ModalDialogResource.res \
          ..\\wnd\\SomeCtrl\\SomeCtrl.o			  \
          ..\\wnd\\StsBar\\StsBar.o               \
                                                  \
          ..\\resources\\resource.res             \

$(TARGET): $(OBJECTS)
		$(LINK) /OUT:$@ $(LDFLAGS) $^ $(LDLIBS)

clean:
		$(RM) $(OBJECTS)
		$(RM) $(TARGET)

.c.o:
		$(CC) //c $< $(CFLAGS) //Fo$@

.rc.res:
		$(RC) -FO $@ $<
