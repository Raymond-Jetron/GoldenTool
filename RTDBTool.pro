TEMPLATE = subdirs
win32:MAKEFILE = windows.Makefile
else:MAKEFILE = linux.Makefile
SUBDIRS += \
    build/qt/MakeData \
    build/qt/CreatePoint \
    build/qt/QueryData \
    build/qt/HistoryDataSync
