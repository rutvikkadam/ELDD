#include<linux/module.h>
#include"import.h"

static __init int import_init(void)
{
	printk(KERN_INFO "%s : import_init is called\n", THIS_MODULE->name);
	my_function();
	printk(KERN_INFO "%s : import_init is completed\n", THIS_MODULE->name);
	return 0;
}

static __exit void import_exit(void)
{
	printk(KERN_INFO "%s : import_exit is called\n", THIS_MODULE->name);
	printk(KERN_INFO "%s : my_var = %d\n", THIS_MODULE->name, my_var);
	printk(KERN_INFO "%s : import_exit is completed\n", THIS_MODULE->name);

}

module_init(import_init);
module_exit(import_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rutvik Kadam");
MODULE_DESCRIPTION("This is dependent module which will use the exported symbols");








