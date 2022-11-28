
// MCP2515 Mask and Filter example for standard CAN message frames.
// Written by Cory J. Fowler (20140717)

/***********************************************************************************
If you send the following standard IDs below to an Arduino loaded with this sketch
you will find that 0x102 and 0x105 will not get in.

ID in Hex  -   Two Data Bytes!   -  Filter/Mask in HEX
   0x100   + 0000 0000 0000 0000 =   0x01000000
   0x101   + 0000 0000 0000 0000 =   0x01010000
   0x102   + 0000 0000 0000 0000 =   0x01020000  This example will NOT be receiving this ID
   0x103   + 0000 0000 0000 0000 =   0x01030000
   0x104   + 0000 0000 0000 0000 =   0x01040000
   0x105   + 0000 0000 0000 0000 =   0x01050000  This example will NOT be receiving this ID
   0x106   + 0000 0000 0000 0000 =   0x01060000
   0x107   + 0000 0000 0000 0000 =   0x01070000

   This mask will check the filters against ID bit 8 and ID bits 3-0.   
    MASK   + 0000 0000 0000 0000 =   0x010F0000
   
   If there is an explicit filter match to those bits, the message will be passed to the
   receive buffer and the interrupt pin will be set.
   This example will NOT be exclusive to ONLY the above frame IDs, for that a mask such
   as the below would be used: 
    MASK   + 0000 0000 0000 0000 = 0x07FF0000
    
   This mask will check the filters against all ID bits and the first data byte:
    MASK   + 1111 1111 0000 0000 = 0x07FFFF00
   If you use this mask and do not touch the filters below, you will find that your first
   data byte must be 0x00 for the message to enter the receive buffer.
   
   At the moment, to disable a filter or mask, copy the value of a used filter or mask.
   
   Data bytes are ONLY checked when the MCP2515 is in 'MCP_STDEXT' mode via the begin
   function, otherwise ('MCP_STD') only the ID is checked.
***********************************************************************************/


#include <mcp_can.h>
#include <SPI.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

/* Serial Output String Buffer */
char msgString[128];

#define REMOTE_CAN_ID_FORMAT 0x40000000
#define EXTENDED_CAN_ID_FORMAT 0x80000000
/* Set INT to pin 20 */
#define CAN0_INT 20
/* Set CS to pin  19 */
MCP_CAN CAN0(19);

void setup()
{
  Serial.begin(115200);
  /* Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled. */
  if(CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized Successfully!");
  } else {
    Serial.println("Error Initializing MCP2515...");
  }
  /* Configuring pin for INT input */
  pinMode(CAN0_INT, INPUT);

  /* Init first mask... */
  CAN0.init_Mask(0,0,0x010F0000);
  /* Init first filter... */
  CAN0.init_Filt(0,0,0x01000000);
  /* Init second filter... */
  CAN0.init_Filt(1,0,0x01010000);

  /* Init second mask... */
  CAN0.init_Mask(1,0,0x010F0000);
  /* Init third filter... */
  CAN0.init_Filt(2,0,0x01030000);
  // Init fourth filter... */
  CAN0.init_Filt(3,0,0x01040000);
  /*Init fifth filter... */
  CAN0.init_Filt(4,0,0x01060000);
  /* Init sixth filter... */
  CAN0.init_Filt(5,0,0x01070000);
 
  Serial.println("MCP2515 Library Standard Mask & Filter Example...");
  /* Change to normal mode to allow messages to be transmitted */
  CAN0.setMode(MCP_NORMAL);
}

void loop()
{
  /* If pin CAN0_INT is low, read receive buffer */
  if(!digitalRead(CAN0_INT)) {
    /* Read data: len = data length, buf = data byte(s) */
    CAN0.readMsgBuf(&rxId, &len, rxBuf);
    /* Determine if ID is standard (11 bits) or extended (29 bits) */
    if((rxId & EXTENDED_CAN_ID_FORMAT) == EXTENDED_CAN_ID_FORMAT) {
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    } else {
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
    }
    Serial.print(msgString);

    /* Determine if message is a remote request frame. */
    if((rxId & REMOTE_CAN_ID_FORMAT) == REMOTE_CAN_ID_FORMAT) {
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++) {
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(msgString);
      }
    }
    Serial.println();
  }
}

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
