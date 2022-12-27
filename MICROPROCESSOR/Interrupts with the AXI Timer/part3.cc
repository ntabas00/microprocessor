#include <iostream>
using namespace std;


#include "xparameters.h"
#include "xil_types.h"

#include "xtmrctr.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xscugic.h"
#include <stdio.h>

/* Instance of the Interrupt Controller */
XScuGic InterruptController;

/* The configuration parameters of the controller */
static XScuGic_Config *GicConfig;

// Timer Instance
XTmrCtr TimerInstancePtr;

int test = 0;

void Timer_InterruptHandler(void)
{
	//define the timer pointer
	unsigned int* timer_ptr = (unsigned int*)XPAR_AXI_TIMER_0_BASEADDR;

	cout << "Value for first:" << *(timer_ptr) << " and second: " << *(timer_ptr + 4) << endl;

	//stop both timers by setting the 8th bit to 0
	*(timer_ptr) = 0x054;
	*(timer_ptr + 4) = 0x054;

	cout << "Value for first:" << *(timer_ptr) << " and second: " << *(timer_ptr + 4) << endl;

	//read the value from the timer and determine which one called the interrupt
	if(*(timer_ptr) & (1<<8))
	{
		cout << "the interrupt has been called by timer_ptr" << endl;
	}
	else{ cout << "the interrupt has been called by timer_ptr + 4" << endl;}

	//prompt user to enter a value to begin the timer
	char input;
	cout << "Press any key to start the timer" << endl;
	cin >> input ;
	cout << "You pressed "<<  input << endl;
	cout << "Enabling the timer to start" << endl;

	*(timer_ptr) = 0x0d4 ;
	*(timer_ptr + 4) = 0x0d4 ;

}

int SetUpInterruptSystem(XScuGic *XScuGicInstancePtr)
{
	/*
	* Connect the interrupt controller interrupt handler to the hardware
	* interrupt handling logic in the ARM processor.
	*/
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
	(Xil_ExceptionHandler) XScuGic_InterruptHandler,
	XScuGicInstancePtr);
	/*
	* Enable interrupts in the ARM
	*/
	Xil_ExceptionEnable();
	return XST_SUCCESS;
}

int ScuGicInterrupt_Init(u16 DeviceId,XTmrCtr *TimerInstancePtr)
{
	int Status;
	/*
	* Initialize the interrupt controller driver so that it is ready to
	* use.
	* */

	GicConfig = XScuGic_LookupConfig(DeviceId);
	if (NULL == GicConfig)
	{
		return XST_FAILURE;
	}
	Status = XScuGic_CfgInitialize(&InterruptController, GicConfig,
	GicConfig->CpuBaseAddress);

	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	/*
	* Setup the Interrupt System
	* */
	Status = SetUpInterruptSystem(&InterruptController);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}



	XScuGic_CPUWriteReg(&InterruptController, XSCUGIC_EOI_OFFSET, XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR);

	/*
	* Connect a device driver handler that will be called when an
	* interrupt for the device occurs, the device driver handler performs
	* the specific interrupt processing for the device
	*/
	Status = XScuGic_Connect(&InterruptController,
							 XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR,
							 (Xil_ExceptionHandler)XTmrCtr_InterruptHandler,
							 (void *)TimerInstancePtr);

	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	/*
	* Enable the interrupt for the device and then cause (simulate) an
	* interrupt so the handlers will be called
	*/
	XScuGic_Enable(&InterruptController, XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR);

	return XST_SUCCESS;
}


int main()
{
    cout << "Application starts " << endl;
    int xStatus;


        //~~~~~~~~~
        //Step-1 :AXI Timer Initialization
        //~~~~~~~~~
        xStatus = XTmrCtr_Initialize(&TimerInstancePtr, XPAR_AXI_TIMER_0_DEVICE_ID);
        if(XST_SUCCESS != xStatus)
        {
                cout << "TIMER INIT FAILED " << endl;
                if(xStatus == XST_DEVICE_IS_STARTED)
                {
                        cout << "TIMER has already started" << endl;
                        cout << "Please power cycle your board, and re-program the bitstream" << endl;
                }
                return 1;
        }





	//~~~~~~~~~
	//Step-2 :Set Timer Handler
	//~~~~~~~~~
        //
        // cast second argument to data type XTmrCtr_Handler since in gcc it gave a warning
        // and with g++ for the C++ version it resulted in an error

	XTmrCtr_SetHandler(&TimerInstancePtr, (XTmrCtr_Handler)Timer_InterruptHandler, &TimerInstancePtr);



      // intialize time pointer with value from xparameters.h file

 unsigned int* timer_ptr = (unsigned int* )XPAR_AXI_TIMER_0_BASEADDR;

         //~~~~~~~~~
        //Step-3 :load the reset value
        //~~~~~~~~~
       *(timer_ptr+ 1) = 0xf0000000;
       *(timer_ptr+ 5) = 0xffffff00;



         //~~~~~~~~~
         //Step-4 : set the timer options
         //~~~~~~~~~
         // from xparameters.h file #define XPAR_AXI_TIMER_0_BASEADDR 0x42800000
         //  Configure timer in generate mode, count up, interrupt enabled
         //  with autoreload of load register

       *(timer_ptr)  = 0x0f4 ;
       *(timer_ptr + 4)  = 0x0f4 ;



	//~~~~~~~~~
	//Step-5 : SCUGIC interrupt controller Initialization
	//Registration of the Timer ISR
	//~~~~~~~~~
	xStatus=
	ScuGicInterrupt_Init(XPAR_PS7_SCUGIC_0_DEVICE_ID, &TimerInstancePtr);
	if(XST_SUCCESS != xStatus)
	{
		cout << " :( SCUGIC INIT FAILED )" << endl;
		return 1;
	}


       // Beginning of our main code


	//We want to control when the timer starts
	char input;
	cout << "Press any key to start the timer" << endl;
	cin >> input ;
	cout << "You pressed "<<  input << endl;
    cout << "Enabling the timer to start" << endl;

        *(timer_ptr) = 0x0d4 ;   // deassert the load 5 to allow the timer to start counting
        *(timer_ptr + 4) = 0x0d4 ;

        // let timer run forever generating periodic interrupts

	while( 1)
        {
          //  // wait forever and let the timer generate periodic interrupts
        }


    return 0;
} 
