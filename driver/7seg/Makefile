
# 7segment Device Driver Makefile

CC     = arm-linux-gcc

obj-m := 7segment.o

KDIR := ~/linux-2.6.25-sm2_p320

PWD := $(shell pwd)

MODULE_OBJS = 7segment.o
MODULE_SRCS = 7segment.c

TEST_TARGET = test
TEST_SRCS = test.c

all : $(MODULE_OBJS) $(TEST_TARGET)

$(MODULE_OBJS) : $(MODULE_SRCS)
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

$(TEST_TARGET) : $(TEST_SRCS)
	$(CC) $(TEST_SRCS) -o $@

clean:
	rm -f *.ko *.o *.mod.* .*.cmd Module.symvers modules.order
	rm -f $(TEST_TARGET)
