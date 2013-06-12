#include <linux/init.h>
#include <linux/module.h>
#include <asm/hardware.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <asm/ioctl.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/signal.h>

#define DRIVER_AUTHOR 	"G11" 
#define DRIVER_DESC	 	"SW Driver" 
#define SW_MAJOR			0
#define SW_NAME 			"SW"
#define SW_MODULE_VERSIO	"SW V0.1"
#define SW_PHY_ADDR			0x14802000
#define SW_ADDR_RANGE 		0x1000
#define TIMER_INTERVAL			(2*HZ/10)	

//Global variable
static int SW_usage = 0;
static int SW_major = 0;
static struct timer_list mytimer;
static unsigned char value;
static pid_t id;
static unsigned int  SW_ioremap;
static unsigned char *SW_addr;

// define functions...
int SW_open(struct inode *minode, struct file *mfile)
{
	if(SW_usage != 0) return -EBUSY;
	
	SW_ioremap = (unsigned int)ioremap(SW_PHY_ADDR,SW_ADDR_RANGE);

	if(!check_mem_region(SW_ioremap, SW_ADDR_RANGE))
		request_region(SW_ioremap, SW_ADDR_RANGE, SW_NAME);
	else
		printk("driver: unable to register this!\n");
	SW_usage = 1;

	SW_addr =  (unsigned char *)(SW_ioremap);

	return 0;
}
	
int SW_release(struct inode *minode, struct file *mfile)
{
	release_region(SW_ioremap, SW_ADDR_RANGE);
	iounmap((unsigned char *)SW_ioremap);
	del_timer(&mytimer);
	SW_usage = 0;
	return 0;
}

void mypollingfunction(unsigned long data)
{
   value = (*SW_addr & 0x07);
	if(value != 7) goto stop_poll;
	
	stop_poll:
		if(value!=7) {
			kill_proc(id,SIGUSR2,1);
		}
		mytimer.expires = get_jiffies_64() + TIMER_INTERVAL;
		add_timer(&mytimer);
}

ssize_t SW_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what)
{
	get_user(id,(int *)gdata);
	
	init_timer(&mytimer);
	mytimer.expires = get_jiffies_64() + TIMER_INTERVAL;
	mytimer.function = &mypollingfunction;
	add_timer(&mytimer);
	return length;
}

ssize_t SW_read(struct file *inode, char *gdata, size_t length, loff_t *off_what)
{
	int ret;
	ret = copy_to_user(gdata, &value, 1);
	if(ret < 0) return -1;
	return length;
}

struct file_operations SW_fops = {
	.owner	= THIS_MODULE,
	.read		= SW_read,
	.write	= SW_write,
	.open		= SW_open,
	.release	= SW_release,
};


int SW_init(void) 
{
	int result;
	result = register_chrdev(SW_MAJOR, SW_NAME, &SW_fops);
	
	if(result < 0) {
		printk(KERN_WARNING"Can't get any major\n");
		return result;
	}
	SW_major = result;
		
	printk("init module, SW major number : %d\n", result);
	
	
	return 0;
}

void SW_exit(void) 
{

	unregister_chrdev(SW_major, SW_NAME);
	
}

module_init(SW_init);
module_exit(SW_exit);

MODULE_AUTHOR(DRIVER_AUTHOR); 
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_LICENSE("Dual BSD/GPL");
