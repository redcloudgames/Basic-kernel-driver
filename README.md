Steps to Compile and Run:

1.Build the kernel driver and user program:
make

2.Load the kernel module:
sudo insmod basic_kernel_driver.ko

3.Create the device file:
sudo mknod /dev/memdev c <major_number> 0
sudo chmod 666 /dev/memdev

4.Run the user program:
./user_program

5.Unload the kernel module:
sudo rmmod basic_kernel_driver
