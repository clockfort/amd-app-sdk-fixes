ifeq ($(BITS), 32)
ifdef release
OUTPUTDIR := $(BUILDDIR)/$(RELEASE_DIR)/$(X86_DIR)/
CFLAGS += $(C_RELEASE_FLAG)
ifneq ($(OS), lnx) 
LDFLAGS += /INCREMENTAL:NO
endif 
else
OUTPUTDIR := $(BUILDDIR)/$(DEBUG_DIR)/$(X86_DIR)/
CFLAGS += $(C_DEBUG_FLAG)
ifneq ($(OS), lnx)
LDFLAGS += /INCREMENTAL:YES
endif
endif
else
ifdef release
OUTPUTDIR := $(BUILDDIR)/$(RELEASE_DIR)/$(X86_64_DIR)/
CFLAGS += $(C_RELEASE_FLAG)
else
OUTPUTDIR := $(BUILDDIR)/$(DEBUG_DIR)/$(X86_64_DIR)/
CFLAGS += $(C_DEBUG_FLAG)
endif
endif

ifdef SAMPLE_EXE
EXE_OUTPUT        := $(EXE_TARGET)$(BINSUFFIX)
OUTPUT            := $(OUTPUTDIR)$(EXE_OUTPUT) 
else
ifdef SDK_STATIC_LIB
STATIC_LIB_OUTPUT := $(LIBPREFIX)$(LIB_TARGET)$(STATIC_LIBSUFFIX)
OUTPUT		      := $(OUTPUTDIR)$(STATIC_LIB_OUTPUT)
else
ifdef SDK_SHARED_LIB
SHARED_LIB_OUTPUT := $(LIBPREFIX)$(SHARED_TARGET)$(SHARED_LIBSUFFIX)
SHARED_LIB_FILE   := $(LIBPREFIX)$(SHARED_TARGET)$(STATIC_LIBSUFFIX)
OUTPUT		  := $(SHARED_LIB_OUTPUT)
LDFLAGS           += $(LD_SHARED_FLAG)
endif
endif
endif

# SDKUtil static library is required to build any target for OpenCL
ifdef OPENCL
ifndef INDEPENDENT_SAMPLE
ifdef release
SDK_UTIL_LIBS := $(DEPTH)/samples/opencl/SDKUtil/$(OUTPUTDIR)/$(LIBPREFIX)SDKUtil$(STATIC_LIBSUFFIX)
else
SDK_UTIL_LIBS := $(DEPTH)/samples/opencl/SDKUtil/$(OUTPUTDIR)/$(LIBPREFIX)SDKUtil$(STATIC_LIBSUFFIX)
endif
endif
endif
ifdef OPENCL
	INSTALL_BIN_DIR = $(DEPTH)/samples/opencl/bin
else
ifdef CAL
	INSTALL_BIN_DIR = $(DEPTH)/samples/cal/bin
endif
endif

# Construct paths
ifeq ($(BITS), 32)
DEPDIR     := depends/$(X86_DIR)
else
DEPDIR     := depends/$(X86_64_DIR)
endif

FASTDEPS   := $(DEPTH)/make/fastdep.pl

DEPNDS_INC_DIR := -I$(DEPTH)/include

DEPNDS_INC_DIR += $(foreach f, $(INCLUDEDIRS), -I$(f))


ADDSDKLIBS := $(foreach f, $(LLIBS), $(LD_LIBLINK_PREFIX)$(f)$(LD_LIBLINK_SUFFIX))
ADDSDKLIBDIRS := $(foreach f, $(LIBDIRS), $(LD_LIBDIR_FLAG)$(f))
ADDSDKINCDIRS := $(foreach f, $(INCLUDEDIRS), $(C_INCLUDE_FLAG)$(f))

ifdef OPENCL
	ADDSDKLIBS += $(foreach f, $(OPENCL_LLIBS), $(LD_LIBLINK_PREFIX)$(f)$(LD_LIBLINK_SUFFIX))
	ADDSDKLIBDIRS += $(LD_LIBDIR_FLAG)$(OPENCL_LIBDIRS)
	ADDSDKINCDIRS += $(C_INCLUDE_FLAG)$(OPENCL_INCLUDEDIRS) $(C_INCLUDE_FLAG)$(DEPTH)/include
