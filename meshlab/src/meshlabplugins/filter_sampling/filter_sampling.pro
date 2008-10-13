include (../../shared.pri)

HEADERS       = filter_sampling.h \
				../../meshlab/meshmodel.h \
		$$VCGDIR/vcg/complex/trimesh/point_sampling.h
				
SOURCES       = filter_sampling.cpp\
				../../meshlab/meshmodel.cpp \ 
		../../meshlab/filterparameter.cpp\
		$$GLEWCODE

TARGET        = filter_sampling

CONFIG       += opengl

