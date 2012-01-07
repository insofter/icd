# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)

obj-m := itd-kmod.o

# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else

#KERNELDIR ?= ~/projects/icd2/linux-2.6.30 
KERNELDIR ?= ~/projects/linux-2.6.29.3-MMnet1000
PWD := $(shell pwd)

default:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-linux- -C $(KERNELDIR) M=$(PWD) modules
clean:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-linux- -C $(KERNELDIR) M=$(PWD) clean

endif
