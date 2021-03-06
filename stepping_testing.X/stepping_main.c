#include <p24f16KA301.h>

_FOSCSEL (FNOSC_FRCDIV);   //8mHz with Post-scaling

#include "C:\Users\David\Documents\GitHub\Mechatronics-Class-Competition\competition.X\board.h" //set up pin names, hold all additional functions

int STATE = 5;

int main()
{
    pin_config_init();
    timing_interrupt_config();
    
    //Wait to move hand from switch
        delay(1000);
        
    while(1)
    {
      
          
        switch(STATE)
       {
            case 0:
                if(go_straight_inches(10))
                {
                    delay(2500);
                    STATE = 1;
                }
                break;
           case 1:
                if(turn_degrees(90))
                {
                    delay(2500);
                    STATE = 2;
                }
                break;
            case 2:
                if(turn_degrees(-90))
                {
                    delay(2500);
                    STATE = 3;
                }
                break;
            case 3:
                if(go_straight_inches(-10))
                {
                    delay(2500);
                    STATE = 0;
                }
                break;
            case 4:
                if(loading_timer(4000))
                {
                    STATE = 5;
                }
                break;
            case 5:
                if(shoot(7))
                {
                    STATE = 5;
                    delay(5000);
                }
                break;
       }
                 
    }
}
