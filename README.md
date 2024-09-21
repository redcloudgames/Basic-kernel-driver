Steps to Compile and Run:

1.Build the kernel driver and user program:</br>
make

2.Load the kernel module:</br>
sudo insmod basic_kernel_driver.ko

3.Create the device file:</br>
sudo mknod /dev/memdev c <major_number> 0</br>
sudo chmod 666 /dev/memdev

4.Run the user program:</br>
./user_program

5.Unload the kernel module:</br>
sudo rmmod basic_kernel_driver
