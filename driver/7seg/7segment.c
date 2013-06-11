#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <asm/fcntl.h>
#include <linux/ioport.h>

#include <asm/ioctl.h>
#include <asm/hardware.h>
#include <asm/uaccess.h>
#include <asm/io.h>


#define DRIVER_AUTHOR				"G11"
#define DRIVER_DESC					"7 Segment test"
#define SEGMENT_MODULE_VERSION	"7 Segment PORT V0.1"
#define	SEGMENT_NAME				"7 Segment"
#define	SEGMENT_MAJOR_NUMBER	0

#define SEGMENT_ADDRESS			0x14800000
#define SEGMENT_ADDRESS_RANGE	0x2000

//Global variable
static unsigned int segment_usage = 0;
static unsigned int segment_major = 0;
static unsigned int segment_ioremap;
static unsigned char *segment_data;
static unsigned char *segment_grid;

int segment_open (struct inode *inode, struct file *filp)
{
	if(segment_usage != 0) return -EBUSY;
	
	segment_ioremap = (unsigned int)ioremap(SEGMENT_ADDRESS, SEGMENT_ADDRESS_RANGE);
	
	if(!check_mem_region(segment_ioremap,SEGMENT_ADDRESS_RANGE))
		request_region(segment_ioremap, SEGMENT_ADDRESS_RANGE, SEGMENT_NAME);
	else printk(KERN_WARNING"Can't get IO Region 0x%x\n", segment_ioremap);	

	segment_grid = (unsigned char *)(segment_ioremap);
	segment_data = (unsigned char *)(segment_ioremap + 0x1000);

	segment_usage = 1;	
	return 0; 
}


int segment_release (struct inode *inode, struct file *filp)
{
	release_region(segment_ioremap, SEGMENT_ADDRESS_RANGE);
	iounmap((unsigned char *)segment_ioremap);
	segment_usage = 0;
	return 0;
}


unsigned char Getsegmentcode (char x)
{
	unsigned char code;
	switch (x) {
		case '0' : code = 0x3f; break;
		case '1' : code = 0x06; break;
		case '2' : code = 0x5b; break;
		case '3' : code = 0x4f; break;
		case '4' : code = 0x66; break;
		case '5' : code = 0x6d; break;
		case '6' : code = 0x7d; break;
		case '7' : code = 0x07; break;
		case '8' : code = 0x7f; break;										
		case '9' : code = 0x6f; break;		
		case 'A' : code = 0x77; break;
		case 'B' : code = 0x7c; break;
		case 'C' : code = 0x39; break;
		case 'D' : code = 0x5e; break;						
		case 'E' : code = 0x79; break;
		case 'F' : code = 0x71; break;
		case 'G' : code = 0x3d; break;
		case 'H' : code = 0x76; break;
		case 'I' : code = 0x30; break;
		case 'J' : code = 0x1e; break;
		case 'K' : code = 0x70; break;
		case 'L' : code = 0x38; break;
		case 'M' : code = 0xd4; break;
		case 'N' : code = 0x54; break;
		case 'O' : code = 0x5c; break;
		case 'P' : code = 0x73; break;
		case 'Q' : code = 0x67; break;
		case 'R' : code = 0x50; break;
		case 'S' : code = 0x6d; break;
		case 'T' : code = 0x07; break;
		case 'U' : code = 0x32; break;
		case 'V' : code = 0x1c; break;
		case 'W' : code = 0x9c; break;
		case 'X' : code = 0x64; break;
		case 'Y' : code = 0x6e; break;
		case 'Z' : code = 0x49; break;
		case ' ' : code = 0x00; break;
		default : code = 0; break;
	}
	return code;
}						
				
ssize_t segment_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what)
{

	unsigned char data[6];
	unsigned char digit[6]={0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
	unsigned int i,j,num,ret,k;
	unsigned char key[6];
	unsigned int count=0,temp1,temp2;

	ret = copy_from_user(&key,gdata,6);
	if (ret < 0) return -1;
	count = 100000;
	while(count>0) {
		data[5]=Getsegmentcode(key[5]);
		data[4]=Getsegmentcode(key[4]);
		data[3]=Getsegmentcode(key[3]);
		data[2]=Getsegmentcode(key[2]);
		data[1]=Getsegmentcode(key[1]);
		data[0]=Getsegmentcode(key[0]);
		for(j=0;j<100;j++) {
			for(i=0;i<6;i++) { 
				*segment_grid = ~digit[i];
				*segment_data = data[i];
				for(k=0;k<65536;k++);
			}
		}
		count--;
	}
	return 1;
}
			
struct file_operations segment_fops = 
{
	.owner		= THIS_MODULE,
	.open		= segment_open,
	.write		= segment_write,
	.release	= segment_release,

};

int segment_init(void)
{
	int result;

	result = register_chrdev(SEGMENT_MAJOR_NUMBER, SEGMENT_NAME, &segment_fops);
	if (result < 0) {
		printk(KERN_WARNING"Can't get any major\n");
		return result;
	}

	segment_major = result;
	printk("init module, 7segment major number : %d\n", result);
	return 0;
}
		
	
void segment_exit(void)
{
	unregister_chrdev(segment_major,SEGMENT_NAME);

}

module_init(segment_init);
module_exit(segment_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");



