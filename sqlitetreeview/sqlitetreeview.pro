if (!include(config.txt)) {
    error("Failed to open config.txt")
}
TARGET=Sqlitetreeview
INCLUDEPATH += \
                             ../include/Win32xx\
                             ../include/sqlite3

SOURCES += \
    main.cpp \
    Mainfrm.cpp \
    SplitterApp.cpp \
    SplitterPanes.cpp \
    StdAfx.cpp \
    Views.cpp \
    utfwstring.cpp

HEADERS += \
    Mainfrm.h \
    resource.h \
    SplitterApp.h \
    SplitterPanes.h \
    stdafx.h \
    Views.h \
    utfwstring.h

LIBS += -L../lib/sqlite3

LIBS += -lsqlite3


win32:RC_FILE += Resource.rc
