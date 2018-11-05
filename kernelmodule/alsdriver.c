#include <linux/fs.h>
#include <linux/pci.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/uaccess.h>

#define brightnessFile "/sys/class/backlight/intel_backlight/brightness"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NIRMAL K V");
MODULE_DESCRIPTION("Adaptive Brightess Driver");


struct input_dev *alsInDevice;
static struct platform_device *alsDevice; // Device structure
struct file *brightnessfileptr;

// File operation functions from kernel space (Not Recommended!!) 
struct file* openFileKern(const char* path, int flags, int rights);
void closeFileKern(struct file* file);
int writeFileKern(struct file* file, unsigned long long offset, const char* data, unsigned int size);

// Sysfs funtion to sensor values 
static ssize_t writeBrightness(struct device *dev, struct device_attribute *attr, const char *buffer, size_t count)
{
    int sensorVal,actualBrightness,minBrightness,maxBrightness,change,abschange;
    char brightnessVal[4];
    
    // copiying input data to variables 
    sscanf(buffer, "%d %d %d %d", &sensorVal, &minBrightness, &maxBrightness, &actualBrightness);

    change = sensorVal - actualBrightness;
    abschange = (change > 0)? change:(-change);
    if(abschange > 20){
        brightnessfileptr = openFileKern(brightnessFile, 1, 0);
        if((actualBrightness + change) >= maxBrightness){
            sprintf(brightnessVal,"%d",maxBrightness);
            writeFileKern((struct file*)brightnessfileptr, 0, brightnessVal, 3);
        }
        else if ((actualBrightness + change) <= minBrightness)
        {
            sprintf(brightnessVal,"%d",minBrightness);
            writeFileKern((struct file*)brightnessfileptr, 0, brightnessVal, 3);
        }
        else{
            sprintf(brightnessVal,"%d",(change + actualBrightness));
            writeFileKern((struct file*)brightnessfileptr, 0, brightnessVal, 3);
        }
        closeFileKern((struct file*)brightnessfileptr);       
    }
    input_sync(alsInDevice);
    return 0;
}

// Attach the sysfs write method 
DEVICE_ATTR(alsevent, 0644, NULL, writeBrightness);

// Attribute Descriptor 
static struct attribute *alsAttributes[] = {
    &dev_attr_alsevent.attr,
    NULL
};

// Attribute group 
static struct attribute_group alsAttributesGrp = {
    .attrs = alsAttributes,
};

// Driver Initializing 
int __init als_init(void)
{
    // Register a platform device 
    alsDevice = platform_device_register_simple("virmouse", -1, NULL, 0);
    if (IS_ERR(alsDevice)){
        printk ("als_init: error\n");
        return PTR_ERR(alsDevice);
    }
    
    // Create a sysfs node to read simulated coordinates 
    sysfs_create_group(&alsDevice->dev.kobj, &alsAttributesGrp);
    
    // Allocate an input device data structure 
    alsInDevice = input_allocate_device();
    if (!alsInDevice) {
        printk("Bad input_allocate_device()\n");
        return -ENOMEM;
    }
	
    // Register with the input subsystem 
    input_register_device(alsInDevice);
    
    // print messages in the kernel log
    printk("Virtual Mouse Driver Initialized.\n");
    
    return 0;
}

// Driver Uninitializing 
void als_uninit(void)
{
    // Unregister from the input subsystem 
    input_unregister_device(alsInDevice);
    
    // Remove sysfs node
    sysfs_remove_group(&alsDevice->dev.kobj, &alsAttributesGrp);
    
    // Unregister driver
    platform_device_unregister(alsDevice);
    
    return;
}

struct file* openFileKern(const char* path, int flags, int rights)
{
	struct file* filp = NULL;
	mm_segment_t oldfs;
	int err = 0;
	oldfs = get_fs();
	set_fs(get_ds());
	filp = filp_open(path, flags, rights);
	set_fs(oldfs);
	if (IS_ERR(filp))
	{
		err = PTR_ERR(filp);
		return NULL;
	}
	return filp;
}

void closeFileKern(struct file* file)
{
	filp_close(file, NULL);
}

int writeFileKern(struct file* file, unsigned long long offset, const char* data, unsigned int size)
{
	mm_segment_t oldfs;
	int ret;
	oldfs = get_fs();
	set_fs(get_ds());
	ret = vfs_write(file, data, size, &offset);
	set_fs(oldfs);
	return ret;
}

module_init(als_init);
module_exit(als_uninit);


