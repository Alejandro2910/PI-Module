#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#define  DEVICE_NAME "ddchar"
#define  CLASS_NAME	"ddc"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TAAM");
MODULE_DESCRIPTION("First trial of reading and sending module");
MODULE_VERSION("0.1");

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  ddcharClass  = NULL; ///< The device-driver class struct pointer
static struct device* ddcharDevice = NULL; ///< The device-driver device struct pointer

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

static int __init ddchar_init(void){
	printk(KERN_INFO, "Initiaziling de character device\n");
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if(majorNumber < 0){
		printk(KERN_ALERT "DDChar: failed to register a major number\n");
		return majorNumber;
	}
	printk(KERN_INFO "DDChar: registered correctly with major number %d\n", majorNumber);
	
	ddcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(ddcharClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(ddcharClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "DDChar: device class registered correctly\n");
   
   ddcharDevice = device_create(ddcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(ddcharDevice)){               // Clean up if there is an error
      class_destroy(ddcharClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(ddcharDevice);
   }
   printk(KERN_INFO "DDChar: device created correctly\n"); // Made it! device was initialized
   return 0;
}

static void __exit ddchar_exit(void){
   device_destroy(ddcharClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(ddcharClass);                          // unregister the device class
   class_destroy(ddcharClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "DDChar: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "DDChar: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	printk(KERN_ALERT “This operation is not supported.\n”);
	return -EINVAL;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "DDChar: Received %zu characters from the user containing %s\n", len, message);
   return len;
}

static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "DDChar: Device successfully closed\n");
   return 0;
}

module_init(ddchar_init);
module_exit(ddchar_exit);
