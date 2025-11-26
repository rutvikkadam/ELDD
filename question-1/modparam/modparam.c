#include<linux/module.h>
#include<linux/moduleparam.h>

// create static variables to store module parameters
static int num = 3;
static char *name = "desd";

// map above variables with module parameters
module_param(num, int, 0644);
module_param_named(u_name, name, charp, 0644);

static __init int modparam_init(void)
{
	printk(KERN_INFO "%s : modparam_init() is called\n", THIS_MODULE->name);
	for(int i = 0 ; i < num ; i++)
		printk(KERN_INFO "%s : name = %s\n", THIS_MODULE->name, name);
	printk(KERN_INFO "%s : modparam_init() is completed\n", THIS_MODULE->name);
	return 0;
}

static __exit void modparam_exit(void)
{
	printk(KERN_INFO "%s : modparam_exit() is called\n", THIS_MODULE->name);
	for(int i = 0 ; i < num ; i++)
		printk(KERN_INFO "%s : name = %s\n", THIS_MODULE->name, name);
	printk(KERN_INFO "%s : modparam_exit() is completed\n", THIS_MODULE->name);
}

module_init(modparam_init);
module_exit(modparam_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rutvik Kadam");
MODULE_DESCRIPTION("This is module with module parameters");









