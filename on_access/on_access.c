#include <const.h>
#include <whitelist.h>

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>
#define  DEVICE_NAME "antv_char"
#define  CLASS_NAME  "antvc"



/*  Code is revised from  https://github.com/ex0dus-0x/hijack and
    https://github.com/derekmolloy/exploringBB/tree/master/extras/kernel/ebbcharmutex
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alan <ex0dus@codemuch.tech>, Derek Molloy, integrated and revised by Weifeng Lin");
MODULE_VERSION("0.0.1");
MODULE_DESCRIPTION("RIVSED for CSE331 PROJECT");


static int    majorNumber;                  ///< Store the device number -- determined automatically
static char   message[256] = {0};           ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened - for mutex
static struct class*  antv_class  = NULL; ///< The device-driver class struct pointer
static struct device* antv_device = NULL; ///< The device-driver device struct pointer

static DEFINE_MUTEX(antv_mutex);     ///< Macro to declare a new mutex


static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/**
 * Devices are represented as file structure in the kernel. The file_operations structure from
 * /linux/fs.h lists the callback functions that you wish to associated with your file operations
 * using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};






/*Run "sudo cat /boot/System.map-`uname -r` | grep sys_call_table" in shell to get sys_call_table*/
static unsigned long *sys_call_table = (unsigned long) 0xffffffff81a00240;

/* This defines a pointer to the real open() syscall */
static asmlinkage int (*old_open)(const char *filename, int flags);



/* enable use to memory page and write to it */
int
set_addr_rw(long unsigned int _addr)
{
    unsigned int level;
    pte_t *pte = lookup_address(_addr, &level);

    if (pte->pte &~ _PAGE_RW) pte->pte |= _PAGE_RW;
}

/* ensure that when cleanup occurs, make page write-protected */
int
set_addr_ro(long unsigned int _addr)
{
    unsigned int level;
    pte_t *pte = lookup_address(_addr, &level);

    pte->pte = pte->pte &~_PAGE_RW;
}



asmlinkage int
new_open(const char *filename, int flags)
{


    printk(KERN_INFO "Intercepting open(%s, %X, %X)\n", filename, flags, mode);

    /*
      calling user space function. Calling antv.
    */


    char *argv[] = { "/home/student/Desktop/cse331/bin/antv","-scan", "filename", NULL };
    static char *envp[] = {
        "HOME=/",
        "TERM=linux",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL };
    call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC );



    //sys_call_table[__NR_open] = new_open;


    /* give execution BACK to the original syscall */
    return (*old_open)(filename, flags);
}



static int __init
init(void)
{
    printk(KERN_INFO "Initializing ON_ACCESS Module!\n");

    // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "ANTVChar failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "ANTVChar: registered correctly with major number %d\n", majorNumber);

   // Register the device class
   antv_class = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(antv_class)){           // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(antv_class);     // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "ANTVChar: device class registered correctly\n");

   // Register the device driver
   antv_device = device_create(antv_class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(antv_device)){          // Clean up if there is an error
      class_destroy(antv_class);      // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(antv_device);
   }
   printk(KERN_INFO "ANTVChar: device class created correctly\n"); // Made it! device was initialized
   mutex_init(&antv_mutex);          // Initialize the mutex dynamically



    /* allow us to write to memory page, so that we can hijack the system call */
    set_addr_rw((unsigned long) sys_call_table);
    /* grab system call number definition from sys_call_table */
    old_open = (void *) sys_call_table[__NR_open];
    /* set the open symbol to our new_open system call definition */
    sys_call_table[__NR_open] = new_open;

    return 0;
}

static void __exit
cleanup(void)
{
    /* set the open symbol BACK to the old open system call definition */

    sys_call_table[__NR_open] = old_open;

    /* set memory page back to read-only */
    set_addr_ro((unsigned long) sys_call_table);


    mutex_destroy(&antv_mutex);                       // destroy the dynamically-allocated mutex
    device_destroy(antv_class, MKDEV(majorNumber, 0)); // remove the device
    class_unregister(antv_class);                      // unregister the device class
    class_destroy(antv_class);                         // remove the device class
    unregister_chrdev(majorNumber, DEVICE_NAME);         // unregister the major number

    printk(KERN_INFO "We are now leaving Kernel Town! Thanks for the stay!\n");
    return;
}


/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){

   if(!mutex_trylock(&antv_mutex)){                  // Try to acquire the mutex (returns 0 on fail)
    printk(KERN_ALERT "ANTVChar: Device in use by another process");
    return -EBUSY;
   }
   numberOpens++;
   printk(KERN_INFO "ANTVChar: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}


/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, message, size_of_message);

   if (error_count==0){           // success!
      printk(KERN_INFO "ANTVChar: Sent %d characters to the user\n", size_of_message);
      return (size_of_message=0); // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "ANTVChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;      // Failed -- return a bad address message (i.e. -14)
   }
}


/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using message[x] = buffer[x]
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){

   sprintf(message, "%s(%zu letters)", buffer, len);   // appending received string with its length
   size_of_message = strlen(message);                 // store the length of the stored message
   printk(KERN_INFO "ANTVChar: Received %zu characters from the user\n", len);
   return len;
}


/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   mutex_unlock(&antv_mutex);                      // release the mutex (i.e., lock goes up)
   printk(KERN_INFO "ANTVChar: Device successfully closed\n");
   return 0;
}




module_init(init);
module_exit(cleanup);
