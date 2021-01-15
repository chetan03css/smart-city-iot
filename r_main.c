/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING 
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT 
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR 
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE 
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software 
* and to discontinue the availability of this software.  By using this software, 
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2010, 2011 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/G13 V1.03.01 [11 Oct 2011]
* Device(s)    : R5F100LE
* Tool-Chain   : CA78K0R
* Description  : This file implements main function.
* Creation Date: 5/2/2017
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
/* Start user code for include. Do not edit comment generated here */

#include"SMART_CITY_ALCD64.c"
#include "SMART_CITY_GPRS.c"

#define LED_G1 P5.0
#define LED_R1 P5.1
#define LED_G2 P5.2
#define LED_R2 P5.3
#define LED_G3 P5.4
#define LED_R3 P5.5

#define Rx1_1 P4.0
#define Rx1_2 P4.1
#define Rx2_1 P4.2

#define Ultra_Sensor P4.3
#define LDR_Lamp P3.0
#define LDR_sunlight P3.1
#define Relay_Led_Pole P1.7


/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
void Device_Init( void );
void Signal_Loop( void );

unsigned char Ambulance_ID[3] = "31";
unsigned char GPRS_TX_ARR[15]="$859$STL$D0L0@";
unsigned char Disp_Line[3] = "00"; 

extern unsigned char Rx_data_arr0[400], Rx_data_arr1[100];
extern unsigned char Rx_count0, Rx_count1;
extern unsigned char Rx_ST_Flag0, Rx_ST_Flag1;
extern unsigned char Rx_data0, Rx_data1;

extern unsigned int y;
extern signed char z;
extern unsigned long x;
extern unsigned char Disp_Line2[7];
extern unsigned char UC_Signal_ON_Time;

unsigned char Road_Count=0;
unsigned char LDR_Lamp_Flag, LDR_sunlight_Flag,Ultra_Sensor_FLag;
unsigned char Update_Count;

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
    /* Start user code. Do not edit comment generated here */
    unsigned char i;
	
	Device_Init( );    
	MSDelay(1000);								

	LED_R1 = 0;
	LED_R2 = 0;
	LED_R3 = 0;
	LED_G1 = 0;
	LED_G2 = 0;
	LED_G3 = 0;
	
//*********** RFID ie UART TEST **********************************************
	ALCD_Comm( 0x01 );								// Clear display Screen
	MSDelay(100);
	ALCD_Message( 0xC0, "   PUNCH CARD   " );		// PUNCH ANY CARD
	MSDelay(1000);									// 2 sec delay
	Rx_ST_Flag0 = 0;								// Indicates UART receive of first byte 
	Rx_count0 = 0;									// Take UART Receiver array to zeroth location
	while(1)
	{
		MSDelay(1000);
		if( Rx_ST_Flag0 == 1 )
		{
			MSDelay(500);							// 0.1 sec delay
			ALCD_Message( 0xC0, "                " );		// PUNCH ANY CARD
			Rx_data_arr0[8] = '\0';
			ALCD_Message( 0xC0, Rx_data_arr0 );		// Testing
			MSDelay(2000);							// 2 sec delay
			Rx_ST_Flag0 = 0;
			Rx_count0 = 0;
			MSDelay(1000);							// 2 sec delay
			break;
		}
	}
