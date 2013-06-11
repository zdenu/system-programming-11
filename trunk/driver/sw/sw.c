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
static int sw_usage = 0;
static int sw_major = 0;
static struct timer_list mytimer;
static unsigned char value;
static pid_t id;
static unsigned int  sw_ioremap;
static unsigned char *sw_addr;

// define functions...
int sw_open(struct inode *minode, struct file *mfile)
{
	if(sw_usage != 0) return -EBUSY;
		
	sw_ioremap = (unsigned int)ioremap(SW_PHY_ADDR,SW_ADDR_RANGE);

	if(!check_mem_region(sw_ioremap, SW_ADDR_RANGE))
		request_region(sw_ioremap, SW_ADDR_RANGE, SW_NAME);
	else
		printk("driver: unable to register this!\n");

	buzzer_addr = (unsigned char *)(sw_ioremap);

	sw_usage = 1;
	return 0;
}
	
int sw_release(struct inode *minode, struct file *mfile)
{
	release_region(sw_ioremap, SW_ADDR_RANGE);
	iounmap((unsigned char *)sw_ioremap);
	del_timer(&mytimer);
	sw_usage = 0;
	return 0;
}

void mypollingfunction(unsigned long data)
{
	int j=1,k,i;
	value =0;
   value = *sw_addr & 0x07;
	if(value != 0x00) goto stop_poll;

	stop_poll:
		if(value>0) {
			kill_proc(id,SIGUSR2,1);
		}
		mytimer.expires = get_jiffies_64() + TIMER_INTERVAL;
		add_timer(&mytimer);
}

ssize_t sw_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what)
{
	get_user(id,(int *)gdata);
	
	init_timer(&mytimer);
	mytimer.expires = get_jiffies_64() + TIMER_INTERVAL;
	mytimer.function = &mypollingfunction;
	add_timer(&mytimer);
	return length;
}

ssize_t sw_read(struct file *inode, char *gdata, size_t length, loff_t *off_what)
{
	int ret;
	ret = copy_to_user(gdata, &value, 1);
	if(ret < 0) return -1;
	return length;
}

struct file_operations sw_fops = {
	.owner		= THIS_MODULE,
	.read		= sw_read,
	.write		= sw_write,
	.open		= sw_open,
	.release	= sw_release,
};


int sw_init(void) 
{
	int result;
	result = register_chrdev(SW_MAJOR, SW_NAME, &SW_fops);
	
	if(result < 0) {
		printk(KERN_WARNING"Can't get any major\n");
		return result;
	}
	sw_major = result;
		
	printk("init module, sw major number : %d\n", result);
	
	
	return 0;
}

void sw_exit(void) 
{

	unregister_chrdev(sw_major, SW_NAME);
	
}

module_init(sw_init);
module_exit(sw_exit);

MODULE_AUTHOR(DRIVER_AUTHOR); 
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_LICENSE("Dual BSD/GPL");
