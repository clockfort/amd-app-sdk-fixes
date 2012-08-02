# Print detailed output
VERBOSE := 1

# Detect platform
UNAME := $(shell uname -a)

ifeq ($(findstring CYGWIN, $(UNAME)), CYGWIN)
  ifeq ($(findstring 6.1, $(UNAME)), 6.1)
    OS := lh
  else
  ifeq ($(findstring 6.0, $(UNAME)), 6.0)
    OS := lh
  else
    OS := xp
  endif
  endif
else 
  ifeq ($(findstring MINGW, $(UNAME)), MINGW)
    OS := lnx
    MINGW := 1
  else
    ifeq ($(findstring Linux, $(UNAME)), Linux)
      OS := lnx
    else 
      ifeq ($(findstring Darwin, $(UNAME)), Darwin)
         OS := MacOSX
      else
         $(error Unknown OS:$(UNAME))
      endif
    endif
   endif
endif


# What CPU and mode?
ifeq ($(findstring WOW64, $(UNAME)), WOW64)
  BITS = 64
else
ifeq ($(findstring i686, $(UNAME)), i686)
  BITS = 32
else
ifeq ($(findstring i586, $(UNAME)), i586)
  BITS = 32
else
ifeq ($(findstring x86_64, $(UNAME)), x86_64)
  BITS = 64
else
  $(error Unknown CPU)
endif
endif
endif
endif

# Override bitness with user specified value 
ifdef bitness 
ifeq ($(bitness), 32)
  BITS = 32
endif
ifeq ($(bitness), 64)
  BITS = 64
endif
endif

#Override compiler with intel comiler if intel defined
ifdef intel 
  INTEL := 1
endif
  
###################################################
# Set up tool and target definitions based on above

ifeq ($(OS), xp) # Windows settings
ifdef INTEL
  CC                := icl
else
  CC                := cl
endif

ifeq ($(BITS), 64)
  CFLAGS            += /nologo /W3 /DWIN32 /DWINDOWS /EHsc /Zm500 /D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECATE /WX 
else
  CFLAGS            += /nologo /W3 /DWIN32 /DWINDOWS /EHsc /Zm500 /D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECATE /WX 
endif
  C_CPP_FLAG	    := /nologo /EP
  C_INCLUDE_FLAG    := /I
  C_DEBUG_FLAG      := /MTd /RTC1 /Od 
  ifeq ($(BITS), 64)
	C_RELEASE_FLAG    := /O2 /Oi /MT
	C_DEBUG_FLAG  += /Zi
  else
	C_RELEASE_FLAG    := /O2 /Oi /arch:SSE2 /MT
	C_DEBUG_FLAG  += /ZI
  endif
  C_STATIC_FLAG     := 
  C_OUTPUT_FLAG     := /Fo
  C_COMPILE_FLAG    := /c
  C_DEFINE_FLAG     := /D
ifdef INTEL
  LD                := xilink
else
  LD                := link
endif
  LDFLAGS           += /nologo /fixed:no /DEBUG /MANIFEST
  LD_LIBDIR_FLAG    := /libpath:
  LD_SHARED_FLAG    := /DLL
  LD_OUTPUT_FLAG    := /out:
  LD_LIBLINK_PREFIX :=  
  LD_LIBLINK_SUFFIX := .lib 
  LD_DEBUG_FLAG     := /debug
  LD_DEF_FLAG       := /def:
  AR                := lib
  ARFLAGS           := /nologo /NODEFAULTLIB:LIBCMT
  AR_OUTPUT_FLAG    := /out:
  AR_LIBDIR_FLAG    := /libpath:
  AR_LIBLINK_PREFIX :=  
  AR_LIBLINK_SUFFIX := .lib 
  SYSTEM_LIBS       := kernel32 gdi32 user32 advapi32 Winmm
  OBJSUFFIX         := .obj
  LIBPREFIX         := 
  SHARED_LIBSUFFIX  := .dll
  STATIC_LIBSUFFIX  := .lib
  BINSUFFIX         := .exe
  MANIFEST_TOOL     := mt /nologo    # Only needed for Windows
  MANIFEST_SUFFIX   := .manifest
  ifeq ($(BITS), 64)
	OPENCL_LLIBS := OpenCL
	CAL_LLIBS    := aticalrt64 aticalcl64  
  else
	OPENCL_LLIBS := OpenCL
	CAL_LLIBS    := aticalrt aticalcl 
  endif
else
ifeq ($(OS), lh) # Windows Vista settings
ifdef INTEL
  CC                := icl
else
  CC                := cl
endif
ifeq ($(BITS), 64)
  CFLAGS            += /nologo /W3 /DWINDOWS /EHsc /Zm500 /D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECATE /WX /FD 
else
  CFLAGS            += /nologo /W3 /DWIN32 /DWINDOWS /EHsc /Zm500 /D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECATE /FD /WX
endif
  C_CPP_FLAG	    := /nologo /EP
  C_INCLUDE_FLAG    := /I
  C_DEBUG_FLAG      := /Od /MTd /RTC1
  ifeq ($(BITS), 64)
	C_RELEASE_FLAG    := /O2 /Ob2 /Oi /MT /Ot /GS- 
	ifndef INTEL
		C_RELEASE_FLAG    += /Oy /DNDEBUG\\
	endif
	C_DEBUG_FLAG     += /Zi
  else
	C_RELEASE_FLAG    := /O2 /Ob2 /Oi /Ot /MT /GS- 
	ifndef INTEL
		C_RELEASE_FLAG    += /Oy /DNDEBUG\\
	endif
	C_DEBUG_FLAG     += /ZI
  endif 	
  C_STATIC_FLAG     := 
  C_OUTPUT_FLAG     := /Fo
  C_COMPILE_FLAG    := /c
  C_DEFINE_FLAG     := /D
