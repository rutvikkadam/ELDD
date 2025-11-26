#include<linux/module.h>

// create few symbols -> variables and functions
int my_var = 123;
void my_function(void);

void my_function(void)
{
	printk(KERN_INFO "%s : inside my_function\n", THIS_MODULE->name);
	printk(KERN_INFO "%s : my_var = %d\n", THIS_MODULE->name, my_var);
	my_var++;
}

// export the symbols for other modules
EXPORT_SYMBOL(my_var);				
	// will export symbol for all modules
EXPORT_SYMBOL_GPL(my_function);		
	// will export symbol only for modules which has GPL license


static __init int export_init(void)
{
	printk(KERN_INFO "%s : export_init is called\n", THIS_MODULE->name);
	my_function();
	printk(KERN_INFO "%s : export_init is completed\n", THIS_MODULE->name);
	return 0;
}

static __exit void export_exit(void)
{
	printk(KERN_INFO "%s : export_exit is called\n", THIS_MODULE->name);
	printk(KERN_INFO "%s : my_var = %d\n", THIS_MODULE->name, my_var);
	printk(KERN_INFO "%s : export_exit is completed\n", THIS_MODULE->name);
}

module_init(export_init);
module_exit(export_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rutvik Kadam");
MODULE_DESCRIPTION("This is dependency module which exports the symbols");








