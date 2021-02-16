/* ready.c - ready */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

/*------------------------------------------------------------------------
 * ready  --  make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
int ready(int pid, int resch)
{
	
	//kprintf("\nStarting ready.c----------\n");

	register struct	pentry	*pptr;

	if (isbadpid(pid))
		return(SYSERR);
	pptr = &proctab[pid];
	pptr->pstate = PRREADY;
	insert(pid,rdyhead,pptr->pprio);
	
	/*kprintf("\nAfter insertion into ready queue\n");
	struct qent *ptr1;
	ptr1 = &q[];
	while(ptr1->qprev != EMPTY)
	{
		kprintf("\nKey: %d\n",ptr1->qkey);
		ptr1 = ptr1->qprev;
	}*/
	
	if (resch)
	{
		//kprintf("\nResched called from ready\n");
		resched();
	}
	return(OK);
}
