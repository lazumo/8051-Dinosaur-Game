#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS 4  /* not including the scheduler */
/* the scheduler does not take up a thread of its own */

typedef char ThreadID;
typedef void (*FunctionPtr)(void);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadExit(void);
void myTimer0Handler();

// semaphore
#define CNAME(s) _ ## s

// create a counting semaphore s that is initialized to n
#define SemaphoreCreate(s, n)\
            s = n\

// do (busy-)wait() on semaphore s
//JZ  label semaphore =0
//JB ACC.7, label label semaphore <0
#define SemaphoreWaitBody(s, label)\
    { __asm \
        label:\
        MOV A, CNAME(s)\
        JZ  label\
        JB ACC.7, label\
        dec  CNAME(s) \
      __endasm; }
#define SemaphoreWait(s)\
    SemaphoreWaitBody(s, L(__COUNTER__) );
// signal() semaphore s
#define SemaphoreSignal(s)\
            __asm \
            INC CNAME(s)\
            __endasm;\

#endif 