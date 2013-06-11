
# sw Device Driver Makefile

CC     = arm-linux-gcc

obj-m := sw.o

KDIR := /home/hanback1/best-1/SM2-P320/linux-2.6.25-sm2_p320

PWD := $(shell pwd)

MODULE_OBJS = sw.o
MODULE_SRCS = sw.c

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
