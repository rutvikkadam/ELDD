#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h> // Required for struct task_struct
#include <linux/sched/signal.h> // Required for for_each_process, if needed, but here we use hierarchy

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rutvik Kadam");
MODULE_DESCRIPTION("A kernel module to list processes in the hierarchy from the current process.");


// Helper function to recursively print the process tree with indentation
void print_process_tree(struct task_struct *task, int depth) {
    struct task_struct *child;
    struct list_head *list;
    int i;

    // Print indentation
    for (i = 0; i < depth; i++) {
        printk(KERN_INFO "  ");
    }
    // Print process info: CHANGED 'state' to '__state'
    printk(KERN_INFO "|- PID: %d | Name: %s | State: %lu\n", task->pid, task->comm, (unsigned long)task->__state);

    // Recurse through children
    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        print_process_tree(child, depth + 1);
    }
}

static int __init process_tree_init(void) {
    struct task_struct *curr_task = current; // Macro to access the current task_struct
    struct task_struct *root_task = current;

    printk(KERN_INFO "Loading Process Tree Module...\n");
    printk(KERN_INFO "Starting traversal from current process (PID: %d, Name: %s)\n", curr_task->pid, curr_task->comm);

    // Optional: Traverse up to the root (init process, PID 1) to print the full system tree
    while (root_task->parent != root_task) {
        root_task = root_task->parent;
    }
    
    // Print the entire process tree starting from the root (init)
    printk(KERN_INFO "--- Full Process Hierarchy from Root (PID 1) ---\n");
    print_process_tree(root_task, 0);

    return 0;
}

static void __exit process_tree_exit(void) {
    printk(KERN_INFO "Process Tree Module Unloaded.\n");
}

module_init(process_tree_init);
module_exit(process_tree_exit);

