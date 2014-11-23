ARM_OBJS := reg.o psr.o setup.o int_asm.o SWI_Handler.o \
	SWI_Handler_C.o IRQ_Handler_C.o handler.o
ARM_OBJS := $(ARM_OBJS:%=$(KDIR)/arm/%)

KOBJS += $(ARM_OBJS)
