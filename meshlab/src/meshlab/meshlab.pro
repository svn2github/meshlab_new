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
                 layerDialog.h \
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
                 filterparameter.cpp \
                 customDialog.cpp \
                 lightingDialog.cpp \
                 filterScriptDialog.cpp \
                 saveSnapshotDialog.cpp \
                 layerDialog.cpp \
                 savemaskexporter.cpp \
                 changetexturename.cpp \
                 stdpardialog.cpp \
                 ../../../sf/wrap/gui/trackball.cpp\
                 ../../../sf/wrap/gui/trackmode.cpp \
                 ../../../code/lib/glew/src/glew.c

FORMS          =    ui/layerDialog.ui \
                    ui/filterScriptDialog.ui \
                    ui/customDialog.ui\
                    ui/lightingProperties.ui \
					ui/savesnapshotDialog.ui\
					ui/aboutDialog.ui \
					ui/renametexture.ui \
					ui/savemaskexporter.ui \
					ui/congratsDialog.ui


RESOURCES     = meshlab.qrc

# to add windows icon 
RC_FILE = meshlab.rc


### the xml info list 
### the next time the app open a new extension
QMAKE_INFO_PLIST = ../install/info.plist


# to add MacOS icon 
ICON = images/meshlab.icns
# note that to add the file icons on the mac the following line does not work. 
# You have to copy the file by hand into the meshlab.app/Contents/Resources directory.
# ICON += images/meshlab_obj.icns

QT           += opengl 
QT           += xml
QT           += network

# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX

DEFINES += GLEW_STATIC

INCLUDEPATH += . .. ../../../vcglib ../../../code/lib/glew/include
CONFIG += stl

# The following define is needed in gcc to remove the asserts
win32-g++:DEFINES += NDEBUG
CONFIG(debug, debug|release) {
	win32-g++:release:DEFINES -= NDEBUG
}


