#COBJS = main.o

include ../buildrules/subdir.rules



BSP ?= x86


ASFLAGS = --32 -march=i486 -mtune=i486
CFLAGS = -m32 -I../include

COBJS = start.o font.o mouse.o int.o
SOBJS = int_entry.o



######################################

all: $(COBJS) $(SOBJS)

clean:
	$(RM) $(COBJS) $(SOBJS)

