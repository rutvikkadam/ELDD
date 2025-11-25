#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>

// This is necessary to access internal kernel symbols like the modules list head
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A kernel module to list all loaded modules.");


static int __init list_modules_init(void) {
    struct module *mod;
    int count = 0;

    printk(KERN_INFO "--- Listing all loaded kernel modules ---\n");

    // Iterate over the list of modules using the kernel API
    // 'THIS_MODULE' is a pointer to the current module's structure
    // We use 'list_for_each_entry' macro to safely iterate
    list_for_each_entry(mod, THIS_MODULE->list.prev, list) {
        printk(KERN_INFO "Module %d: %s\n", ++count, mod->name);
    }
    
    printk(KERN_INFO "--- Total modules found: %d ---\n", count);
    return 0;
}

static void __exit list_modules_exit(void) {
    printk(KERN_INFO "Module listing module unloaded.\n");
}

module_init(list_modules_init);
module_exit(list_modules_exit);

