#COBJS = main.o
LDSCRITP = helloos.lds

SUBDIRS = bsp hypervisor lib
SUBDIRS_CLEAN = $(SUBDIRS)

CFLAGS = -m32
ASFLAGS = --32 -march=i486 -mtune=i486
LDFLAGS = -T $(LDSCRITP) --print-map -N 

SOBJS = helloos.o lowlevel_init.o begin.o lib.o int_entry.o
SOBJS += dummy.o
COBJS = start.o font.o mouse.o

#SRCS := $(SOBJS:.o=.s) $(COBJS:.o=.c)


all: $(SUBDIRS)


$(SUBDIRS)::
	$(MAKE) -C $@
	

helloos: $(SOBJS) $(LDSCRITP) $(COBJS)
	ld $(SOBJS) $(COBJS) -o $@ $(LDFLAGS)
helloos.bin: helloos
	objcopy -O binary $< $@
dump: helloos.bin
	objdump -b binary -D -mi8086 $<
boot.img: helloos.bin
	dd if=$< of=boot.img bs=512 count=1024
run.x86: boot.img
	kvm -d cpu -fda $< -net none -no-kvm

.PHONY: clean

clean: 
	$(MAKE) TARGET=clean $(SUBDIRS)
	#$(RM) -f boot.img helloos $(SOBJS) $(COBJS) helloos.bin
