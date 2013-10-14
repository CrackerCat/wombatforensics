QT += widgets gui core concurrent
CONFIG += debug

INCLUDEPATH += ../sleuthkit/framework/tsk/framework/services/
INCLUDEPATH += ../sleuthkit/framework/tsk/framework/extraction/
VPATH += ../sleuthkit/framework/tsk/framework/services/
VPATH += ../sleuthkit/framework/tsk/framework/extraction/
HEADERS = wombatforensics.h wombatvariable.h wombatdatabase.h ui_wombatforensics.h progresswindow.h ui_progresswindow.h sleuthkit.h TskDBBlackboard.h TskImageFileTsk.h TskImgDBSqlite.h TskLog.h TskSystemPropertiesImpl.h TskSchedulerQueue.h basictools.h binviewmodel.h binviewwidget.h ui_exportdialog.h exportdialog.h
SOURCES = main.cpp wombatforensics.cpp wombatdatabase.cpp progresswindow.cpp sleuthkit.cpp TskDBBlackboard.cpp TskImageFileTsk.cpp TskImgDBSqlite.cpp TskLog.cpp TskSystemPropertiesImpl.cpp TskSchedulerQueue.cpp basictools.cpp binviewmodel.cpp binviewwidget.cpp exportdialog.cpp
RESOURCES += wombat.qrc progresswindow.qrc
DESTDIR = ./
#DESTDIR = ../build
LIBS = -lsqlite3 -lewf -ltsk -ltskframework
#LIBS = -lsqlite3 -lewf -ltsk -L/home/pasquale/libs -Wl,-rpath,/home/pasquale/libs -ltskframework

if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
mac:LIBS = $$member(LIBS, 0) $$member(LIBS, 1)_debug
win32:LIBS = $$member(LIBS, 0) $$member(LIBS, 1)d
}

#install
#target.path = ../build
target.path = ./
INSTALLS += target
