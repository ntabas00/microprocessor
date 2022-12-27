#include "stdbool.h"
#include "xparameters.h"
#include "xil_types.h"
#include "xgpio.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xtmrctr.h"
#include <iostream>

using namespace std;

int main()
{
        float DutyCycle;
        float TimeCycle;
        int xStatus;
	    static XGpio GPIOInstance_Ptr;
	    XTmrCtr TimerInstance_Ptr;
	    u32* Timer_Ptr = (u32*)XPAR_TMRCTR_0_BASEADDR;


	cout << "#### counter Application Starts ####"<< endl;

         if( DutyCycle >  TimeCycle) {

        cout << "" << endl;

         return 1;
     }

           //Step-1: AXI GPIO Initialization

	xStatus = XGpio_Initialize(&GPIOInstance_Ptr,XPAR_AXI_GPIO_FOR_OUTPUT_DEVICE_ID);
	if(xStatus != XST_SUCCESS)
	{
	cout << "GPIO A Initialization FAILED" << endl;
	return 1;
         }

          *(Timer_Ptr) = 0x206;   // pwm=1, gent0(external signal)=1, udt0(counter down)=1
          *(Timer_Ptr+4) = 0x206;  //

       cout <<"enter duty cycle: " << endl;
       cin  >> DutyCycle;

       cout <<"enter period : " << endl;
       cin  >> TimeCycle;



       cout << "duty cycle: "<<DutyCycle<<endl;
       cout << "time cycle: "<<TimeCycle<<endl;



       // timer1 load


        *(Timer_Ptr+1) = (TimeCycle*50000000) -2;

        //timer2 load
        (Timer_Ptr+5) = (TimeCycle(DutyCycle/100)*50000000);



      //load value into timer

       *(Timer_Ptr) = 0x226; // pwm=1, gent0(external signal) =1, udt0(counter down)=1 , LDR=1
       *(Timer_Ptr+4) = 0x226; // pwm=1, gent0(external signal) =1, udt0(counter down) =1 , LDR=1

        // stop load and enable timer
       *(Timer_Ptr) = 0x286;  // pwm=1, gent0(external signal)=1, udt0(counter down)=1, ENT( enable timer) = 1
       *(Timer_Ptr+4) = 0x286;  // pwm=1, gent0(external signal)=1, udt0(counter down)=1, ENT( enable timer) = 1



          return 0;
}


