include (../../sharedfilter.pri)

HEADERS       = filter_perceptualmetric.h \
                ../../meshlab/meshmodel.h \
                perceptualmetrics.h
				
SOURCES       = filter_perceptualmetric.cpp \
                ../../meshlab/meshmodel.cpp \ 
                $$GLEWCODE

TARGET        = filter_perceptualmetric

CONFIG       += opengl

