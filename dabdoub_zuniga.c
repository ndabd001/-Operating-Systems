#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/init.h>


/*
* Search each leaf first.
*/
void depthFirstSearch(struct task_struct *task)
{
	struct task_struct *taskChild;
	struct list_head *taskList;

	/*
	* This prints out child process ID, parent process id and state.
	*/
	printk("Name: %s, pid: [%d], state %li parent pid: [%d]\n", task->comm, task->pid, task->state, task->parent->pid);
	list_for_each(taskList, &task->children)
	{
		taskChild = list_entry(taskList, struct task_struct, sibling);
		depthFirstSearch(taskChild);
	}
}

asmlinkage long sys_dabdoub_zuniga(int pantherid)
{
	printk("Displaying dump of process information, author: %d\n", pantherid);

	depthFirstSearch(&init_task);

	printk("Done printing task information, author: %d\n", pantherid);
	
	return 0;
}
