PA1 - Implemented two new scheduling policies, viz., Random Scheduler and Linux-like Scheduler in XINU OS to tackle starvation problem which is prevalent in traditional priority based scheduler policy in XINU.

### 2) Random based Scheduler (based loosely on the Linux kernel 2.2)

This scheduling algorithm is based on priority of processes.

Example, 
Processes with their priorities are listed:

P1: 20, P2: 30, P3: 10
Generate random number between 0 - (20+30+10). For example, the random number is 19. If 19 <= P2, select P2 for running. Otherwise select the next process. 

### 2) Linux-like Scheduler (based loosely on the Linux kernel 2.2)

This scheduling algorithm loosely emulates the Linux scheduler in the 2.2 kernel. We consider all the processes "conventional processes" and use the policies of the SCHED\_OTHER scheduling class within the 2.2 kernel. In this algorithm, the scheduler divides CPU time into continuous _epochs_. In each epoch, every existing process is allowed to execute up to a given _time quantum_, which specifies the maximum allowed time for a process within the current epoch. The time quantum for a process is computed and refreshed at the beginning of each epoch. If a process has used up its quantum during an epoch, it cannot be scheduled until another epoch starts. For example, a time quantum of 10 allows a process to only execute for 10 ticks (10 timer interrupts) within an epoch. On the other hand, a process can be scheduled many times as long as it has not used up its time quantum. For example, a process may invoke sleep before using up its quantum, but still be scheduled after waking up within an epoch. The scheduler ends an epoch and starts a new one when all the **runnable** processes (i.e., processes in the ready queue) have used up their time.

The rules for computing the time quantum for a process are as follows. At the beginning of a new epoch, the scheduler computes the time quantum for **every** existing processes, including the blocked ones. As a result, a blocked process can start in the new epoch when it becomes runnable. For a process that has never executed or used up its time quantum in the previous epoch, its new quantum value is set to its process priority (i.e., quantum = priority). For a process that has not used up its quantum in the previous epoch, the scheduler allows half of the unused quantum to be carried over to the new epoch. Suppose for each process there is a variable counter describing how many ticks are left from its previous quantum, then the new quantum value is set to floor(counter / 2) + priority. For example, a counter of 5 and a priority of 10 produce a new quantum value of 12. Any new processes created in the middle of an epoch have to wait until the next epoch to be scheduled. Any priority changes in the middle of an epoch, e.g., through create() and chprio(), only take effect in the next epoch.

To schedule processes during each epoch, the scheduler introduces a _goodness value_ for each process. This goodness value is essentially a dynamic priority that is updated whenever the scheduler is invoked (**hint**: consider when the scheduler could be invoked). For a process that has used up its quantum, its goodness value is 0. For a runnable process, the scheduler considers both the priority and the remaining quantum (recorded by counter) in computing the goodness value: goodness = counter + priority (**hint**: when you implement this, you need to consider the rule that any priority changes only take effect in the next epoch). The scheduler always schedules a runnable process that has the highest _goodness_ value (it uses the round-robin strategy if there are processes with the same goodness value). The scheduled process will keep running without being preempted (**hint**: look into clkint.S, also consider how to handle the NULL process) until it yields or uses up its time quantum.

Examples of how processes should be scheduled under this scheduler are as follows:  
If there are processes P1, P2, P3 with priority 10, 20, 15, the initial time quantum for each process would be P1 = 10, P2 = 20, and P3 = 15, so the maximum CPU time for an epoch would be 10 + 20 + 15 = 45. The possible schedule for two epochs would be P2, P3, P1, P2, P3, P1, but not: P2, P3, P2, P1, P3, P1.  
If P1 yields in the middle of its execution (e.g., by invoking sleep) in the first epoch, the time quantum for each process in the second epoch could be P1 = 12, P2 = 20, and P3 = 15, thereby the maximum CPU time would be 12 + 20 + 15 = 47.


### 3) Other Implementation Details

1\. void setschedclass (int sched\_class)  
      This function should change the scheduling type to either RANDOMSCHED or LINUXSCHED.  
  
2\. int getschedclass()  
      This function should return the scheduling class, which should be either RANDOMSCHED or LINUXSCHED.  
  
3\. Each of the scheduling class should be defined as a constant:  
      #define RANDOMSCHED 1  
      #define LINUXSCHED 2  
  
4\. Some of the source files of interest are: create.c, resched.c, resume.c, suspend.c, ready.c, proc.h, kernel.h, etc.  
  

**For Linux-like Scheduler:**  
Three processes A,B and C are created with priorities 5, 50 and 90. In each process, we print a character ('A', 'B', or 'C') at a certain time interval for LOOP times (LOOP = 50). The main process also prints a character ('M') at the same time interval. You are expected to get similar results as follows:  
  
MCCCCCCCCCCCCCBBBBBBBMMMACCCCCCCCCCCCCBB  
BBBBBMMMACCCCCCCCCCCCBBBBBBBMMMACCCCCCCC  
CCCCBBBBBBBMMBBBBBBBMMMABBBBBBBMMMABBBBB  
BBMMMBMMAMMMAMMMMMAMMMAMMMAMMMMMMAMMAMMM  
MMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA  