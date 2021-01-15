
unsigned char GPRS_Comm_Err_Count;
unsigned char Control_Z = 0x1A;
unsigned char GPRS_Comm_OK_Flag;
unsigned char GPRS_ERROR_Flag;

extern unsigned char Rx_data_arr1[100];
extern unsigned char Rx_count1;
extern unsigned char Rx_ST_Flag1;
extern unsigned char Rx_data1;



//--------------------------------------------------------------------------------------------------------------------
// Function PROTOTYPES
//--------------------------------------------------------------------------------------------------------------------


void GPRS_Init_Comm( void );
void GPRS_Send_Server( unsigned char *GPRS_Data_Str_Ptr, unsigned char GPRS_Data_Str_Length );


//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

void GPRS_Init_Comm( void )
{
	unsigned char i;
	
	ALCD_Comm( 0x01 );								// Clear display Screen
	MSDelay(100);	

	Rx_ST_Flag1 = 0;
	Rx_count1 = 0;

	R_UART1_Send( "AT\r", 3 );
	ALCD_Message( 0x80, "AT");
	MSDelay(2000);

	R_UART1_Send( "AT+CMGF=1\r", 10 );
	ALCD_Message( 0x80, "AT+CMGF=1");
	MSDelay(2000);

	R_UART1_Send( "AT+CIPSHUT\r", 11 );
	ALCD_Message( 0x80, "AT+CIPSHUT");
	MSDelay(3000);
		
	R_UART1_Send( "AT+CIPMUX=0\r", 12 );
	ALCD_Message( 0x80, "AT+CIPMUX=0");
	MSDelay(2000);


	R_UART1_Send( "AT+CSTT=\"airtelgprs.com\",\"\",\"\"\r", 31 );
	ALCD_Message( 0x80, "airtelgprs.com");
	MSDelay(7000);

	R_UART1_Send( "AT+CIICR\r", 9 );
	ALCD_Message( 0x80, "AT+CIICR      ");
	MSDelay(5000);

	R_UART1_Send( "AT+CIFSR\r", 9 );
	ALCD_Message( 0x80, "AT+CIFSR");
	MSDelay(5000);


	R_UART1_Send( "AT+CIPSTART=\"TCP\",\"34.208.178.87\",\"85\"\r", 39 );
	
	ALCD_Message( 0x80, "34.208.178.87");
	MSDelay(8000);

	ALCD_Comm( 0x01 );								// Clear display Screen
	MSDelay(100);	
	
}


void GPRS_Send_Server( unsigned char *GPRS_Data_Str_Ptr, unsigned char GPRS_Data_Str_Length )
{
	unsigned char i;
	
	Rx_ST_Flag1 = 0;
	Rx_count1 = 0;
	R_UART1_Send( "AT+CIPSEND\r", 11 );
	MSDelay(1000);
	Rx_ST_Flag1 = 0;
	Rx_count1 = 0;
	R_UART1_Send( GPRS_Data_Str_Ptr, GPRS_Data_Str_Length );
	MSDelay(3000);
	R_UART1_Send( &Control_Z, 1 );
	MSDelay(500);
	Rx_ST_Flag1 = 0;
	Rx_count1 = 0;
//	while( Rx_ST_Flag1 == 0 )
	{
		;
	}
	MSDelay(2000);
}

