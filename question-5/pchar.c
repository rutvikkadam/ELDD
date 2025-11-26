#include <linux/module.h>
#include <linux/kfifo.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/wait.h>

// device operations
static int pchar_open(struct inode *pinode, struct file *pfile);
static int pchar_close(struct inode *pinode, struct file *pfile);
static ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t ubufsize, loff_t *poffset);
static ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t ubufsize, loff_t *poffset);

// device private struct
#define MAX 32
typedef struct pchar_device
{
    struct kfifo buffer;
    struct cdev cdev;

    wait_queue_head_t wr_wq;   // block writer when buffer is full
    wait_queue_head_t rd_wq;   // block reader when buffer is empty

} pchar_device_t;

// number of devices
static int devcnt = 4;
module_param(devcnt, int, 0444);

// devices array
static pchar_device_t *devices;

// globals
static dev_t devno;
static int major;
static struct class *pclass;

static struct file_operations pchar_fops = {
    .owner = THIS_MODULE,
    .open = pchar_open,
    .release = pchar_close,
    .write = pchar_write,
    .read = pchar_read,
};

static int __init pchar_init(void)
{
    int ret, i;
    struct device *pdevice;

    pr_info("%s: pchar_init() called.\n", THIS_MODULE->name);

    devices = kmalloc(devcnt * sizeof(pchar_device_t), GFP_KERNEL);
    if (!devices)
    {
        ret = -ENOMEM;
        pr_err("%s: kmalloc() failed.\n", THIS_MODULE->name);
        goto kmalloc_fail;
    }

    ret = alloc_chrdev_region(&devno, 0, devcnt, "pchar");
    if (ret < 0)
    {
        pr_err("%s: alloc_chrdev_region() failed.\n", THIS_MODULE->name);
        goto alloc_fail;
    }

    major = MAJOR(devno);

    pclass = class_create("pchar_class");
    if (IS_ERR(pclass))
    {
        ret = -1;
        pr_err("%s: class_create() failed.\n", THIS_MODULE->name);
        goto class_fail;
    }

    for (i = 0; i < devcnt; i++)
    {
        dev_t devnum = MKDEV(major, i);
        pdevice = device_create(pclass, NULL, devnum, NULL, "pchar%d", i);
        if (IS_ERR(pdevice))
        {
            ret = -1;
            pr_err("%s: device_create() failed for pchar%d.\n", THIS_MODULE->name, i);
            goto dev_create_fail;
        }
    }

    for (i = 0; i < devcnt; i++)
    {
        dev_t devnum = MKDEV(major, i);
        cdev_init(&devices[i].cdev, &pchar_fops);
        ret = cdev_add(&devices[i].cdev, devnum, 1);
        if (ret < 0)
        {
            pr_err("%s: cdev_add() failed for pchar%d.\n", THIS_MODULE->name, i);
            goto cdev_fail;
        }
    }

    for (i = 0; i < devcnt; i++)
    {
        ret = kfifo_alloc(&devices[i].buffer, MAX, GFP_KERNEL);
        if (ret < 0)
        {
            pr_err("%s: kfifo_alloc() failed for pchar%d.\n", THIS_MODULE->name, i);
            goto kfifo_fail;
        }
    }

    for (i = 0; i < devcnt; i++)
    {
        init_waitqueue_head(&devices[i].wr_wq);
        init_waitqueue_head(&devices[i].rd_wq); // <-- add reader queue
    }

    return 0;

kfifo_fail:
    while (--i >= 0)
        kfifo_free(&devices[i].buffer);
    i = devcnt;

cdev_fail:
    while (--i >= 0)
        cdev_del(&devices[i].cdev);
    i = devcnt;

dev_create_fail:
    while (--i >= 0)
        device_destroy(pclass, MKDEV(major, i));
    class_destroy(pclass);

class_fail:
    unregister_chrdev_region(devno, devcnt);

alloc_fail:
    kfree(devices);

kmalloc_fail:
    return ret;
}

static void __exit pchar_exit(void)
{
    int i;

    for (i = devcnt - 1; i >= 0; i--)
        kfifo_free(&devices[i].buffer);

    for (i = devcnt - 1; i >= 0; i--)
        cdev_del(&devices[i].cdev);

    for (i = devcnt - 1; i >= 0; i--)
        device_destroy(pclass, MKDEV(major, i));

    class_destroy(pclass);
    unregister_chrdev_region(devno, devcnt);
    kfree(devices);
}

static int pchar_open(struct inode *pinode, struct file *pfile)
{
    pfile->private_data = container_of(pinode->i_cdev, pchar_device_t, cdev);
    return 0;
}

static int pchar_close(struct inode *pinode, struct file *pfile)
{
    return 0;
}


static ssize_t pchar_write(struct file *pfile, const char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    pchar_device_t *dev = pfile->private_data;
    int nbytes, ret;

    // block writer if buffer full
    ret = wait_event_interruptible(dev->wr_wq, !kfifo_is_full(&dev->buffer));
    if (ret != 0)
        return -ERESTARTSYS;

    ret = kfifo_from_user(&dev->buffer, ubuf, ubufsize, &nbytes);
    if (ret < 0)
        return ret;

    // wake reader if sleeping
    if (nbytes > 0)
        wake_up_interruptible(&dev->rd_wq);

    return nbytes;
}


static ssize_t pchar_read(struct file *pfile, char __user *ubuf, size_t ubufsize, loff_t *poffset)
{
    pchar_device_t *dev = pfile->private_data;
    int nbytes, ret;

    // block reader if buffer empty
    ret = wait_event_interruptible(dev->rd_wq, !kfifo_is_empty(&dev->buffer));
    if (ret != 0)
        return -ERESTARTSYS;

    ret = kfifo_to_user(&dev->buffer, ubuf, ubufsize, &nbytes);
    if (ret < 0)
        return ret;

    // wake writer if sleeping
    if (nbytes > 0)
        wake_up_interruptible(&dev->wr_wq);

    return nbytes;
}

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rutvik Kadam");
MODULE_DESCRIPTION("If buﬀer is empty, block the reader process until some data is written into the buﬀer");