//****************************************************
	
	
	LED_R1 = 1;
	LED_R2 = 1;
	LED_R3 = 1;
	LED_G1 = 1;
	LED_G2 = 1;
	LED_G3 = 1;
	
	GPRS_TX_ARR[24] = '\0';
	GPRS_Send_Server( GPRS_TX_ARR, 14 );	
	ALCD_Message( 0xC0," GPRS ACTIVATED " );
	MSDelay(2000);
	
	ALCD_Comm( 0x01 );								// Clear display Screen
	MSDelay(1000);

	Road_Count = 0;
	Disp_Line2[0] = Road_Count + 48;
	z = (UC_Signal_ON_Time * 5);
	Disp_Line2[5] = (z%10) + 48;
	Disp_Line2[4] = (z/10) + 48;
	ALCD_Message( 0xC0, "ROAD " );
	ALCD_Message( 0xCB, " SEC " );
	ALCD_Message( 0xC5, Disp_Line2 );
	Signal_Loop( );
	R_TAU0_Channel0_Start( );
	
	Rx_ST_Flag0 = 0;								// Indicates UART receive of first byte 
	Rx_count0 = 0;
	
	Rx_ST_Flag1=0;
	Rx_count1=0;
	LDR_sunlight_Flag=0;
	LDR_Lamp_Flag=0;
	Ultra_Sensor_FLag=0;
	
	while(1)
	{
		Update_Count++;
		MSDelay(500);
		if( Update_Count >= 60 )
		{
			Update_Count = 0;
			ALCD_Message( 0x80, "STUS UPDTE" );
			GPRS_TX_ARR[10]=Disp_Line[0];
			GPRS_TX_ARR[12]=Disp_Line[1];
			GPRS_Send_Server( GPRS_TX_ARR, 14 );
			MSDelay(5000);						
			ALCD_Message( 0x80, "          " );

			Rx_ST_Flag1 = 0;
			Rx_count1 = 0;
		}
		
		ALCD_Message( 0xC5, Disp_Line2 );
 		MSDelay(200);
		
		if ( (Rx1_1 == 0) && (Rx1_2 == 0) ) 
		{
			/*LED_R3 = 0;
			LED_G3 = 1;
			LED_R2 = 0;
			LED_G2 = 1;
			LED_R1 = 1;
			LED_G1 = 0;*/
			
			Road_Count = 0;
			Signal_Loop( );
			UC_Signal_ON_Time = 4;
			z = 20;
		}
		else if(Rx1_1 == 0)
		{
			Road_Count = 0;
			Signal_Loop( );
			UC_Signal_ON_Time = 3;
			z = 15;
		}
		
		if(Rx2_1 == 0)
		{
			Road_Count = 1;
			Signal_Loop( );
			UC_Signal_ON_Time = 3;
			z = 15;
		}
		
		if( Rx_ST_Flag0 == 1 )
		{
			MSDelay(100);							// 0.1 sec delay
			Rx_data_arr0[8] = '\0';
			ALCD_Message( 0x80, Rx_data_arr0 );
			MSDelay(1000);
			ALCD_Message( 0xC5, Disp_Line2 );
			
			
			if( (Ambulance_ID[0]==Rx_data_arr0[6]) && (Ambulance_ID[1]==Rx_data_arr0[7]) )
			{
				ALCD_Message( 0x80, "AMBULANCE" );
				Road_Count = 0;
				Signal_Loop( );
				UC_Signal_ON_Time = 5;
				z = 25;
				MSDelay(1000);
			}

	
			ALCD_Message( 0xC5, Disp_Line2 );
			MSDelay(500);
			ALCD_Message( 0x80, "         " );


			Rx_ST_Flag0 = 0;
			Rx_count0 = 0;
		}
	
		if ( Ultra_Sensor == 1 )                                // DUSTBIN CONCEPT
		{
			ALCD_Message(0x8A,"D-FILD");
			//GPRS_TX_ARR[10]='1';
			Disp_Line[0]='1';
			MSDelay(4000);
			Rx_ST_Flag1 = 0;
			Rx_count1 = 0;
			
			ALCD_Message(0x8A,"      ");
		}
		else 
		{
			//Ultra_Sensor_FLag == 1
			//ALCD_Message(0x8A,"D-EMPT");
			//GPRS_TX_ARR[10]='0';
			Disp_Line[0]='0';
			MSDelay(3000);
			Rx_ST_Flag1 = 0;
			Rx_count1 = 0;
			
			//ALCD_Message(0x8A,"      ");	
		}
	
		
		if( (LDR_sunlight == 0) && (LDR_sunlight_Flag == 0) )           //STREET LIGHT CONCEPT
		{
			Relay_Led_Pole = 1;
			LDR_sunlight_Flag = 1;
			ALCD_Message( 0x8A,"NIGHT" ); 
			MSDelay(500);
			Rx_ST_Flag1 = 0;
			Rx_count1 = 0;
			
			ALCD_Message(0x8A, "     ");
		}
		else if( (LDR_sunlight == 1) && (LDR_sunlight_Flag == 1) )
		{
			Relay_Led_Pole = 0;
			LDR_sunlight_Flag = 0;
			ALCD_Message( 0x8A, "DAY" );
			MSDelay(500);
		}		

		if( (LDR_sunlight == 0) && (LDR_Lamp == 0) && (LDR_Lamp_Flag == 0) )
		{
			LDR_Lamp_Flag = 1;
			ALCD_Message( 0x8A, "L-PROB" );
			//GPRS_TX_ARR[12]='1';
			Disp_Line[1]='1';
			MSDelay(5000);
			ALCD_Message( 0x8A, "      " );
			Rx_ST_Flag1 = 0;
			Rx_count1 = 0;
			
		}
		else if( (LDR_sunlight == 0) && (LDR_Lamp == 1) && (LDR_Lamp_Flag == 1) )
		{
			LDR_Lamp_Flag = 0;
			ALCD_Message( 0x8A, "L-NORM" );
			//GPRS_TX_ARR[12]='0';
			Disp_Line[1]='0';
			MSDelay(5000);
			ALCD_Message( 0x8A, "      " );
			Rx_ST_Flag1 = 0;
			Rx_count1 = 0;
		}
			
		
	}
	

    while (1U)
    {
        ;
    }
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
void Device_Init( void )
{
	MSDelay(200);							// 0.5 sec delay
	ALCD_Init( );

	LED_R1 = 1;
	LED_R2 = 1;
	LED_R3 = 1;
	LED_G1 = 1;
	LED_G2 = 1;
	LED_G3 = 1;
	
	MSDelay(200);							// 0.5 sec delay
	ALCD_Message( 0x80, "   SMART CITY   " );
	ALCD_Message( 0xc0, "    USING IOT   " );
	MSDelay(1000);


	R_SAU0_Create( );
	R_UART0_Start( );
	MSDelay(500);
							
	R_UART1_Start( );
	MSDelay(500);
	
	R_TAU0_Create( );
	
	ALCD_Message( 0xC0,"  WAITING N/W   " );
	MSDelay(7000);	
	
	GPRS_Init_Comm( );
    	ALCD_Message( 0xC0,"   GPRS INIT    " );
	MSDelay(1000);	

}
void Signal_Loop( void )
{

	if( Road_Count >= 3 )
		Road_Count = 0;
		
		
		
	UC_Signal_ON_Time = 1;
	if ( Road_Count == 0 )
	{
		LED_R3 = 0;
		LED_G3 = 1;
		LED_R2 = 0;
		LED_G2 = 1;
		LED_R1 = 1;
		LED_G1 = 0;
		
/*		if( Rx1_1 == 0 )
		{
			UC_Signal_ON_Time = 3;	
			if( Rx1_2 == 0 )
			
				UC_Signal_ON_Time = 4;				// for 15 sec
		}

		else*/
		UC_Signal_ON_Time = 2;
				
		
		Road_Count = 1;
	}

	else if ( Road_Count == 1 )
	{
		LED_R3 = 0;
		LED_G3 = 1;
		LED_R1 = 0;
		LED_G1 = 1;
		LED_R2 = 1;
		LED_G2 = 0;

/*		if( Rx2_1 == 0 )
			UC_Signal_ON_Time = 3;				// for 15 sec					
		else*/
		
			UC_Signal_ON_Time = 2;				// for 10 sec
			
		Road_Count = 2;
	}

	else if ( Road_Count == 2 )
	{
		LED_R1 = 0;
		LED_G1 = 1;
		LED_R2 = 0;
		LED_G2 = 1;
		LED_R3 = 1;
		LED_G3 = 0;
		
		UC_Signal_ON_Time = 2;				// for 10 sec

		
		Road_Count = 3;
		
	}

	Disp_Line2[0] = Road_Count + 48;
	z = (UC_Signal_ON_Time * 5);
	x = 0;
	y = 0;

}
/* End user code. Do not edit comment generated here */
