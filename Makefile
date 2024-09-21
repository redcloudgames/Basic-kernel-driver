# Makefile for compiling kernel module and user-space program

obj-m += basic_kernel_driver.o

all: kernel user

# Compile the kernel module
kernel:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# Compile the user-space program
user: user_program.c
    gcc -o user_program user_program.c

clean:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
    rm -f user_program
