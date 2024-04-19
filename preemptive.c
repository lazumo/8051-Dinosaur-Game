#include <8051.h>

#include "preemptive.h"

__data __at (0x2E) char counter;
__data __at (0x30) char savedSP[4];
__data __at (0x34) char thread_Bitmap;
__data __at (0x35) char cur_Thread;
__data __at (0x36) char new_Thread;
__data __at (0x37) char tmp_SP;

/*
 * @@@ [8 pts]
 * define a macro for saving the context of the current thread by
 * 1) push ACC, B register, Data pointer registers (DPL, DPH), PSW
 * 2) save SP into the saved Stack Pointers array
 *   as indexed by the current thread ID.
 * Note that 1) should be written in assembly, 
 *     while 2) can be written in either assembly or C
 */
#define SAVESTATE \
        { __asm \
            PUSH ACC\
            PUSH B\
            PUSH DPL\
            PUSH DPH\
            PUSH PSW\
         __endasm; \
         savedSP[cur_Thread] = SP;\
        }

/*
 * @@@ [8 pts]
 * define a macro for restoring the context of the current thread by
 * essentially doing the reverse of SAVESTATE:
 * 1) assign SP to the saved SP from the saved stack pointer array
 * 2) pop the registers PSW, data pointer registers, B reg, and ACC
 * Again, popping must be done in assembly but restoring SP can be
 * done in either C or assembly.
 */
#define RESTORESTATE \
         {SP = savedSP[cur_Thread];\
         __asm \
            POP PSW\
            POP DPH\
            POP DPL\
            POP B\
            POP ACC\
          __endasm; \
         }


 /* 
  * we declare main() as an extern so we can reference its symbol
  * when creating a thread for it.
  */

extern void main(void);

/*
 * Bootstrap is jumped to by the startup code to make the thread for
 * main, and restore its context so the thread can run.
 */

void Bootstrap(void) {
      /*
       * @@@ [2 pts] 
       * initialize data structures for threads (e.g., mask)
       *
       * optional: move the stack pointer to some known location
       * only during bootstrapping. by default, SP is 0x07.
       *
       * @@@ [2 pts]
       *     create a thread for main; be sure current thread is
       *     set to this thread ID, and restore its context,
       *     so that it starts running main().
       */
      
      TMOD = 0;  
      IE = 0x82; 
      TR0 = 1; 
      thread_Bitmap = 0;
      cur_Thread = ThreadCreate( main );
      RESTORESTATE;
}

void myTimer0Handler(){
      EA = 0;
      SAVESTATE;
      //Error: <a> machine specific addressing or addressing mode error
    __asm
        MOV A, R0
        PUSH ACC
        MOV A, R1
        PUSH ACC
        MOV A, R2
        PUSH ACC
        MOV A, R3
        PUSH ACC
        MOV A, R4
        PUSH ACC
        MOV A, R5
        PUSH ACC
        MOV A, R6
        PUSH ACC
        MOV A, R7
        PUSH ACC
    __endasm;
      
       do {

            if(cur_Thread==3) cur_Thread=0;
            else cur_Thread++;
            if((1<<cur_Thread) & thread_Bitmap)
               break;
        } while (1);
      __asm
         POP ACC
         MOV R7, A
         POP ACC
         MOV R6, A
         POP ACC
         MOV R5, A
         POP ACC
         MOV R4, A
         POP ACC
         MOV R3, A
         POP ACC
         MOV R2, A
         POP ACC
         MOV R1, A
         POP ACC
         MOV R0, A
      __endasm;  
      RESTORESTATE;
      counter++;
      EA = 1;
      __asm 
         reti
      __endasm;
       
}

/*
 * ThreadCreate() creates a thread data structure so it is ready
 * to be restored (context switched in).
 * The function pointer itself should take no argument and should
 * return no argument.
 */
ThreadID ThreadCreate(FunctionPtr fp) {
      
      __critical{
         if( thread_Bitmap == 15 ) {new_Thread = -1;}
         else{
            if(!(thread_Bitmap & 1)) new_Thread = 0;
            else{
               if(!(thread_Bitmap & 2)) new_Thread = 1;
               else{
                  if(!(thread_Bitmap & 4)) new_Thread = 2;
                  else new_Thread = 3;
               }
            }
            thread_Bitmap |= 1 << new_Thread;
         
            //bc
            tmp_SP = SP;
            SP = 0x10 * new_Thread + 0x3F ;
            //d
            //DPTR = (unsigned int)fp; 2-byte ptr is passed in DPTR  no need
            //remember two__
            __asm
               PUSH DPL
               PUSH DPH
            __endasm;
            //e
            __asm 
               MOV A, #0
               PUSH ACC
               PUSH ACC
               PUSH ACC
               PUSH ACC
            __endasm;
            //f
            PSW = new_Thread << 3;
            __asm 
               PUSH PSW
            __endasm;
            //g
            savedSP[new_Thread]=SP;
            SP = tmp_SP;
            }
         
      }
      return new_Thread;
 
}



/*
 * this is called by a running thread to yield control to another
 * thread.  ThreadYield() saves the context of the current
 * running thread, picks another thread (and set the current thread
 * ID to it), if any, and then restores its state.
 */

void ThreadYield(void) {
   __critical{
       SAVESTATE;
       do {
                 if(cur_Thread==3) cur_Thread=0;
                 else cur_Thread++;
                 if((1<<cur_Thread) & thread_Bitmap)
                     break;
        } while (1);
        RESTORESTATE;
      }
}


/*
 * ThreadExit() is called by the thread's own code to terminate
 * itself.  It will never return; instead, it switches context
 * to another thread.
 */
void ThreadExit(void) {
   __critical{
      thread_Bitmap ^= (1  <<  cur_Thread);

      if(thread_Bitmap){
         if((thread_Bitmap & 1)) cur_Thread = 0;
         else{
            if((thread_Bitmap & 2)) cur_Thread = 1;
            else{
               if((thread_Bitmap & 4)) cur_Thread = 2;
               else cur_Thread = 3;
            }
         }
      }
      else cur_Thread = -1;


        RESTORESTATE;
   }
}
