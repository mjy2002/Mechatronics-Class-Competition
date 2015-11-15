/* 
 * File: board.h  
 * Author: Walter Coe
 * Comments: Functions and Declarations which govern the motion for the 
 *              ME330 shooter at BYU.
 * Revision history: See Github...
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

//Values
#define LEFT_FORWARD 1
#define RIGHT_FORWARD 0

#define LEFT_BACKWARD 1
#define RIGHT_BACKWARD 0

#define STEP_DELAY 10 // Is this value arbitrary right now or is this a minimum/optimized value? - David

#define INCH_TO_WALL 24.0
#define INCH_FROM_ULTRA_TO_CENTER 4.5
#define DIST_TOL 0.1
#define STANDARD_STEP_LENGTH .09
#define STANDARD_STEP_ANGLE .05

#define LOADING_IR_FREQ 100

#define INCHES_CORNER_TO_CENTER 33.3

#define IR_FOUND_THRESH 70  //Percent of full voltage

#define INCH_PER_MIRCOSECONDS .00676

float read_dist()
{
	static unsigned long averageMicrosF;
	static unsigned long averageMicrosB;
	static unsigned long oldAverageMicrosF;
	static unsigned long oldAverageMicrosB;
	static float distance;
	
	static unsigned long sumMicrosF;
	static unsigned long sumMicrosB;
	
	sumMicrosF = 0;
	sumMicrosB = 0;
	
	for(i = 0; i <= ULTRASONIC_VALUES; i++)
	{
		sumMicrosF = sumMicrosF + ultrasonicValuesF[i];
		sumMicrosB = sumMicrosB + ultrasonicValuesB[i];
	}
	
	sumMicrosF = sumMicrosF + oldAverageMicrosF;
	sumMicrosB = sumMicrosB + oldAverageMicrosB;
	
	averageMicrosF = sumMicrosF/(ULTRASONIC_VALUES + 1);
	averageMicrosB = sumMicrosB/(ULTRASONIC_VALUES + 1);
	
	oldAverageMicrosF = averageMicrosF;
	oldAverageMicrosB = averageMicrosB;
	
	distance = (( 48.0 - ((float)averageMicrosF * INCH_PER_MIRCOSECONDS) - INCH_FROM_ULTRA_TO_CENTER ) + ( ((float)averageMicrosF * INCH_PER_MIRCOSECONDS) + INCH_FROM_ULTRA_TO_CENTER ))/2.0;
	
	return(distance);
}


float ir_front_percent()
{
    //return values between 0-100 for percent of IR seen
    return(-0.0298*ADC1BUF0 + 122.1);
}


float ir_back_percent()
{
    //return values between 0-100 for percent of IR seen
    return(-0.0298*ADC1BUF1 + 122.1);
}


int ir_front_found()
{
    if((-0.0298*ADC1BUF0 + 122.1) >= IR_FOUND_THRESH)
    {
        return(1);
    }
    else
    {
        return(0);
    }
}


int ir_back_found()
{
    if((-0.0298*ADC1BUF1 + 122.1) >= IR_FOUND_THRESH)
    {
        return(1);
    }
    else
    {
        return(0);
    }
}


float abs_f(float value)
{
	if(value >= 0)
		return(value);
	else
		return(-1.0*value);
}

int go_straight_inches(float inches)
{
    #define stepsPerRev 200 //Number of steps required for one rev of motor
    #define wheelCircumferenceInches 12.76	//Drive wheel circumference in inches
    static int stepsPerInch = (int)(stepsPerRev/wheelCircumferenceInches);	//Number of pulses required to move forward 1 inch (pulses/revolution)*(revolutions/inch)
    static int numberOfSteps;
    static int stepsTaken = 0;
    static long lastTime = 0;

    numberOfSteps = (int)(abs_f(inches)*stepsPerInch);	//convert inches to number of steps

    if(inches >= 0)
    {
        _RB7 = 0;  //set direction pins, both forward, DIR-R
        _RB8 = 1;  //set direction pins, both forward, DIR-L
    }

    else
    {
        _RB7 = 1;  //set direction pins, both backward, DIR-R
        _RB8 = 0;  //set direction pins, both backward, DIR-L
    }

    if(stepsTaken < numberOfSteps)  //Not enough steps yet...
    {
        if((milliseconds - lastTime) <= STEP_DELAY)  //Not yet waited long enough
        {
            //do nothing YET...
        }
        else    //Its time to step...
        {
            if(_RB15)   //STEP
            {
                _RB15 = 0;  //STEP
            }
            else
            {
                _RB15 = 1;  //STEP
            }
            
            if(_RB15 == 1) //STEP was transitioned HIGH
            {
                stepsTaken++;   //Count steps when pulse goes HIGH
            }
            lastTime = milliseconds;
        }
        return(0);  //Need to continue
    }
    else    //We've taken enough steps
    {
        stepsTaken = 0; //Reset steps taken for next call
        return(1);  //We're done...
    }
}


int turn_degrees(int degrees)
{
    static int stepsPerDegree = 1.055; //Number of pulses required to spin 1 degrees
    static int numberOfSteps;   //Number of steps to reach the desired degrees
    numberOfSteps = (int)(abs_f(degrees) * stepsPerDegree); // This rounds to lower integer, right? - David
    static int stepsTaken = 0;
    static long lastTime = 0;


    if(degrees >= 0)    //Turn CW
    {
        _RB7 = 1;  //set DIR-R
        _RB8 = 1;  //set DIR-L
    }
    else
    {
        _RB7 = 0;  //set DIR-R
        _RB8 = 0;  //set DIR-L
    }

    if(stepsTaken < numberOfSteps)  //Not enough steps yet...
    {
        if((milliseconds - lastTime) <= STEP_DELAY)  //Not yet waited long enough
        {
            //do nothing YET...
        }
        else    //Its time to step...
        {
            if(_RB15)
            {
                _RB15 = 0;
            }
            else
            {
                _RB15 = 1;
            }
            //toggle(PIN_STEP_L); //Change the value from 1->0 or visa-versa
            if(_RB15 == 1) //Step pin was toggled HIGH
            {
                stepsTaken++;   //Count steps when pulse goes HIGH
            }
            lastTime = milliseconds;
        }
        return(0);  //Need to continue
    }
    else    //We've taken enough steps
    {
        stepsTaken = 0; //Reset steps taken for next call
        return(1);  //We're done...
    }
}


int find_normal()
{
	static int statFlag = 0;
	static int dirFlag = 0;
	static float dist1 = 33.3;	//Dist1 = Measure distance
	//TODO: Write read_dist() function. Should return float value of filtered distance measurements...
	if(dirFlag == 0)
	{
		turn_degrees(1);	//Turn 1 degree
	}
	else
	{
		turn_degrees(-1);
	}
	static float dist2;	//Dist2 = Measure distance
	dist2 = read_dist();
	
	if((dist2 > dist1) && (statFlag == 0))	//If (Dist2 > Dist1 and Flag = 0)	//Going wrong direction, first time through
	{
		if(dirFlag == 1)	//	Go the other way?
		{
			dirFlag = 0;
		}
		else
		{
			dirFlag = 1;
		}
		
		return(0);	//keep searching
	}		
	else if(dist1 > dist2)	//Going right direction
	{
		//	Continue turning in that direction
		statFlag = 0;		//	Flag = 1
		return(0);	// keep searching
	}
	else if((dist2 > dist1) && (statFlag == 1))		//Else if(Dist2 > Dist1 && Flag==1)	//Reached the minimum
	{
		if(dirFlag == 0)
		{
			turn_degrees(-1*STANDARD_STEP_ANGLE);	//	Go back half a step in angle
		}
		else
		{
			turn_degrees(STANDARD_STEP_ANGLE);	//	Go back half a step in angle
		}
		
		return(1);	//	Return(1)
		statFlag = 0;	//	Flag = 0
	}
}


int find_24()
{
	static float error;
		
	error = (INCH_TO_WALL -  read_dist());	//Error = (24 - Dist)
	//TO DO: write a read_dist() function. This should return a float which is a filtered value representing the distance from the wall
		
	if(abs_f(error) < DIST_TOL)	//We are there...
	{
		return(1);	//Center Found
	}
	
	else	//Change position...
	{
		if(error > 0)	//Positive error, move backward
		{
			go_straight_inches(-1*STANDARD_STEP_LENGTH); // If our DIST_TOL is 0.1 in, how can we reach it taking 0.5 in steps? Reduce to 0.1 in, maybe? - David
		}
		else	//Negative error, move forward
		{
			go_straight_inches(STANDARD_STEP_LENGTH);
		}
		
		return(0);	//Keep looking for center
	}
}


void ultrasonic_setup()
{
	// Configure CN interrupt on Pin2 and Pin3
    _CN6IE = 1;     // Enable CN on pin 6 (CNEN1 register)
    _CN6PUE = 0;    // Disable pull-up resistor (CNPU1 register)
    _CN30IE = 1;     // Enable CN on pin 7 (CNEN1 register)
    _CN30PUE = 0;    // Disable pull-up resistor (CNPU1 register)
    _CNIP = 4;      // Set CN interrupt priority (IPC4 register)
    _CNIF = 0;      // Clear interrupt flag (IFS1 register)
    _CNIE = 1;      // Enable CN interrupts (IEC1 register)
	
	ultraLastStateF = _RB2; //Current state of ultrasonic
	ultraLastStateB = _RA2; //Current state of ultrasonic
	
	for(i = 0; i <= ULTRASONIC_VALUES; i++)
	{
		ultrasonicValuesB[i] = 2000;	//Start with an array of 2 milliseconds times
		ultrasonicValuesF[i] = 2000;	//Start with an array of 2 milliseconds times
	}

    // CONFIGURE PWM2 (on pin 4) USING TIMER2
    // Configure Timer2, which will be the source for the output compare that
    // drives Ultrasonic_Front
    T2CONbits.TON = 0;      // Timer2 off while configuring PWM, pg. 144
    T2CONbits.TCKPS = 0b00;    // Timer2 1:1 clock pre-scale, pg. 144
    T2CONbits.T32 = 0;      // Timer2 acts as a single 16-bit timer, pg. 144
    T2CONbits.TCS = 0;      // Timer2 internal clock source, pg. 144

    // Configure PWM2 on OC1 (pin 4)										-- Is this supposed to be PWM2 on pin 4? - David
    OC2CON1 = 0b0000;               // Clear OC2 configuration bits, Table 4-8
    OC2CON2 = 0b0000;               // Clear OC2 configuration bits, Table 4-8
    OC2CON1bits.OCTSEL = 0b000;       // Use Timer2, pg. 157
    OC2CON1bits.OCM = 0b110;          // Edge-aligned PWM mode, pg. 158
    OC2CON2bits.SYNCSEL = 0b01100;    // Use Timer2, pg. 160

    // Set period and duty cycle
    PR2 = 25000;            // Period of Timer2 = 50ms
                            // See Equation 15-1 in the data sheet
    OC2R = 5;               // On time of 10us

    // Turn on Timer2
    T2CONbits.TON = 1;
	
	// CONFIGURE PWM3 (on pin 5) USING TIMER2
    // Configure Timer3, which will be the source for the output compare that
    // drives Ultrasonic_Front													
    T3CONbits.TON = 0;      	// Timer3 off while configuring PWM, pg. 144
    T3CONbits.TCKPS = 0b00;    	// Timer3 1:1 clock pre-scale, pg. 144
    //T3CONbits.T32 = 0;      	// Timer3 acts as a single 16-bit timer, pg. 144
    T3CONbits.TCS = 0;      	// Timer3 internal clock source, pg. 144

    // Configure PWM3 on OC1 (pin 5)											
    OC3CON1 = 0b0000;               	// Clear OC3 configuration bits, Table 4-8
    OC3CON2 = 0b0000;               	// Clear OC3 configuration bits, Table 4-8
    OC3CON1bits.OCTSEL = 0b001;       	// Use Timer3, pg. 157
    OC3CON1bits.OCM = 0b110;          	// Edge-aligned PWM mode, pg. 158
    OC3CON2bits.SYNCSEL = 0b01101;    	// Unsyncronized, pg. 160

    // Set period and duty cycle
    PR3 = 25000;         	// Period of Timer3 = 50ms
                            // See Equation 15-1 in the data sheet
    OC3R = 5;               // On time of 10us

    // Turn on Timer2
    T3CONbits.TON = 1;
    
    _TRISB0 = 0;
    _TRISB1 = 0;
    
    ANSB = 0;
    
    TMR2 = 0;
    TMR3 = 12500;
	
}


void loader_finder_digital_setup()
{
    //TODO: Add all configurations needed to change pin to digital input...
    //      as well as anything else to make the switch between analog and digital
    
	// Configure CN interrupt on Pin2 and Pin3
    _CN3IE = 1;     // Enable CN on pin 6 (CNEN1 register)
    _CN3PUE = 0;    // Disable pull-up resistor (CNPU1 register)
    _CNIP = 4;      // Set CN interrupt priority (IPC4 register)
    _CNIF = 0;      // Clear interrupt flag (IFS1 register)
    _CNIE = 1;      // Enable CN interrupts (IEC1 register)
	
	loaderIrState = _RA1;   //IR Back
	
	for(i = 0; i <= IR_TIMES; i++)
	{
		irTimeValues[i] = 0;	//Start with a blank array
	}
    
    ANSA = 0;       //Turn off analog for port A
    _TRISA0 = 1;    //Pin2 as Input
    _TRISA1 = 1;    //Pin3 as Input
}


void ir_finder_analog_setup()
{
    //TODO: All configurations needed to switch between digital interrupt to
    //      analog input
    
    _CN3IE = 0;     // Disable CN on pin 6 (CNEN1 register)
    
    //------------------------------------------------------------------------
    // A /D Configuration Function
    //
    // This function configures the A/D to read from two channels in auto
    // conversion mode.
    //------------------------------------------------------------------------
    
    // AD1CHS register
    _CH0NA = 0;         // AD1CHS<7:5> -- Use VDD as negative input

    // AD1CON1 register
    _ADON = 1;          // AD1CON1<15> -- Turn on A/D
    _ADSIDL = 0;        // AD1CON1<13> -- A/D continues while in idle mode?
    _MODE12 = 1;        // AD1CON1<10> -- 12-bit or 10-bit?
    _FORM = 0b00;       // AD1CON1<9:8> -- Output format, pg. 211
    _SSRC = 0b0111;     // AD1CON1<7:4> -- Auto conversion (internal counter)
    _ASAM = 1;          // AD1CON1<2> -- Auto sampling

    // AD1CSSL registers
    AD1CSSL = 0b0000000000000011;   // AD1CSSL<15:0> -- Select lower channels to scan, turn on AN0 and AN1
    AD1CSSH = 0x0000;               // AD1CSSH<15:0> -- Select upper channels to scan, NOT USED!

    // AD1CON2 register, see pg. 212
    _PVCFG = 0b00;      // AD1CON2<15:14> -- Use VDD as positive ref voltage
    _NVCFG = 0;         // AD1CON2<13> -- Use VSS as negative ref voltage
    _BUFREGEN = 1;      // AD1CON2<11> -- Results stored using channel indexed
                        // mode -- AN1 result is stored in ADC1BUF1, AN2 result
                        // is stored in ADC1BUF2, etc.
    _CSCNA = 1;         // AD1CON2<10> -- Scans inputs specified in AD1CSSx
                        // registers instead of using channels specified
                        // by CH0SA bits in AD1CHS register
    _ALTS = 0;          // AD1CON2<0> -- Sample MUXA only
    _SMPI = 0b00001;    // AD1CON2<6:2> -- Interrupts at the conversion for
                        // every other sample

    // AD1CON3 register
    _ADRC = 0;              // AD1CON3<15> -- Use system clock
    _SAMC = 0x00001;        // AD1CON3<12:8> -- Auto sample every A/D period TAD
    _ADCS = 0x00111111;     // AD1CON3<7:0> -- A/D period TAD = 64*TCY

    ANSA = 1;       //Turn on analog for port A
    _TRISA0 = 1;    //Pin2 as Input
    _TRISA1 = 1;    //Pin3 as Input
}


void _ISR _CNInterrupt(void)    //Interrupt
{
    _CNIF = 0;      // Clear interrupt flag (IFS1 register)
	
	timeTemp = microseconds;
    timeTempMillis = milliseconds;
	
    if(_RA1 != loaderIrState) //Rear (towards the Loader) state has changed
    {
        loaderIrState = _RA1;
        
        for(i = 0; i < IR_TIMES; i++)
			{
				irTimeValues[i] = irTimeValues[i+1];	//Replace current value with next value...
			}
		irTimeValues[IR_TIMES] = (timeTempMillis);
    }
        
	else if(_RA2 != ultraLastStateB)	//The Rear Ultrasonic Pulse Train Just Changed
	{
        ultraLastStateB = _RA2;
        
		if(_RA2)	//Rear Ultrasonic Pulse Just Started
		{
			startTimeUltraB = timeTemp;
		}
		else	//Rear Ultrasonic Pulse Just Ended
		{
			for(i = 0; i < ULTRASONIC_VALUES; i++)
			{
				ultrasonicValuesB[i] = ultrasonicValuesB[i+1];	//Replace current value with next value...
			}
			ultrasonicValuesB[ULTRASONIC_VALUES] = (timeTemp - startTimeUltraB);
		}
	}
	
	else if(_RB2 != ultraLastStateF)	//The Front Ultrasonic Pulse Train Just Changed
	{
        ultraLastStateF = _RB2;
        
		if(_RB2)	//Front Ultrasonic Pulse Just Started
		{
			startTimeUltraF = timeTemp;
		}
		else	//Front Ultrasonic Pulse Just Ended
		{
			
			for(i = 0; i < ULTRASONIC_VALUES; i++)
			{
				ultrasonicValuesF[i] = ultrasonicValuesF[i+1];	//Replace current value with next value...
			}
			ultrasonicValuesF[ULTRASONIC_VALUES] = (timeTemp - startTimeUltraF);
		}
	}
}


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

