#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>

// device
#define MAX 32
static char buffer[MAX];
static dev_t devno;
// device class
static struct class *pclass;
// device cdev
static struct cdev pchar_cdev;
// device operations

static int pchar_open(struct inode *pinode, struct file *pfile);
static int pchar_close(struct inode *pinode, struct file *pfile);
static ssize_t pchar_write(struct file *pfile, const char *ubuf, size_t ubufsize, loff_t *poffset);
static ssize_t pchar_read(struct file *pfile, char *ubuf, size_t ubufsize, loff_t *poffset);

static struct file_operations pchar_fops = {
    .open = pchar_open,
    .release = pchar_close,
    .write = pchar_write,
    .read = pchar_read,
};

// driver init
static int __init pchar_init(void)
{
    int ret;
    struct device *pdevice;
    pr_info("%s: pchar_init() called.\n", THIS_MODULE->name);
    // allocate device number
    ret = alloc_chrdev_region(&devno, 0, 1, "pchar");
    if (ret < 0)
    {
        pr_err("%s: alloc_chrdev_region() failed.\n", THIS_MODULE->name);
        return ret;
    }
    pr_info("%s: alloc_chrdev_region() allocated devno: %d/%d\n",
            THIS_MODULE->name, MAJOR(devno), MINOR(devno));
    // create device class
    pclass = class_create("pchar_class");
    if (IS_ERR(pclass))
    {
        pr_err("%s: class_create() failed.\n", THIS_MODULE->name);
        unregister_chrdev_region(devno, 1);
        return -1;
    }
    pr_info("%s: class_create() created device class.\n", THIS_MODULE->name);
    // create device file
    pdevice = device_create(pclass, NULL, devno, NULL, "pchar%d", 0);
    if (IS_ERR(pdevice))
    {
        pr_err("%s: device_create() failed.\n", THIS_MODULE->name);
        class_destroy(pclass);
        unregister_chrdev_region(devno, 1);
        return -1;
    }
    pr_info("%s: device_create() created device file.\n", THIS_MODULE->name);
    // init cdev
    cdev_init(&pchar_cdev, &pchar_fops);
    // add cdev in kernel
    ret = cdev_add(&pchar_cdev, devno, 1);
    if (ret < 0)
    {
        pr_err("%s: cdev_add() failed.\n", THIS_MODULE->name);
        device_destroy(pclass, devno);
        class_destroy(pclass);
        unregister_chrdev_region(devno, 1);
        return ret;
    }
    pr_info("%s: cdev_add() added device in kernel.\n", THIS_MODULE->name);
    return 0;
}

// driver exit
static void __exit pchar_exit(void)
{
    pr_info("%s: pchar_exit() called.\n", THIS_MODULE->name);
    // delete cdev from kernel
    cdev_del(&pchar_cdev);
    pr_info("%s: cdev_del() delete device from kernel.\n", THIS_MODULE->name);
    // destroy device file
    device_destroy(pclass, devno);
    pr_info("%s: device_destroy() destroyed device file.\n", THIS_MODULE->name);
    // destroy device class
    class_destroy(pclass);
    pr_info("%s: class_destroy() destroyed device class.\n", THIS_MODULE->name);
    // release device number
    unregister_chrdev_region(devno, 1);
    pr_info("%s: unregister_chrdev_region() released device number.\n", THIS_MODULE->name);
}

// file operations
static int pchar_open(struct inode *pinode, struct file *pfile)
{
    pr_info("%s: pchar_open() called.\n", THIS_MODULE->name);
    return 0;
}

static int pchar_close(struct inode *pinode, struct file *pfile)
{
    pr_info("%s: pchar_close() called.\n", THIS_MODULE->name);
    return 0;
}

static ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    int ret;
    pr_info("%s: pchar_write() called.\n", THIS_MODULE->name);
    // copy user buffer into device buffer
    ret = copy_from_user(buffer, ubuf, ubufsize);
    if (ret != 0)
    {
        pr_info("%s: copy_from_user() failed.\n", THIS_MODULE->name);
        return -EFAULT;
    }
    return ubufsize;
}

static ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    int ret;
    pr_info("%s: pchar_read() called.\n", THIS_MODULE->name);
    // copy device buffer into user buffer
    ret = copy_to_user(ubuf, buffer, ubufsize);
    if (ret != 0)
    {
        pr_info("%s: copy_to_user() failed.\n", THIS_MODULE->name);
        return -EFAULT;
    }
    return ubufsize;
}

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rutvik Kadam");
MODULE_DESCRIPTION("Simple Pseudo Char Device Driver");
