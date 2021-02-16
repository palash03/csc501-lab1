/* insert.c  -  insert */

#include <conf.h>
#include <kernel.h>

#include<proc.h> //Add by me
#include <q.h>



/*------------------------------------------------------------------------
 * insert.c  --  insert an process into a q list in key order
 *------------------------------------------------------------------------
 */
int insert(int proc, int head, int key)
{
	//kprintf("\nEntering insert----------------\n");	

	int	next;			/* runs through list		*/
	int	prev;

	next = q[head].qnext;

	while (q[next].qkey < key)	/* tail has maxint as key	*/
	{
		next = q[next].qnext;
	}
	q[proc].qnext = next;
	q[proc].qprev = prev = q[next].qprev;
	q[proc].qkey  = key;
	q[prev].qnext = proc;
	q[next].qprev = proc;
	
	/*int i,j;
	kprintf("-------Inside insert.c--------\nProcess name: ");
	for(i=0;i<NPROC;i++)
        {
                struct pentry *proc = &proctab[i];
                if(proc->pprio > 0)
                {
                        for(j=0;j<strlen(proc->pname);j++)
                        {
                                kprintf("%c",proc->pname[j]);
                        }
			kprintf("%s\n",proc->pname);
                }
        }	
	kprintf("\nProcess being added has Currpid: %d, Key: %d",currpid,q[proc].qkey);
	kprintf("\nGetting out of insert\n");*/
	/*
	struct qent *ptr;
        ptr = &q[rdyhead].qnext;
        while(ptr->qkey != MAXINT)
        {
                kprintf("Priority assigned to q values %d\n",ptr->qkey); 
		ptr = ptr.qnext;
        }
        kprintf("Exit the for loop --------------\n");
	*/
	/*int sum = 0;
	int temp = q[head].qnext;
	while(q[temp].qkey != MAXINT)
	{
		sum = sum + q[temp].qkey;
		kprintf("Priority assigned to q values: %d\n",q[temp].qkey);
		temp = q[temp].qnext;
	}
	kprintf("\nSum: %d , %d running processes,  Exiting insert---------------\n",sum,numproc);
	*/
	return(OK);
}
