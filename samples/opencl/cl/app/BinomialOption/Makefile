DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= BinomialOption
EXE_TARGET_INSTALL   	= BinomialOption

####
#
#  C/CPP files
#
####

FILES 	= BinomialOption
CLFILES	= BinomialOption_Kernels.cl

LLIBS  	+= SDKUtil


include $(DEPTH)/make/openclsdkrules.mk 

