
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h> // required for various structures related to files liked fops.
#include <asm/uaccess.h> // required for copy_from and copy_to user functions
#include <linux/semaphore.h>
#include <linux/cdev.h>

static int Major;
#define BUFFER_SIZE 1024
dev_t dev_no,dev;

struct device
{
	char array[100];
	struct semaphore sem;
}char_dev;


int open(struct inode *inode, struct file *filp)
 {
     printk(KERN_INFO "Inside open \n");
     if(down_interruptible(&char_dev.sem)) {
          printk(KERN_INFO " could not hold semaphore");
          return -1;
     }
     return 0;
}



int release(struct inode *inode, struct file *filp)
{
        printk (KERN_INFO "Inside close \n");
        printk(KERN_INFO "Releasing semaphore");
        up(&char_dev.sem);
        return 0;
}


ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp)
{
       unsigned long ret;
       printk("Inside read \n");
       ret = copy_to_user(buff, char_dev.array, count);
       return ret;
}



/*ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp)

{

       unsigned long ret;

       printk(KERN_INFO "Inside write \n");
       printk("*offp:%lld\n",*offp);
       ret = copy_from_user(char_dev.array+*offp, buff, count);
       *offp+=count;
       return count;

}*/
static ssize_t device_write(struct file *fp, const char *buff, size_t length, loff_t *ppos)
{
        int maxbytes;           /* maximum bytes that can be read from ppos to BUFFER_SIZE*/
        int bytes_to_write;     /* gives the number of bytes to write*/
        int bytes_writen;       /* number of bytes actually writen*/
        maxbytes = BUFFER_SIZE - *ppos;
        if (maxbytes > length)
                bytes_to_write = length;
        else
                bytes_to_write = maxbytes;
        printk("*ppos:%lld\n",*ppos);
        bytes_writen = bytes_to_write - copy_from_user(char_dev.array + *ppos, buff, bytes_to_write);
        printk(KERN_INFO "charDev : device has been written %d\n", bytes_writen);
        *ppos += bytes_writen;
        printk(KERN_INFO "charDev : device has been written\n");
        return bytes_writen;
}

static loff_t device_lseek(struct file *file, loff_t offset, int orig)
{
        loff_t new_pos = 0;
        printk(KERN_INFO "charDev : lseek function in work\n");
        switch (orig) {
        case 0 :        /*seek set*/
                new_pos = offset;
                break;
        case 1 :        /*seek cur*/
                new_pos = file->f_pos + offset;
                break;
        case 2 :        /*seek end*/
                new_pos = BUFFER_SIZE - offset;
                break;
        }
        if (new_pos > BUFFER_SIZE)
                new_pos = BUFFER_SIZE;
        if (new_pos < 0)
                new_pos = 0;
        file->f_pos = new_pos;
        return new_pos;
}


struct file_operations fops = {

 read:  read,

 write:  device_write,

 open:   open,

 release: release,

 llseek: device_lseek

};



struct cdev *kernel_cdev;

int char_dev_init (void)
{
     int ret;
     dev_t dev_no;
     kernel_cdev = cdev_alloc();
     kernel_cdev->ops = &fops;
     kernel_cdev->owner = THIS_MODULE;
     printk (" Inside init module\n");

     ret = alloc_chrdev_region( &dev_no , 0, 1,"chr_arr_dev");
     if (ret < 0) {
          printk("Major number allocation is failed\n");
          return ret;
     }

     Major = MAJOR(dev_no);
     dev = MKDEV(Major,0);
     sema_init(&char_dev.sem,1);
     printk (" The major number for your device is %d\n", Major);
     ret = cdev_add(kernel_cdev,dev,1);      //將cdev增加上去，這個函式特別重要，因為他操作了模組的「生」

     if(ret < 0 )
     {
          printk(KERN_INFO "Unable to allocate cdev");
          return ret;
     }

     return 0;

}



void char_dev_cleanup(void)
{
     printk(KERN_INFO " Inside cleanup_module\n");
     cdev_del(kernel_cdev);             //上面是操作「生」，那這個就是決定模組的「死」
     unregister_chrdev_region(Major, 1);
}



MODULE_LICENSE("GPL");

module_init(char_dev_init);

module_exit(char_dev_cleanup);
