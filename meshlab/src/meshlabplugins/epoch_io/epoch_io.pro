TEMPLATE      = lib
CONFIG       += plugin 
INCLUDEPATH  += ../.. ../../../../sf ../../../../code/lib/glew/include

FORMS         = ui/v3dImportDialog.ui
HEADERS       = epoch_io.h \
				epoch_reconstruction.h \
				epoch_camera.h \		
				radial_distortion.h\
				v3dImportDialog.h \
				scalar_image.h \
                maskRenderWidget.h \
                maskImageWidget.h \
                fillImage.h
				
SOURCES       = epoch_io.cpp \
                epoch_camera.cpp \
                radial_distortion.cpp \
                scalar_image.cpp \
                v3dImportDialog.cpp \ 
                maskRenderWidget.cpp \
                maskImageWidget.cpp \
                fillImage.cpp
                
TARGET        = epoch_io
DESTDIR       = ../../meshlab/plugins
QT           += xml


# the following line is needed to avoid mismatch between 
# the awful min/max macros of windows and the limits max
win32:DEFINES += NOMINMAX
unix{
	QMAKE_CC	 = gcc
	QMAKE_CXX	 = g++
	QMAKE_LINK	 = gcc
	CONFIG		+= warn_off debug_and_release
#	LIBS		+= -l3ds
}


contains(TEMPLATE,lib) {
   CONFIG(debug, debug|release) {
      unix:TARGET = $$member(TARGET, 0)_debug
      else:TARGET = $$member(TARGET, 0)d
   }
}
