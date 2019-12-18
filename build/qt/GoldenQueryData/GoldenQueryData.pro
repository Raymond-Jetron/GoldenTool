TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

DEFINES -= UNICODE
DEFINES += UMBCS

#x86,x64,x86_win,x64_win,ARM,ARM64,MIPS
CPU_TYPE = x64
SolutionDir = $$PWD/../../..
ProjectName = GoldenQueryData
CONFIG(debug, debug|release) {
    compiled = Debug
}
CONFIG(release, debug|release) {
    compiled = Release
}

SOURCES += \
    $$SolutionDir/src/$$ProjectName/main.cpp

OBJECTS_DIR += $$SolutionDir/obj/$$ProjectName/$$CPU_TYPE/$$compiled
DESTDIR += $$SolutionDir/bin/$$ProjectName/$$CPU_TYPE/$$compiled
mkpath($$OBJECTS_DIR)
mkpath($$DESTDIR)

include(deployment.pri)
qtcAddDeployment()

win32{
    DIFINES += _WIN32
    DEFINES -= _LINUX
    MAKEFILE = windows.Makefile
    LIBS += -L$$SolutionDir/third/api/$$CPU_TYPE/ -lgoldenapi64
}else{
    DIFINES -= _WIN32
    DEFINES += _LINUX
    MAKEFILE = linux.Makefile
    !exists($$DESTDIR/libgoldenapi.so){
        system("cp $$SolutionDir/third/api/$$CPU_TYPE/lib* $$DESTDIR")
    }

    LIBS += -L$$DESTDIR/ -lgoldenapi
    LIBS += -lpthread

    DEPENDPATH += $$DESTDIR
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,./"
}

INCLUDEPATH += $$SolutionDir/third/api/$$CPU_TYPE \
    $$SolutionDir/third \
    $$SolutionDir/common
