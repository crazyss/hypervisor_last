
include ../buildrules/subdir.rules

#
# compiler rule
#
$(OBJDIR)/%.o: %.c
	$(CC) $(CC_ARCH_SPEC) $(CFLAGS_OPTIONS_ALL) -c -o $@ $<

#
# assembler rule
#
$(OBJDIR)/%.o: %.s
	$(AS) $(CC_ARCH_SPEC) $(CFLAGS_OPTIONS_ALL) -c -o $@ $<

ASFLAGS = --32 -march=i486 -mtune=i486

SOBJS = lib.o


all: $(SOBJS)


clean:
	rm -rf $(SOBJS)
