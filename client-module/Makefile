obj-m += module.o

all:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CC)testerddchar.c -o test
	
clean:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm test
