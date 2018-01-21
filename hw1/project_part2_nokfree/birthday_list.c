#include <linux/init.h>
#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>

struct birthday {
  int day;
  int month;
  int year;
  struct list_head list;
};

static LIST_HEAD(birthday_list);
int simple_init(void)
{

  int i = 0;
  struct birthday *ptr;

  printk(KERN_INFO "Loading Module\n");

  for(i = 0; i < 50000; i++)
  {
    struct birthday *person;
    person = kmalloc(sizeof(*person), GFP_KERNEL);
    person->day = i;
    person->month = i;
    person->year = 2018;
    printk(KERN_INFO "Added person with day %d, month %d, year %d\n",i,i,2018);
    INIT_LIST_HEAD(&person->list);
  }

  // Now traverse the list

  list_for_each_entry(ptr, &birthday_list, list)
  {
    printk(KERN_INFO "Traversed entry with day %d, month %d, year %d",ptr->day,ptr->month, ptr->year);
  }

  return 0;
}

void simple_exit(void) {
  struct birthday *ptr, *next;
  printk(KERN_INFO "Removing Module after freeing memory\n");
  list_for_each_entry_safe(ptr, next, &birthday_list, list)
  {
    // list_del(&ptr->list);
    // kfree(ptr);
  }

}

module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HW1");
MODULE_AUTHOR("Joe");
