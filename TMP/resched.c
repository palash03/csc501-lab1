/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include "LAB1.h"

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

int maxGoodness(int goodness)
{
	int choosePid = 0;
	int temp = q[rdyhead].qnext;
	while(q[temp].qkey != MAXINT)
	{
		struct pentry *p = &proctab[temp];
		if(goodness < p->good && p->check == 1)
		{
			choosePid = temp;
		}
		temp = q[temp].qnext;
	}
	return choosePid;
}

int resched()
{
	if(getschedclass() == 1)
	{
		extern int ctr1000; 
		register struct	pentry	*optr;	/* pointer to old process entry */
		register struct	pentry	*nptr;	/* pointer to new process entry */
		int i,r;
		extern int checkFlag;
				
		optr = &proctab[currpid];
		if(optr->pstate == PRCURR)
		{
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);	
		}
		
		int sum = 0;
		int a = q[(rdyhead)].qnext;
		while(q[a].qkey != MAXINT)
		{
			sum = sum + q[a].qkey;
			a = q[a].qnext;
		}
		
		if(sum != 0)
		{
			srand(ctr1000);
			r = rand()%sum;
		}
		else
		{
			r = -1;
		}
		
		int temp = q[(rdytail)].qprev;
		
		while(q[temp].qkey != MININT)
		{
			if(r < q[temp].qkey)		
			{
				break;
			}
			else
			{
				r = r - q[temp].qkey;
			}
			temp = q[temp].qprev;
		}
		dequeue(temp);
		nptr = &proctab[temp];
		currpid = temp;
		nptr->pstate = PRCURR;	
		#ifdef  RTCLOCK
			preempt = QUANTUM;
		#endif
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);	
		return OK;
	}

	//Linux Scheduler
	if(getschedclass() == 2)
	{
		register struct	pentry	*optr;	/* pointer to old process entry */
		register struct	pentry	*nptr;	/* pointer to new process entry */
		int findMaxGoodness = 0,pidGood = 0;
		int temp,epoch = 0;
		
		optr = &proctab[currpid];
		preempt = max(0,preempt);
			
		if(optr->count == 0)
		{
			optr->good = 0;
		}
		else
		{
			optr->good = optr->good - (optr->count - preempt);
		}
		optr->count = preempt;
		
		if(optr->pstate == PRCURR)
		{
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		pidGood = maxGoodness(findMaxGoodness);	//Find the process with the maximum goodness
		nptr = &proctab[pidGood];
		findMaxGoodness = nptr->good;
		
		if(findMaxGoodness == 0)
		{
			int i;
			for(i=1;i<NPROC;i++)
			{
				struct pentry *p = &proctab[i];
				if(p->pstate != PRFREE)
				{
					p->good = p->count + p->pprio;
					p->count = (p->count / 2) + p->pprio;
					p->check = 1;
				}
			}	
			pidGood = maxGoodness(0);
			nptr = &proctab[pidGood];
			currpid = pidGood;
			nptr->pstate = PRCURR;
			dequeue(pidGood);			
			#ifdef RTCLOCK
				preempt = nptr->count;
			#endif     
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
			return OK;
		}
		else
		{
			nptr = &proctab[pidGood];
			currpid = pidGood;
			nptr->pstate = PRCURR;
			dequeue(pidGood);			
			#ifdef	RTCLOCK
				preempt = nptr->count;
			#endif     
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
			return OK;	
		}		
		nptr = &proctab[NULLPROC];	
		currpid = NULLPROC;
		nptr->pstate = PRCURR;
		dequeue(NULLPROC);			
		#ifdef	RTCLOCK
			preempt = QUANTUM;	
		#endif
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
		return OK;
	}
	else
	{
		register struct	pentry	*optr;	/* pointer to old process entry */
		register struct	pentry	*nptr;	/* pointer to new process entry */

		/* no switch needed if current process priority higher than next*/
		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) && (lastkey(rdytail)<optr->pprio)) 
		{
			return(OK);
		}
		
		/* force context switch */
		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		/* remove highest priority process at end of ready list */
		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
		#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
		#endif
		
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);	
	}
	return OK;
}
