

ASFLAGS = --32 -march=i486 -mtune=i486
CFLAGS = -m32 -fno-stack-protector -I../include

COBJS = start.o font.o mouse.o int.o keyboard.o memtest.o mm.o fifo8.o serial.o console.o mmu.o libmmu.o
SOBJS = int_entry.o libmemtest.o





include $(TOPDIR)/buildrules/subdir.rules
