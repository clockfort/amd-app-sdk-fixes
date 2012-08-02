DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 1
EXE_TARGET 		= EigenValue
EXE_TARGET_INSTALL   	= EigenValue

####
#
#  C/CPP files
#
####

# -O3 removed from optimization flags for linux 32 system
ifeq ($(OS), lnx) # Linux settings 
  ifeq ($(BITS), 32)
    C_RELEASE_FLAG    := -funroll-all-loops  -fexpensive-optimizations -ffast-math -finline-functions -frerun-loop-opt -static-libgcc
  endif
endif

FILES 	= EigenValue
CLFILES	= EigenValue_Kernels.cl

LLIBS  	+= SDKUtil

include $(DEPTH)/make/openclsdkrules.mk 

