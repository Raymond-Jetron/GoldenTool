TEMPLATE = subdirs
win32:MAKEFILE = windows.Makefile
else:MAKEFILE = linux.Makefile
SUBDIRS += \
    build/qt/GoldenMakeData \
    build/qt/GoldenCreatePoint \
    build/qt/GoldenQueryData \
    build/qt/HistoryDataSync
