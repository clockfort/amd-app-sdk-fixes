DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= BinomialOptionMultiGPU
EXE_TARGET_INSTALL   	= BinomialOptionMultiGPU

####
#
#  C/CPP files
#
####

FILES 	= BinomialOptionMultiGPU
CLFILES	= BinomialOptionMultiGPU_Kernels.cl

LLIBS  	+= SDKUtil


include $(DEPTH)/make/openclsdkrules.mk 

