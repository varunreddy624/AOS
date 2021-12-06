#include<linux/syscalls.h>
#include<linux/kernel.h>

SYSCALL_DEFINE0(Varunhello)
{
    printk("Hiii!!! Varun Here\n");
    return 0;
}
