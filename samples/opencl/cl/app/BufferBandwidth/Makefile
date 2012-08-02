DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
SDK_UTIL_NOT_REQUIRED = 1
EXE_TARGET 		= BufferBandwidth
EXE_TARGET_INSTALL   	= BufferBandwidth

####
#
#  C/CPP files
#
####

FILES 	= BufferBandwidth \
		  Host \
		  Log \
		  Shared \
		  Timer

CLFILES = BufferBandwidth_Kernels.cl

ifeq ($(OS),lnx)
ifndef MINGW
LDFLAGS += -lrt
endif
endif


include $(DEPTH)/make/openclsdkrules.mk 

