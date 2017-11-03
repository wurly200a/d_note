.SUFFIXES: .rc .res

TARGET = dnote.exe
CC      = "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.11.25503\bin\HostX86\x86\cl.exe"
RC      = "C:\Program Files (x86)\Windows Kits\8.1\bin\x86\rc.exe"
LINK    = "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.11.25503\bin\HostX86\x86\link.exe"
LIBPATH = "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x86"

CFLAGS = //I ../inc //W4 //O2 //MT //EHsc

LDFLAGS = /SUBSYSTEM:WINDOWS /LIBPATH:$(LIBPATH)
LDLIBS  = ComCtl32.Lib ComDlg32.Lib User32.Lib Gdi32.Lib kernel32.Lib Imm32.Lib shell32.lib

OBJECTS = \
          ..\\develop\\DebugWnd\\DebugWnd.o       \
          ..\\develop\\DebugWnd\\DebugWndMenu.o	  \
          ..\\develop\\DebugWnd\\DebugWndFont.o	  \
          ..\\develop\\DebugWnd\\DebugWndConfig.o \
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
