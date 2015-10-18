ccflags-y := -std=gnu99

obj-m += nullgroup.o
nullgroup-objs := src/nullgroup.o

build:
	$(MAKE) -C /lib/modules/${shell uname -r}/build/ M=${PWD} modules

clean:
	$(MAKE) -C /lib/modules/${shell uname -r}/build/ M=${PWD} clean
