# Disable built in rules so we don't try to build kernel objects.
.SUFFIXES:
MAKEFLAGS+=-r

CC:=clang-19

INCLUDE_DIRS:=include/ arch/x86/include/generated/ arch/x86/include/
DEFINES:=__KERNEL__ CONFIG_64BIT CONFIG_X86_64

CC_FLAGS:=$(foreach i,$(INCLUDE_DIRS),-isystem ../$i) $(foreach d,$(DEFINES),-D $d)
DEP_FLAGS=-MD -MP -MF $@.d

# Add .user to our files so our rules aren't used for kernel objects.
OBJS:=$(addsuffix .user.o,$(wildcard *.c *.S))
KERNEL_OBJS:=$(addprefix ../,net/core/trait.o arch/x86/lib/hweight.o)

test: $(OBJS) $(KERNEL_OBJS)
	$(CC) -no-pie $^ -o $@

clean:
	rm $(OBJS)

# Include header dependency info.
-include $(addsuffix .d,$(OBJS))

%.c.user.o: %.c
	$(CC) $(CC_FLAGS) $(DEP_FLAGS) -c $< -o $@

%.S.user.o: %.S
	$(CC) $(CC_FLAGS) $(DEP_FLAGS) -c $< -o $@
