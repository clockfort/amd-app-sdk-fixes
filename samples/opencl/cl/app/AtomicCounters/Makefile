DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= AtomicCounters
EXE_TARGET_INSTALL   	= AtomicCounters

####
#
#  C/CPP files
#
####

FILES 	= AtomicCounters 

CLFILES = AtomicCounters_Kernels.cl

LLIBS  += SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

