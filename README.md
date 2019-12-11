# device-driver
sample code

step1:go to the directory of the driver code.
step2:make
step3:gcc char_main.c -o char_main
step4:dmesg  //watch the major number
step5:According the major number to set the character device 
      sudo mknod /dev/char_dev c major_number 0
      sudo chmod 777 /dev/char_dev
