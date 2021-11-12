#include<linux/syscalls.h>
#include<linux/kernel.h>
#include<linux/cred.h>
#include<linux/sched.h>

SYSCALL_DEFINE0(Varunprocess)
{
    struct task_struct *parent=current->parent;
    printk("parent_process_pid: %d \n", parent->pid);

    printk("current_process_pid: %d \n", current->pid);
    return 0;

