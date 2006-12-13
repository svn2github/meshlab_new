HEADERS        = interfaces.h \
		         GLLogStream.h \
                 mainwindow.h \
                 meshmodel.h \
                 glarea.h \
                 filterscript.h \
                 filterparameter.h \
                 plugindialog.h \
                 customDialog.h \
                 lightingDialog.h \
                 filterScriptDialog.h \
                 plugin_support.h \
                 saveSnapshotDialog.h \
                 savemaskexporter.h \
                 changetexturename.h \
                 GenericELDialog.h \
                 stdpardialog.h \
                 ../../../sf/wrap/gui/trackball.h\
                 ../../../sf/wrap/gui/trackmode.h\
                 ../../../sf/wrap/gl/trimesh.h
SOURCES        = main.cpp \
                 mainwindow_Init.cpp \
                 mainwindow_RunTime.cpp\
                 meshmodel.cpp \
		 GLLogStream.cpp \
                 glarea.cpp \
                 plugin_support.cpp \
                 plugindialog.cpp \
                 filterscript.cpp \
                 customDialog.cpp \
                 lightingDialog.cpp \
                 filterScriptDialog.cpp \
                 saveSnapshotDialog.cpp \
                 savemaskexporter.cpp \
                 changetexturename.cpp \
                 stdpardialog.cpp \
                 ../../../sf/wrap/gui/trackball.cpp\
                 ../../../sf/wrap/gui/trackmode.cpp \
                 ../../../code/lib/glew/src/glew.c

FORMS          =	ui/filterScriptDialog.ui \
                    ui/customDialog.ui\
                    ui/lightingProperties.ui \
					ui/savesnapshotDialog.ui\						 
					ui/aboutDialog.ui \
					ui/renametexture.ui \
					ui/savemaskexporter.ui \
					ui/GenericELDialog.ui


RESOURCES     = meshlab.qrc

# to add windows icon 
RC_FILE = meshlab.rc

QT           += opengl 
QT           += xml
QT           += network

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

DEFINES += GLEW_STATIC

INCLUDEPATH += . ../../../sf ../../../code/lib/glew/include
CONFIG += stl
#win32:LIBS	+= ../../../code/lib/glew/lib/glew32.lib 
win32-g++{
#LIBS	+= glew32
#LIBPATH += ../../../code/lib/glew/lib/
}

unix{
	LIBS		+= -L../../../code/lib/glew/lib -lGLEW
	QMAKE_CC	 = gcc
	QMAKE_CXX	 = g++
	QMAKE_LINK	 = gcc
	CONFIG		+= warn_off debug_and_release
        LIBS            += -L../../../code/lib/glew/lib -lGLEW
}
