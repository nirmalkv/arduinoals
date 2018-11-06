modulename := alsdriver
obj-m+=alsdriver.o
all:
		make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
load:
		-rmmod $(modulename)
		insmod $(modulename).ko

unload:
	-rmmod $(modulename)