ifdef INTEL
  LD                := xilink
else
  LD                := link
endif
  LDFLAGS           += /nologo /fixed:no /INCREMENTAL:NO /DEBUG /MANIFEST
  LD_LIBDIR_FLAG    := /libpath:
  LD_SHARED_FLAG    := /DLL
  LD_OUTPUT_FLAG    := /out:
  LD_LIBLINK_PREFIX :=  
  LD_LIBLINK_SUFFIX := .lib 
  LD_DEBUG_FLAG     := /debug
  LD_DEF_FLAG       := /def:
  AR                := lib
  ARFLAGS           := /nologo /NODEFAULTLIB:LIBCMT
  AR_OUTPUT_FLAG    := /out:
  AR_LIBDIR_FLAG    := /libpath:
  AR_LIBLINK_PREFIX :=  
  AR_LIBLINK_SUFFIX := .lib 
  SYSTEM_LIBS       := kernel32 gdi32 user32 advapi32 Winmm
  OBJSUFFIX         := .obj
  LIBPREFIX         := 
  SHARED_LIBSUFFIX  := .dll
  STATIC_LIBSUFFIX  := .lib
  BINSUFFIX         := .exe
  MANIFEST_TOOL     := mt     # Only needed for Windows
  MANIFEST_SUFFIX   := .manifest
  ifeq ($(BITS), 64)
	OPENCL_LLIBS := OpenCL
	CAL_LLIBS    := aticalrt64 aticalcl64  
  else
	OPENCL_LLIBS := OpenCL
	CAL_LLIBS    := aticalrt aticalcl 
  endif
  
else 
ifeq ($(OS), lnx) # Linux settings 
  ifeq ($(BITS), 64)
  else
    CFLAGS += -m32 -msse2
    LDFLAGS += -m32
  endif
ifdef INTEL
  CC                := icpc
else
  CC                := g++ 
endif
  CFLAGS            += -Wpointer-arith $(GCC_APP_FLAG) 
ifndef INTEL
  CFLAGS            += -Wfloat-equal
endif 

  C_CPP_FLAG	    := -E
  C_INCLUDE_FLAG    := -I  
  C_DEBUG_FLAG      := -g3 -ffor-scope 
  C_RELEASE_FLAG    := -O3 -funroll-all-loops  -fexpensive-optimizations -ffast-math -finline-functions -frerun-loop-opt -static-libgcc
  C_STATIC_FLAG     := 
  C_OUTPUT_FLAG     := -o 
  C_COMPILE_FLAG    := -c 
  C_DEFINE_FLAG	    := -D
ifdef INTEL
  LD                := icpc
else
  LD                := g++
endif

ifdef MINGW
  LDFLAGS           += -L/usr/X11R6/lib 
else
  LDFLAGS           += -lpthread -ldl -L/usr/X11R6/lib -lX11
endif
  LD_LIBDIR_FLAG    := -L
  LD_SHARED_FLAG    := -shared   
  LD_OUTPUT_FLAG    := -o 
  LD_LIBLINK_PREFIX := -l
  LD_LIBLINK_SUFFIX := 
  LD_DEBUG_FLAG     := 
  AR                := ar
  ARFLAGS           := -rsc 
  AR_OUTPUT_FLAG    := 
  AR_LIBDIR_FLAG    := 
  AR_LIBLINK_PREFIX :=  
  AR_LIBLINK_SUFFIX := 
  SYSTEM_LIBS      := 
  OBJSUFFIX        := .o
  LIBPREFIX        := lib
  SHARED_LIBSUFFIX := .so
  STATIC_LIBSUFFIX := .a
  BINSUFFIX        := 
  MANIFEST_SUFFIX  := 
  OPENCL_LLIBS := OpenCL
ifdef MINGW
  ifeq ($(BITS), 64)
	CAL_LLIBS    := aticalrt64 aticalcl64  
  else
	CAL_LLIBS    := aticalrt aticalcl 
  endif
else
  CAL_LLIBS	   := aticalrt aticalcl
endif 
endif
endif
endif


# Install locations
BUILDDIR := build
DEBUG_DIR := debug
RELEASE_DIR := release
X86_DIR := x86
X86_64_DIR := x86_64
ifeq ($(BITS), 64)
CPU_ARCH = x86_64
else
CPU_ARCH = x86
endif

# Compiler and Runtime
RUNTINE_BIN = $(DEPTH)/bin/$(CPU_ARCH)
INCLUDEDIRS = $(DEPTH)/samples/opencl/SDKUtil/include
OPENCL_INCLUDEDIRS = "$(AMDAPPSDKROOT)/include" 
CAL_INCLUDEDIRS = "$(AMDAPPSDKROOT)/include/CAL"
OPENCL_LIBDIRS = "$(AMDAPPSDKROOT)/lib/$(CPU_ARCH)"
CAL_LIBDIRS = "$(AMDAPPSDKROOT)/lib"

LIBDIRS = $(DEPTH)/lib/$(CPU_ARCH) $(DEPTH)/TempSDKUtil/lib/$(CPU_ARCH) 
CL_SUFFIX	   	   := .cl
CLC				   := clc

# Common
PERL    := perl
MKDIR   := mkdir -p
RM	:= rm
MAKE	:= make
CP	:= cp -f
ECHO    := @echo
INSTALL			   := install
