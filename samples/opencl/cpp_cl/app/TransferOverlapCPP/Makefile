DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= TransferOverlapCPP
EXE_TARGET_INSTALL   	= TransferOverlapCPP

####
#
#  C/CPP files
#
####

FILES 	= TransferOverlapCPP \
		  Log \
		  Timer

ifeq ($(OS),lnx)
ifndef MINGW
LDFLAGS += -lrt
endif
endif

CLFILES = TransferOverlapCPP_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

