TARGET = dnote.exe
CC = gcc
RC = windres

#Debug
#CFLAGS = -I../inc -Ic:/usr/MinGW/inc -O0 -g3 -Wall -fmessage-length=0 -finput-charset=cp932 -fexec-charset=cp932
#Release
CFLAGS = -I../inc -Ic:/usr/MinGW/inc -O1 -Wall -fmessage-length=0 -finput-charset=cp932 -fexec-charset=cp932

CPPFLAGS = -c

LDFLAGS = -Wl,--subsystem,windows -mwindows
LDLIBS = -lcomctl32 -lcomdlg32 -luser32 -lgdi32 -lkernel32 -limm32
OBJECTS = Config.o DateTime.o File.o Font.o LinkedList.o Version.o WinMain.o EditWnd.o EditWndBuffer.o MainWnd.o MainWndMenu.o ModalDialog.o SomeCtrl.o StsBar.o resource.o

$(TARGET): $(OBJECTS)
		$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
		$(RM) $(OBJECTS)
		$(RM) $(TARGET)

Config.o: ../sys/Config/Config.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

DateTime.o: ../sys/DateTime/DateTime.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

File.o: ../sys/File/File.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

Font.o: ../sys/Font/Font.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

LinkedList.o: ../sys/LinkedList/LinkedList.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

Version.o: ../sys/Version/Version.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

WinMain.o: ../sys/WinMain/WinMain.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

EditWnd.o: ../wnd/EditWnd/EditWnd.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

EditWndBuffer.o: ../wnd/EditWnd/EditWndBuffer.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

MainWnd.o: ../wnd/MainWnd/MainWnd.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

MainWndMenu.o: ../wnd/MainWnd/MainWndMenu.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

ModalDialog.o: ../wnd/ModalDlg/ModalDialog.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

SomeCtrl.o: ../wnd/SomeCtrl/SomeCtrl.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

StsBar.o: ../wnd/StsBar/StsBar.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

resource.o: ../resources/resource.rc
		$(RC) $< -o $@
