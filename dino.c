
#include <8051.h>
#include "preemptive.h"
#include "keylib.h"
#include "buttonlib.h"
#include "lcdlib.h"

#define INIT 0
#define GAME 1
#define OVER 2


#define DINO '\1'
#define CACTUS '\2'
//__data __at (0x20) unsigned char lcd_ready;
__data __at (0x21) unsigned char state;
__data __at (0x22) unsigned char key_full;
__data __at (0x23) unsigned char dinosaur_row;
__data __at (0x24) unsigned char local_dinosaur_row;
__data __at (0x25) unsigned  int map[2];
//map(0x26)
//map(0x27)
//map(0x28)
__data __at (0x29) unsigned  int localmap[2];
//map(0x2A)
//map(0x2B)
//map(0x2C)
__data __at (0x2D) char game_empty;
__data __at (0x2E) char counter;
__data __at (0x2F) signed char i;
/*
__data __at (0x30) char savedSP[4];
__data __at (0x34) char thread_Bitmap;
__data __at (0x35) char cur_Thread;
__data __at (0x36) char new_Thread;
__data __at (0x37) char tmp_SP;
*/
__data __at (0x38) char score;
__data __at (0x39) char level;
__data __at (0x3A) char buffer;
__data __at (0x3B) char Number;
__data __at (0x3C) char key_empty;
__data __at (0x3D) char game_full;
__data __at (0x3E) char mutex;
__data __at (0x3F) char lastkey;

//__data __at (0x70) unsigned char test;


#define L(x) LABEL(x)
#define LABEL(x) x##$




void keypad_ctrl(void) {

    Init_Keypad();
    lastkey=0;
    Number = 0;
    buffer = 0;

    while(1) {
        EA = 0;
        Number = KeyToChar();  
        EA = 1;  
        if (Number != '\0' && lastkey == '\0') {
            SemaphoreWait(key_empty);
            SemaphoreWait(mutex);
                buffer = Number; 
            SemaphoreSignal(mutex);  
            SemaphoreSignal(key_full);   

        }
        lastkey = Number;  

        
    }
}

void game_ctrl(void) {

    state  =  0;
    dinosaur_row = 1;
    score = 0;
    level = 0;
    counter = 0;
    map[0]=  0x0000 ;
    map[1] =  0x0000 ;

    while (1) {
    
        if(state == INIT){

            SemaphoreWait(key_full);
            SemaphoreWait(game_empty);
            SemaphoreWait(mutex);

                if(buffer == '#') state = GAME ; 
                else if(buffer <='9' && buffer >='0') level  = buffer - '0';
            
            SemaphoreSignal(mutex);
            SemaphoreSignal(game_full);  
            SemaphoreSignal(key_empty);

        }
        else if(state == GAME){ 
            if(key_full){
                SemaphoreWait(key_full);
                SemaphoreWait(mutex);
                    if(buffer=='2') dinosaur_row =  0;
                    else if(buffer=='8') dinosaur_row =  1;
                    else;
                SemaphoreSignal(mutex);
                SemaphoreSignal(key_empty);
            }
            SemaphoreWait(game_empty);
            SemaphoreWait(mutex);
                if(counter >= (63 - level*7)){
                    if((map[0] %2) | ((map[1] %2)<<1)){
                        map[0] = map[0] << 1;
                        map[1] = map[1] << 1; 
                    }
                    else{
                        map[0] = map[0] << 1;
                        map[1] = map[1] << 1;
                        if((map[0]<<13)>>13  != 0 ||  (map[1]<<13)>>13  != 0 ){;}
                        else if( (TL0)%3 ==2 )  map[0] += 1;
                        else if( (TL0)%3 ==1 ) map[1] += 1;
                    }
                    counter = 0;
                    if(dinosaur_row){
                        if( map[1] >> 15 ) state = OVER;
                        else if(map[0] >> 15 ) score++;
                        else;
                    }
                    else{
                        if( map[0] >> 15 ) state = OVER;
                        else if(map[1] >> 15 ) score++;
                        else;
                    }
                }
                else{
                    if(dinosaur_row){
                        if( map[1] >> 15 ) state = OVER;
                    }
                    else{
                        if( map[0] >> 15 ) state = OVER;
                    }
                }
            SemaphoreSignal(mutex);
            SemaphoreSignal(game_full);
        }
        else;
        //ThreadYield();
    }
}


