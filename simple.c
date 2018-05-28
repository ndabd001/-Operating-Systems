/*
##########################################################
## COP4610 – Principles of Operating Systems – Summer C 2017
## Prof. Jose F. Osorio
## Student: Nicolas Dabdoub - 4531579
##
## Assignment #: 1
## Specs: Creating Linux Kernel Modules
## Due Date: 05/24/2017 by 11:55pm
##
## Module Name: Birthday Module
##
## We Certify that this program code has been written by us
## and no part of it has been taken from any sources.
##########################################################
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>

/* Building the birthday structure */
typedef struct _birthday{
	int day;
	int month;
	int year;
	struct list_head list;
}birthday;

/* Initialize head of list */
LIST_HEAD(birthday_list);

/*initialize variables for future use */
int i = 0;
birthday *person;
birthday *ptr;

/* This function is called when the module is loaded. */
int simple_init(void)
{
        printk(KERN_INFO "Loading Module\n");

	/* Generates 5 different birthdays */
	for(i = 0; i < 5; i++)
	{
		person = kmalloc(sizeof(*person), GFP_KERNEL);
		person->day = i+1;
		person->month = i+2;
		person->year = i+3;
		/* Initialize the list and add this strcut to tail */
		INIT_LIST_HEAD(&person->list);
		list_add_tail(&person->list, &birthday_list);
	}

	printk(KERN_INFO "Birthday List:\n");

	/* Iterate through list and print */
	list_for_each_entry(ptr, &birthday_list, list)
	{
		printk(KERN_INFO "day: %d, month: %d, year: %d\n", ptr->day, ptr->month, ptr->year);
	}       

	printk(KERN_INFO "List complete\n");
	
        return 0;
}

/* Variables for this function */
birthday *next;

/* This function is called when the module is removed. */
void simple_exit(void) {
	printk(KERN_INFO "Removing Module\n");

	/* Free memory that was mapped */
	list_for_each_entry_safe(ptr, next, &birthday_list, list)
	{
		printk(KERN_INFO "Removing - day: %d, month: %d, year: %d\n", ptr->day, ptr->month, ptr->year);
		list_del(&ptr->list);
		kfree(ptr);
	}
	
	printk(KERN_INFO "Memory has been freed\n");

}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Birthday Module");
MODULE_AUTHOR("ND");