else
ifdef CAL
	ADDSDKLIBS += $(foreach f, $(CAL_LLIBS), $(LD_LIBLINK_PREFIX)$(f)$(LD_LIBLINK_SUFFIX))
	ADDSDKLIBDIRS += $(LD_LIBDIR_FLAG)$(CAL_LIBDIRS)
	ADDSDKINCDIRS += $(C_INCLUDE_FLAG)$(CAL_INCLUDEDIRS) $(C_INCLUDE_FLAG)$(DEPTH)/include
endif
endif


LDFLAGS += $(ADDSDKLIBS) 
LDFLAGS += $(ADDSDKLIBDIRS) 
CFLAGS += $(ADDSDKINCDIRS) 

ifeq ($(OS), lnx)
ifdef release
	export LD_LIBRARY_PATH:=$(AMDAPPSDKROOT)/lib/$(CPU_ARCH):$(AMDAPPSDKSAMPLESROOT)/lib/$(CPU_ARCH):$(LD_LIBRARY_PATH)
else
	export LD_LIBRARY_PATH:=$(AMDAPPSDKROOT)/lib/$(CPU_ARCH):$(AMDAPPSDKSAMPLESROOT)/lib/$(CPU_ARCH):$(LD_LIBRARY_PATH)
endif
endif

all: $(SUBDIRS) depend $(OUTPUT) install

# Recursive rule for subdirectories 
$(SUBDIRS):
	$(MAKE) -C $@

clean:
ifeq ($(DEPTH), .)
	rm -r -f $(INSTALL_ROOT_DIR)
endif
ifdef SUBDIRS
	for i in $(SUBDIRS); do make -C $$i clean; done
endif
ifdef SAMPLE_EXE
	rm -f -r $(BUILDDIR)
	rm -f -r $(INSTALL_ROOT_DIR)$(OUTPUTDIR)$(EXE_OUTPUT)
	rm -rf $(DEPDIR)
endif

ifdef SDK_STATIC_LIB
	rm -f -r $(BUILDDIR)
	rm -f -r $(INSTALL_ROOT_DIR)$(OUTPUTDIR)$(STATIC_LIB_OUTPUT)
	rm -rf $(DEPDIR)
endif

ifdef SDK_SHARED_LIB
	rm -f -r $(BUILDDIR)
	rm -f -r $(INSTALL_ROOT_DIR)$(OUTPUTDIR)$(SHARED_LIB_OUTPUT)
	rm -f -r $(INSTALL_ROOT_DIR)$(OUTPUTDIR)$(SHARED_LIB_FILE)
	rm -rf $(DEPDIR)
endif

ifdef REMOVE_ON_CLEAN
	rm -rf $(REMOVE_ON_CLEAN)
endif

# How to make dependencies if required for this target

DEPS := $(FILES)
DEPS := $(addprefix $(DEPDIR)/, $(DEPS))
DEPS := $(addsuffix .depend, $(DEPS))
	

depend: $(DEPS)
$(DEPDIR)/%.depend: %.cpp
	$(MKDIR) $(DEPDIR)
	$(PERL) $(FASTDEPS) -I. $(DEPNDS_INC_DIR) --obj-suffix='$(OBJSUFFIX)' --obj-prefix='$(OUTPUTDIR)/' $< > $@
$(DEPDIR)/%.depend: %.c
	$(MKDIR) $(DEPDIR)
	$(PERL) $(FASTDEPS) -I. $(DEPNDS_INC_DIR) --obj-suffix='$(OBJSUFFIX)' --obj-prefix='$(OUTPUTDIR)/' $< > $@
	
ifdef FILES
-include $(DEPS)
endif

OBJS := $(addprefix $(OUTPUTDIR)/, $(FILES))
OBJS := $(addsuffix $(OBJSUFFIX), $(OBJS))

# SDK Sample Exectuable Rules
ifdef SAMPLE_EXE
$(OUTPUT): $(OBJS) $(SDK_UTIL_LIBS)
	$(ECHO) Building $@
	$(LD) $(LD_OUTPUT_FLAG)$@ $(OBJS) $(LDFLAGS) 
endif	

# SDK Static Lib Rules
ifdef SDK_STATIC_LIB
#OBJS := $(addprefix $(OUTPUTDIR)/, $(FILES))
#OBJS := $(addsuffix $(OBJSUFFIX), $(OBJS))

