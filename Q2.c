#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>

SYSCALL_DEFINE2(diviprint,
                char __user *, input,
                int, input_length)
{
        char buffer[256];
        unsigned long left_length = input_length;
        unsigned long chunklen = sizeof(buffer);
        while( left_length > 0 ){
                if(left_length < chunklen ) chunklen =left_length;
                if( copy_from_user(buffer, input, chunklen) ){
                return -EFAULT;
        }
                left_length -= chunklen;
        }

        printk("%s\n", buffer);

        return 0;
}
