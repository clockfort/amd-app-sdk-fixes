DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= TransferOverlap
EXE_TARGET_INSTALL   	= TransferOverlap

####
#
#  C/CPP files
#
####

FILES 	= TransferOverlap \
		  Log \
		  Timer

ifeq ($(OS),lnx)
ifndef MINGW
LDFLAGS += -lrt
endif
endif

CLFILES = TransferOverlap_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