ifeq ($(LIB_TARGET), SDKUtil)
SDK_UTIL_LIBS := 
endif

$(OUTPUT): $(OBJS) $(SDK_UTIL_LIBS)
	$(ECHO) Building $@	
	$(AR) $(ARFLAGS) $(AR_OUTPUT_FLAG)$@ $(OBJS)
endif

# SDK Static Lib Rules
ifdef SDK_SHARED_LIB
#OBJS := $(addprefix $(OUTPUTDIR)/, $(FILES))
#OBJS := $(addsuffix $(OBJSUFFIX), $(OBJS))

$(OUTPUT): $(OBJS) $(SDK_UTIL_LIBS)
	$(ECHO) Building $@	
	$(LD) $(LDFLAGS) $(ADDSDKLIBS) $(LD_OUTPUT_FLAG)$(OUTPUTDIR)$@ $(OBJS)
endif

# .cpp -> object

$(OUTPUTDIR)/%$(OBJSUFFIX): %.cpp $(DEPDIR)/%.depend 
	$(MKDIR) $(OUTPUTDIR)
	$(ECHO) Building $@
	$(CC) $(CFLAGS) $(C_OUTPUT_FLAG)$@ $(C_COMPILE_FLAG) $<

# .c -> object
$(OUTPUTDIR)/%$(OBJSUFFIX): %.c $(DEPDIR)/%.depend 
	$(MKDIR) $(OUTPUTDIR)
	$(ECHO) Building $@
	$(CC) $(CFLAGS) $(C_OUTPUT_FLAG)$@ $(C_COMPILE_FLAG) $<


install: $(OUTPUT)
# Install executables 
ifdef EXE_TARGET_INSTALL 
ifdef EXE_DIR_INSTALL
	$(INSTALL) -D $(OUTPUTDIR)$(EXE_OUTPUT) $(EXE_DIR_INSTALL)/$(EXE_OUTPUT)
else
	$(INSTALL) -D $(OUTPUTDIR)$(EXE_OUTPUT) $(INSTALL_BIN_DIR)/$(CPU_ARCH)/$(EXE_OUTPUT)
endif
endif 

# Install OpenCL kernel files
ifdef CLFILES
	for f in $(CLFILES); do \
		$(INSTALL) -D $$f $(INSTALL_BIN_DIR)/$(CPU_ARCH)/$$f; \
	done
endif

ifdef IMAGES 
	for f in $(IMAGES); do \
		$(INSTALL) -D $$f $(INSTALL_BIN_DIR)/$(CPU_ARCH)/$$f; \
	done
endif



# Install static libraries
ifdef LIB_TARGET_INSTALL	
ifndef LIB_DIR_INSTALL
	$(INSTALL) -D $(OUTPUTDIR)$(STATIC_LIB_OUTPUT) $(INSTALL_LIB_DIR)$(CPU_ARCH)/$(STATIC_LIB_OUTPUT)
else
	$(INSTALL) -D $(OUTPUTDIR)$(STATIC_LIB_OUTPUT) $(LIB_DIR_INSTALL)$(CPU_ARCH)/$(STATIC_LIB_OUTPUT)
endif
endif 

# Install dynamic libraries
ifdef SHARED_TARGET_INSTALL	
ifndef SHARED_DIR_INSTALL
	$(INSTALL) -D $(OUTPUTDIR)$(SHARED_LIB_OUTPUT) $(INSTALL_ROOT_DIR)/bin/$(CPU_ARCH)/$(SHARED_LIB_OUTPUT)
	$(INSTALL) -D $(OUTPUTDIR)$(SHARED_LIB_FILE) $(INSTALL_LIB_DIR)$(CPU_ARCH)/$(SHARED_LIB_FILE)
else
	$(INSTALL) -D $(OUTPUTDIR)$(SHARED_LIB_OUTPUT) $(SHARED_DIR_INSTALL)$(CPU_ARCH)/$(SHARED_LIB_OUTPUT)
	$(INSTALL) -D $(OUTPUTDIR)$(SHARED_LIB_FILE) $(SHARED_DIR_INSTALL)$(CPU_ARCH)/$(SHARED_LIB_FILE)
endif
endif






.PHONY: all $(SUBDIRS) depend clean 	 





	
