
// MCP2515 Mask and Filter example for extended CAN message frames.
// Written by Cory J. Fowler (20140717)

/***********************************************************************************
If you send the following extended IDs below to an Arduino loaded with this sketch
you will find that 0x00FFCC00 and 0x00FF9900 will not get in.

   ID in Hex is the same as the Filter in Hex.
   0x00FFEE00
   0x00FFDD00
   0x00FFCC00  This example will NOT be receiving this ID
   0x00FFBB00
   0x00FFAA00
   0x00FF9900  This example will NOT be receiving this ID
   0x00FF8800
   0x00FF7700

   This mask will check the filters against ID bits 23 through 8.
   (Those familiar with J1939 might see why I used this mask.)
    MASK = 0x00FFFF00
   If there is an explicit filter match to those bits, the message will be passed to the
   receive buffer and the interrupt pin will be set.
   
   This example will NOT be exclusive to ONLY the above message IDs, for that a mask such
   as the below would be used: 
    MASK = 0x1FFFFFFF
   
   At the moment, to disable a filter or mask, copy the value of a used filter or mask.
   
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
  CAN0.init_Mask(0,1,0x00FFFF00);
  /* Init first filter... */
  CAN0.init_Filt(0,1,0x00FFEE00);
  /* Init second filter... */
  CAN0.init_Filt(1,1,0x00FFDD00);

  /* Init second mask... */
  CAN0.init_Mask(1,1,0x00FFFF00);
  /* Init third filter... */
  CAN0.init_Filt(2,1,0x00FFBB00);
  // Init fourth filter... */
  CAN0.init_Filt(3,1,0x00FFAA00);
  /*Init fifth filter... */
  CAN0.init_Filt(4,1,0x00FF8800);
  /* Init sixth filter... */
  CAN0.init_Filt(5,1,0x00FF7700);
  
  Serial.println("MCP2515 Library Extended Mask & Filter Example...");
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
