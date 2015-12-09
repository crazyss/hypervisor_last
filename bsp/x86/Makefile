
#Due to wbinvd not support by i386
ASFLAGS = --32 -march=i486 -mtune=i486

SOBJS += bootsector.o lowlevel_init.o begin.o 
#SOBJS += dummy.o

include $(TOPDIR)/buildrules/subdir.rules
