/*
 *	Andrew Capatina
 *	4/27/2022
 *
 *	Custom ioctl() implementation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>

#include "operations.h"

dev_t dev = 0;
static struct class *dev_class;
static struct cdev test_cdev;

/*
 * Function Prototypes.
 */
static int __init test_driver_init(void);
static void __exit test_driver_exit(void);
static int test_open(struct inode * inode, struct file * file);
static int test_release(struct inode *inode, struct file *file);
static ssize_t test_read(struct file *filp, char __user *buf, size_t len, loff_t * off);
static ssize_t test_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long test_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/*
** File operation sturcture
*/
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = test_read,
        .write          = test_write,
        .open           = test_open,
        .unlocked_ioctl = test_ioctl,
        .release        = test_release,
};

/*
** This function will be called when we open the Device file
*/
static int test_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device opened.\n");
        return 0;
}
/*
** This function will be called when we close the Device file
*/
static int test_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device closed.\n");
        return 0;
}
/*
** This function will be called when we read the Device file
*/
static ssize_t test_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Read function.\n");
        return 0;
}
/*
** This function will be called when we write the Device file
*/
static ssize_t test_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Write function.\n");
        return len;
}

static long test_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	u_int8_t value;

	switch(cmd)
	{
		case WR_VALUE:
			
			if(copy_from_user(&value, (u_int8_t*) arg, sizeof(value)))
			{
				printk(KERN_ERR "ioctl: Write failed.\n");
				return -1;
			}
			
			printk(KERN_INFO "ioctl: Write received value: %x \n", value);
			
			break;		

		case RD_VALUE:
			value = 0x0F;
			if(copy_to_user((u_int8_t*) arg, &value, sizeof(value)))
			{
				printk(KERN_ERR "ioctl: Read failed.\n");
				return -1;
			}

			printk(KERN_INFO "ioctl: Read success.\n");
			
			break;
		default:
			printk(KERN_INFO "Default\n");
			break;
	}

	return 0;

}

static int __init test_driver_init(void)
{

	if((alloc_chrdev_region(&dev, 0, 1, "test_dev")) < 0)
	{
		printk(KERN_ERR "Cannot allocate major number.\n");
		return -1;
	}
	printk(KERN_INFO "Major=%d, minor=%d \n", MAJOR(dev), MINOR(dev));

	cdev_init(&test_cdev, &fops);

	if((cdev_add(&test_cdev, dev, 1)) < 0)
	{
		printk(KERN_ERR "Cannot add the device to the system.\n");
		goto r_class;
	}

	if((dev_class = class_create(THIS_MODULE, "test_class")) == NULL)
	{
		printk(KERN_ERR "Cannot create the struct class.\n");
		goto r_class;
	}

	if((device_create(dev_class, NULL, dev, NULL, "test_dev")) == NULL)
	{
		printk(KERN_ERR "Cannot create the device. \n");
		goto r_device;
	}
	printk(KERN_INFO "Device driver insert done!\n");
	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev, 1);

	return -1;
}

static void __exit test_driver_exit(void)
{
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&test_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Device driver removed!\n");

}

module_init(test_driver_init);
module_exit(test_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrew Capatina");
MODULE_DESCRIPTION("ioctl() example for custom drivers.");
MODULE_VERSION("1.0");



