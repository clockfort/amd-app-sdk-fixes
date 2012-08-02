DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= HistogramAtomics
EXE_TARGET_INSTALL   	= HistogramAtomics

####
#
#  C/CPP files
#
####

FILES 	= HistogramAtomics
CLFILES	= HistogramAtomics_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

