#include<linux/syscalls.h>
#include<linux/kernel.h>
#include<linux/cred.h>
#include<linux/sched.h>

SYSCALL_DEFINE0(divigetpid)
{
        printk("User id is:%u\n",task_tgid_vnr(current));
      return task_tgid_vnr(current);
}
