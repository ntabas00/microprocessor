#include "xil_exception.h"
#include "xil_cache.h"
#include "xparameters.h"
#include <iostream>
#include "xtmrctr.h"

using namespace std;

int main()
{
	XTmrCtr TimerInstancePtr;// timer pointer
	u32* cdma_ptr = (u32*) XPAR_AXI_CDMA_0_BASEADDR;   //pointer for cdma base address
	u32* source_ptr = (u32*) XPAR_PS7_DDR_0_S_AXI_HP0_BASEADDR; // pointer for source address
	u32* destination_ptr = (u32*) XPAR_PS7_DDR_0_S_AXI_HP2_BASEADDR; //pointer for destinationaddress
    int xStatus;




    int size= 1048576;

	for(int i =0; i <= size; i++)  //initializing the contents of the source array
	{
		*(source_ptr + i) = i;
	}


	for(int i =0; i <= size; i++)//initializing the contents of the destination array
	{
		*(destination_ptr + i) = -i;
	}






	xStatus = XTmrCtr_Initialize(&TimerInstancePtr, XPAR_AXI_TIMER_0_DEVICE_ID);

	if(xStatus != XST_SUCCESS)
        {
	   cout <<  "TIMER INIT FAILED" << endl;
	   return 0;
	}


       //setting the timer reset value
	XTmrCtr_SetResetValue(&TimerInstancePtr, 0, 0);

	//setting the timer option
	XTmrCtr_SetOptions(&TimerInstancePtr, XPAR_AXI_TIMER_0_DEVICE_ID, XTC_CAPTURE_MODE_OPTION);


	//reset the cdma
	*(cdma_ptr) = 0x00000004;

	//configure the cdma
	*(cdma_ptr) = 0x00000020;

	//load address of source array
	*(cdma_ptr + 6) = 0x20000000;

	//load address of destination array
	*(cdma_ptr + 8) = 0x30000000;

	//flush the cash
	Xil_DCacheFlush();

	//number of bytes to transfer

	*(cdma_ptr + 10) = size*4;



	//start the timer
	XTmrCtr_Start(&TimerInstancePtr, 0);




       //begin transfer
	int idle_bit_in_status_register ;
	idle_bit_in_status_register = *(cdma_ptr + 1) & 2;
	while(idle_bit_in_status_register== 0)
	{


		idle_bit_in_status_register = *(cdma_ptr + 1) & 2; //isolate the idle bit
		     if(idle_bit_in_status_register == 2){
                      break;
                  }
	}



	XTmrCtr_Stop(&TimerInstancePtr, 0);   // stop timer
	unsigned int clock_timer;
	clock_timer = XTmrCtr_GetValue(&TimerInstancePtr, 0);

	bool are_equal = false;


	for(int i=0 ; i <= 64; i++)

			if  (  *(destination_ptr + i) = *(source_ptr + i)){

				are_equal = true;

                        cout <<"same vale for teh destination and source : " << *(destination_ptr + i) << " " << *(source_ptr + i) << endl;

			     }
			         else
			              {
			        	   cout <<"values are diffrent for destination   and source " << *(destination_ptr + i) << " " << *(source_ptr + i) << endl;
			        	   cout<< "app ends"<<endl;
			              }


	if (!(are_equal)){
             cout << "values are different for source and destination reg!" << endl;
             cout<< "app ends"<<endl;

         }
	else {
              cout << "values for source and destination register are equal!" << endl;
           }

        cout << "it takes  "<<  clock_timer <<" to perform"<<endl;



	return 0;
}
