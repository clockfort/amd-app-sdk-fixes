#root makefile 

DEPTH = .

include $(DEPTH)/make/openclsdkdefs.mk

SUBDIRS = samples
      
include $(DEPTH)/make/openclsdkrules.mk 

