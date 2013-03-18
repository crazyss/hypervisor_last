
MKDIR = mkdir

TOPDIR = $(CURDIR)
export TOPDIR
OBJDIR := $(CURDIR)/obj
export OBJDIR


LDSCRITP = $(TOPDIR)/configure/helloos.lds

SUBDIRS = bsp hypervisor lib


CFLAGS = -m32
ASFLAGS = --32 -march=i486 -mtune=i486
#LDFLAGS = -T $(LDSCRITP) --print-map -N 

SOBJS = helloos.o lowlevel_init.o begin.o lib.o int_entry.o
SOBJS += dummy.o
COBJS = start.o font.o mouse.o


#
# compiler rule
#
%.o: %.c
	$(CC) $(CC_ARCH_SPEC) $(CFLAGS_OPTIONS_ALL) -c -o $(OBJDIR)/$@ $<

#
# assembler rule
#
%.o: %.s
	$(AS) $(CC_ARCH_SPEC) $(CFLAGS_OPTIONS_ALL) -c -o $(OBJDIR)/$@ $<






all: $(OBJDIR) $(SUBDIRS)
	@echo $(wildcard *lib)
	@echo $(wildcard $(OBJDIR)/*.o)

$(OBJDIR):
	$(MKDIR) -p $(OBJDIR)





$(SUBDIRS)::
	$(MAKE) -C $@

include buildrules/linker.rules
	
#
#helloos: $(SOBJS) $(LDSCRITP) $(COBJS)
#	ld $(SOBJS) $(COBJS) -o $@ $(LDFLAGS)
#helloos.bin: helloos
#	objcopy -O binary $< $@
#dump: helloos.bin
#	objdump -b binary -D -mi8086 $<
#boot.img: helloos.bin
#	dd if=$< of=boot.img bs=512 count=1024
#run.x86: boot.img
#	kvm -d cpu -fda $< -net none -no-kvm
#
.PHONY: clean

clean: 
	$(MAKE) TARGET=clean $(SUBDIRS)
	$(RM) -r obj
	#$(RM) -f boot.img helloos $(SOBJS) $(COBJS) helloos.bin
