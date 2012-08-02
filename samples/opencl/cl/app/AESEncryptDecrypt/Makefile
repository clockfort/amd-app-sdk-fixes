DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
INSTALL_TO_PUBLIC       = 1
EXE_TARGET 		= AESEncryptDecrypt
EXE_TARGET_INSTALL   	= AESEncryptDecrypt

####
#
#  C/CPP files
#
####

FILES 		= AESEncryptDecrypt 
CLFILES		= AESEncryptDecrypt_Kernels.cl
IMAGES		= input512.bmp

LLIBS  		+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

