SHELL = sh -xv

ifdef SRCDIR

VPATH = $(SRCDIR)

# Add your targets here
TARGETS = demo.hex

all: $(TARGETS)

include config.mk

# For each target define a TARGETNAME_SRC, TARGETNAME_OBJ and define any
# additional dependencies for your the target TARGETNAME.elf file (just
# define the dependencies, a generic rule for .elf target exists in
# config.mk).
CALCULATOR_SRC = multi-precision.c

ifeq ($(TARGET),stm32f4)
  CALCULATOR_SRC += demo.S # <-------------------- Change this away from the demo
endif

CALCULTATOR_OBJ = $(call objs,$(CALCULATOR_SRC))

demo.elf: $(CALCULTATOR_OBJ) libhal.a


# Don't forget to add all objects to the OBJ variable
OBJ += $(CALCULTATOR_OBJ) # <-------------------- Check this works

# Include generated dependencies
-include $(filter %.d,$(OBJ:.o=.d))

else
#######################################
# Out-of-tree build mechanism.        #
# You shouldn't touch anything below. #
#######################################
.SUFFIXES:

OBJDIR := build

.PHONY: $(OBJDIR)
$(OBJDIR): %:
	+@[ -d $@ ] || mkdir -p $@
	+@$(MAKE) --no-print-directory -r -I$(CURDIR) -C $@ -f $(CURDIR)/Makefile SRCDIR=$(CURDIR) $(MAKECMDGOALS)

Makefile : ;
%.mk :: ;
% :: $(OBJDIR) ;

.PHONY: clean
clean:
	rm -rf $(OBJDIR)

endif
