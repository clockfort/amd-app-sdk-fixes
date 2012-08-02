DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
SDK_UTIL_NOT_REQUIRED   = 1
EXE_TARGET 		= KernelLaunch
EXE_TARGET_INSTALL   	= KernelLaunch

####
#
#  C/CPP files
#
####

FILES 	= KernelLaunch \
		  Log \
		  Shared \
		  Timer
CLFILES = KernelLaunch_Kernels.cl

ifeq ($(OS),lnx)
ifndef MINGW
LDFLAGS += -lrt
endif
endif

include $(DEPTH)/make/openclsdkrules.mk 

