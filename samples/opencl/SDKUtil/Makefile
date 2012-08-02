DEPTH = ../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SDK_STATIC_LIB	        = 1
LIB_TARGET 		= SDKUtil
LIB_TARGET_INSTALL   	= SDKUtil
LIB_DIR_INSTALL   	= $(DEPTH)/TempSDKUtil/lib/

####
#
#  C/CPP files
#
####

FILES = SDKApplication \
        SDKBitMap \
	SDKCommon \
	SDKCommandArgs \
	SDKFile \
	SDKThread

INCLUDEDIRS += include 

include $(DEPTH)/make/openclsdkrules.mk 
