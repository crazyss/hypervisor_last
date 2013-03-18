include $(TOPDIR)/buildrules/builder.rules


ASFLAGS = --32 -march=i486 -mtune=i486

SOBJS = helloos.o lowlevel_init.o begin.o 
SOBJS += dummy.o

all: $(SOBJS)


clean:
	rm -rf $(SOBJS) 