void render_task(void) {
    localmap[0]= map[0];
    localmap[1]= map[1];
    local_dinosaur_row = dinosaur_row;
    LCD_Init(); 
    LCD_set_symbol1(); // bitmap for dinosaur starts at 0x10
    LCD_set_symbol2();   // bitmap for cactus starts at 0x20
    LCD_clearScreen(); 
    
    
    while (1) {

        if(state == INIT  ){
                SemaphoreWait(game_full);
                SemaphoreWait(mutex);
                    local_dinosaur_row = dinosaur_row;
                SemaphoreSignal(mutex);
                SemaphoreSignal(game_empty);
                    EA = 0;
                    LCD_cursorGoTo(local_dinosaur_row,0);
                    LCD_write_char('\1'); 
                    EA = 1;

        }
        else if(state==GAME){
                SemaphoreWait(game_full);
                SemaphoreWait(mutex);
                    local_dinosaur_row = dinosaur_row;
                    localmap[0]= map[0];
                    localmap[1]= map[1];
                SemaphoreSignal(mutex);
                SemaphoreSignal(game_empty);
            EA = 0;
            LCD_cursorGoTo(local_dinosaur_row ,0); 
            LCD_write_char('\1');  
            EA = 1;
            for(i=14 + local_dinosaur_row  ;i>=1;i--){
            if( (localmap[0]  << 15-i) >> 15 ){
                EA = 0;
                LCD_cursorGoTo(0,15-i);
                LCD_write_char('\2');
                EA = 1;
            }
            else{
                EA = 0;
                LCD_cursorGoTo(0,15-i);
                LCD_write_char(' ');    
                EA = 1;    
            }
            }

            for(i=14 + 1- local_dinosaur_row;i>=1;i--){
                if( (localmap[1]  << 15-i) >> 15 ){
                    EA = 0;
                    LCD_cursorGoTo(1,15-i);
                    LCD_write_char('\2');
                    EA = 1;
                }
                else{
                    EA = 0;
                    LCD_cursorGoTo(1,15-i);
                    LCD_write_char(' ');
                    EA = 1;
                }
            }
        }
        else if(state==OVER){
            EA = 0;
            LCD_cursorGoTo(0,0);
            LCD_write_char('G');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,1);
            LCD_write_char('A');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,2);
            LCD_write_char('M');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,3);
            LCD_write_char('E');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,4);
            LCD_write_char('O');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,5);
            LCD_write_char('V');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,6);
            LCD_write_char('E');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,7);
            LCD_write_char('R');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,8);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,9);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,10);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,11);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,12);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,13);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(0,14);
            LCD_write_char(' ');
            EA = 1;

            if(score == 0){
            
            EA = 0;
                LCD_cursorGoTo(1,0);
                LCD_write_char('0');
            EA = 1;
            EA = 0;
                LCD_cursorGoTo(1,1);
                LCD_write_char(' ');
            EA = 1;
            }
            else{
            
            EA = 0;
                LCD_cursorGoTo(1,0);
                LCD_write_char(score/10 +  '0');
            EA = 1;
            EA = 0;

                LCD_cursorGoTo(1,1);
                LCD_write_char(score%10 +  '0');
            EA = 1;     
            }
            EA = 0;
            LCD_cursorGoTo(1,2);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(1,3);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(1,4);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(1,5);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(1,6);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(1,7);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(1,8);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(1,9);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(1,10);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(1,11);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(1,12);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(1,13);
            LCD_write_char(' ');
            EA = 1;
            EA = 0;
            LCD_cursorGoTo(1,14);
            LCD_write_char(' ');
            EA = 1;
            
        }
        else;
        //ThreadYield();
    }
    
}


/* [5 pts for this function]
 * main() is started by the thread bootstrapper as thread-0.
 * It can create more thread(s) as needed:
 * one thread can act as producer and another as consumer.
 */
void main(void) {
        
        SemaphoreCreate(mutex, 1);
        SemaphoreCreate(key_empty,0);
        SemaphoreCreate(key_full,1);
        SemaphoreCreate(game_empty,0);
        SemaphoreCreate(game_full,1);

        ThreadCreate( keypad_ctrl );
        ThreadCreate( game_ctrl );
        render_task(); 
}

void _sdcc_gsinit_startup(void) {
        __asm
                ljmp  _Bootstrap
        __endasm;
}

void _mcs51_genRAMCLEAR(void) {}
void _mcs51_genXINIT(void) {}
void _mcs51_genXRAMCLEAR(void) {}
void timer0_ISR(void) __interrupt(1) {
        __asm
            ljmp _myTimer0Handler
        __endasm;
        
}