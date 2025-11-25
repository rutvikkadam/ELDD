#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/wait.h>

#define MAX_DEVICES 4

typedef struct pchar_device {
    struct cdev cdev;
    wait_queue_head_t op_wq;   // wait queue for open
    int is_open;               // flag: 1 = open, 0 = free
} pchar_device_t;

static int devcnt = MAX_DEVICES;
module_param(devcnt, int, 0444);

static pchar_device_t *devices;
static dev_t devno;
static int major;
static struct class *pclass;

static int pchar_open(struct inode *inode, struct file *file)
{
    pchar_device_t *dev = container_of(inode->i_cdev, pchar_device_t, cdev);
    file->private_data = dev;

    // block if device is already open
    if (wait_event_interruptible(dev->op_wq, dev->is_open == 0))
        return -ERESTARTSYS;

    dev->is_open = 1;  // mark device as open
    pr_info("pchar: device opened\n");
    return 0;
}

static int pchar_close(struct inode *inode, struct file *file)
{
    pchar_device_t *dev = file->private_data;
    dev->is_open = 0;               // free device
    wake_up_interruptible(&dev->op_wq); // wake up blocked openers
    pr_info("pchar: device closed\n");
    return 0;
}

static struct file_operations pchar_fops = {
    .owner = THIS_MODULE,
    .open = pchar_open,
    .release = pchar_close,
};

static int __init pchar_init(void)
{
    int ret, i;

    devices = kmalloc(devcnt * sizeof(pchar_device_t), GFP_KERNEL);
    if (!devices)
        return -ENOMEM;

    ret = alloc_chrdev_region(&devno, 0, devcnt, "pchar");
    if (ret < 0)
        goto fail_alloc;

    major = MAJOR(devno);
    pclass = class_create(THIS_MODULE->name);
    if (IS_ERR(pclass)) {
        ret = PTR_ERR(pclass);
        goto fail_class;
    }

    for (i = 0; i < devcnt; i++) {
        dev_t devnum = MKDEV(major, i);
        cdev_init(&devices[i].cdev, &pchar_fops);
        ret = cdev_add(&devices[i].cdev, devnum, 1);
        if (ret < 0)
            goto fail_cdev;

        init_waitqueue_head(&devices[i].op_wq);
        devices[i].is_open = 0;

        device_create(pclass, NULL, devnum, NULL, "pchar%d", i);
    }

    pr_info("pchar: module loaded\n");
    return 0;

fail_cdev:
    while (--i >= 0)
        cdev_del(&devices[i].cdev);
    class_destroy(pclass);
fail_class:
    unregister_chrdev_region(devno, devcnt);
fail_alloc:
    kfree(devices);
    return ret;
}

static void __exit pchar_exit(void)
{
    int i;
    for (i = 0; i < devcnt; i++) {
        cdev_del(&devices[i].cdev);
        device_destroy(pclass, MKDEV(major, i));
    }
    class_destroy(pclass);
    unregister_chrdev_region(devno, devcnt);
    kfree(devices);
    pr_info("pchar: module unloaded\n");
}

module_init(pchar_init);
module_exit(pchar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mansi Parsekar");
MODULE_DESCRIPTION("single-process open with blocking");

