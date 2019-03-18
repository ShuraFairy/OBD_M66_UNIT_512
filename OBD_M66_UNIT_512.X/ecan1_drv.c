/*******************************************************************************
  ECAN buffer and filter configuration source file

  Company:
    Microchip Technology Inc.

  File Name:
    ecan1drv.c

  Summary:
    Configures the message acceptance filters and the data buffers.

  Description:
    This source file sets the mask for the acceptance filters for the incoming
    data and also configures the data buffers by specifying identifiers for them.
*******************************************************************************/
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <xc.h>
#include <stdint.h>
#include "ecan1_config.h"

extern volatile int rxmsg1;
extern volatile unsigned int CAN1AUX_R_ptr, CAN1AUX_W_ptr;
extern unsigned long ID;
extern volatile unsigned int CAN1_fault;
extern unsigned char __attribute__ ((far)) CAN1_DATA_buf [8];
extern unsigned char __attribute__ ((far)) CANOBD_DATA_buf1 [8];
extern unsigned char __attribute__ ((far)) CANOBD_DATA_buf2 [8];
extern unsigned char __attribute__ ((far)) CANOBD_DATA_buf3 [8];
extern volatile unsigned char __attribute__ ((far)) CAN1AUX_DATA_buf[25][8];
extern volatile unsigned long __attribute__ ((far)) CAN1AUX_ID_buf[25][1];
extern unsigned char BRPVAL1;
extern unsigned int bitrate1, Fp1;
extern union __attribute__ ((far))  {
    unsigned int CAN1SETTINGS;
   struct 
    {
		unsigned char on:1;
        unsigned char ide:1;
        unsigned int  bitrate:10;
        unsigned int :4;
    };
} CAN1SETTINGSbits;
extern union __attribute__ ((far))  {
    unsigned long CAN1CFG;
   struct 
    {
		unsigned char   sjw:2;
        unsigned char  seg1:3;
        unsigned char  seg2:3;
        unsigned char prseg:3;
        unsigned char   sam:1;
        unsigned char      :4;
        
		unsigned char   sjw_slp:2;
        unsigned char  seg1_slp:3;
        unsigned char  seg2_slp:3;
        unsigned char prseg_slp:3;
        unsigned char   sam_slp:1;
        unsigned char      :4;        
    };
} CAN1CFGbits;

/******************************************************************************
 * Function:     void Ecan1WriteRxAcptFilter(int16_t n, int32_t identifier,
 *               uint16_t exide,uint16_t bufPnt,uint16_t maskSel)
 *
 * PreCondition:  None
 *
 * Input:         n-> Filter number [0-15]
 *                identifier-> Bit ordering is given below
 *                Filter Identifier (29-bits) :
 *                0b000f ffff ffff ffff ffff ffff ffff ffff
 *                     |____________|_____________________|
 *                        SID10:0          EID17:0
 *
 *               Filter Identifier (11-bits) :
 *               0b0000 0000 0000 0000 0000 0fff ffff ffff
 *                                           |___________|
 *                                             SID10:
 *               exide -> "0" for standard identifier
 *                        "1" for Extended identifier
 *               bufPnt -> Message buffer to store filtered message [0-15]
 *               maskSel -> Optinal Masking of identifier bits [0-3]
 *
 * Output:        None
 *
 * Side Effects:  None
 *
 * Overview:      Configures Acceptance filter "n" for ECAN1.
 *****************************************************************************/
void ecan1WriteRxAcptFilter( int16_t n, int32_t identifier, uint16_t exide, uint16_t bufPnt, uint16_t maskSel )
{
    uint32_t    sid10_0 = 0;

    uint32_t    eid15_0 = 0;

    uint32_t    eid17_16 = 0;
    uint16_t    *sidRegAddr;
    uint16_t    *bufPntRegAddr;
    uint16_t    *maskSelRegAddr;
    uint16_t    *fltEnRegAddr;

    C1CTRL1bits.WIN = 1;

    // Obtain the Address of CiRXFnSID, CiBUFPNTn, CiFMSKSELn and CiFEN register for a given filter number "n"
    sidRegAddr = ( uint16_t * ) ( &C1RXF0SID + (n << 1) );
    bufPntRegAddr = ( uint16_t * ) ( &C1BUFPNT1 + (n >> 2) );
    maskSelRegAddr = ( uint16_t * ) ( &C1FMSKSEL1 + (n >> 3) );
    fltEnRegAddr = ( uint16_t * ) ( &C1FEN1 );

    // Bit-filed manupulation to write to Filter identifier register
    if( exide == 1 )
    {   // Filter Extended Identifier
        eid15_0 = ( identifier & 0xFFFF );
        eid17_16 = ( identifier >> 16 ) & 0x3;
        sid10_0 = ( identifier >> 18 ) & 0x7FF;

        *sidRegAddr = ( ((sid10_0) << 5) + 0x8 ) + eid17_16;    // Write to CiRXFnSID Register
        *( sidRegAddr + 1 ) = eid15_0;  // Write to CiRXFnEID Register
    }
    else
    {   // Filter Standard Identifier
        sid10_0 = ( identifier & 0x7FF );
        *sidRegAddr = ( sid10_0 ) << 5; // Write to CiRXFnSID Register
        *( sidRegAddr + 1 ) = 0;        // Write to CiRXFnEID Register
    }

    *bufPntRegAddr = ( *bufPntRegAddr ) & ( 0xFFFF - (0xF << (4 * (n & 3))) );      // clear nibble
    *bufPntRegAddr = ( (bufPnt << (4 * (n & 3))) | (*bufPntRegAddr) );              // Write to C1BUFPNTn Register
    *maskSelRegAddr = ( *maskSelRegAddr ) & ( 0xFFFF - (0x3 << ((n & 7) * 2)) );    // clear 2 bits
    *maskSelRegAddr = ( (maskSel << (2 * (n & 7))) | (*maskSelRegAddr) );           // Write to C1FMSKSELn Register
    *fltEnRegAddr = ( (0x1 << n) | (*fltEnRegAddr) );   // Write to C1FEN1 Register
    C1CTRL1bits.WIN = 0;
}

/******************************************************************************
 * Function:     void Ecan1WriteRxAcptMask(int16_t m, int32_t identifier,
 *               uint16_t mide, uint16_t exide)
 *
 * PreCondition:  None
 *
 * Input:        m-> Mask number [0-2]
                 identifier-> Bit ordering is given below n-> Filter number [0-15]
 *                identifier-> Bit ordering is given below
 *                Filter mask Identifier (29-bits) :
 *                0b000f ffff ffff ffff ffff ffff ffff ffff
 *                     |____________|_____________________|
 *                        SID10:0          EID17:0
 *
 *               Filter mask Identifier (11-bits) :
 *               0b0000 0000 0000 0000 0000 0fff ffff ffff
 *                                           |___________|
 *                                             SID10:
 *               mide ->  "0"  Match either standard or extended address message
 *                             if filters match
 *                        "1"  Match only message types that correpond to
 *                             'exide' bit in filter
 *
 * Output:        None
 *
 * Side Effects:  None
 *
 * Overview:      Configures Acceptance filter "n" for ECAN1.
 *****************************************************************************/
void ecan1WriteRxAcptMask( int16_t m, int32_t identifier, uint16_t mide, uint16_t exide )
{
    uint32_t    sid10_0 = 0;

    uint32_t    eid15_0 = 0;

    uint32_t    eid17_16 = 0;
    uint16_t    *maskRegAddr;

    C1CTRL1bits.WIN = 1;

    // Obtain the Address of CiRXMmSID register for given Mask number "m"
    maskRegAddr = ( uint16_t * ) ( &C1RXM0SID + (m << 1) );

    // Bit-filed manupulation to write to Filter Mask register
    if( exide == 1 )
    {   // Filter Extended Identifier
        eid15_0 = ( identifier & 0xFFFF );
        eid17_16 = ( identifier >> 16 ) & 0x3;
        sid10_0 = ( identifier >> 18 ) & 0x7FF;

        if( mide == 1 )
        {
            *maskRegAddr = ( (sid10_0) << 5 ) + 0x0008 + eid17_16;  // Write to CiRXMnSID Register
        }
        else
        {
            *maskRegAddr = ( (sid10_0) << 5 ) + eid17_16;           // Write to CiRXMnSID Register
        }

        *( maskRegAddr + 1 ) = eid15_0; // Write to CiRXMnEID Register
    }
    else
    {   // Filter Standard Identifier
        sid10_0 = ( identifier & 0x7FF );
        if( mide == 1 )
        {
            *maskRegAddr = ( (sid10_0) << 5 ) + 0x0008; // Write to CiRXMnSID Register
        }
        else
        {
            *maskRegAddr = ( sid10_0 ) << 5;            // Write to CiRXMnSID Register
        }

        *( maskRegAddr + 1 ) = 0;                       // Write to CiRXMnEID Register
    }

    C1CTRL1bits.WIN = 0;
}

/******************************************************************************
 * Function:     void Ecan1WriteTxMsgBufId(uint16_t buf, int32_t txIdentifier, uint16_t ide,
 *               uint16_t remoteTransmit)
 *
 * PreCondition:  None
 *
 * Input:        buf    -> Transmit Buffer Number
 *               txIdentifier ->
 *               Extended Identifier (29-bits):
 *                0b000f ffff ffff ffff ffff ffff ffff ffff
 *                     |____________|_____________________|
 *                        SID10:0          EID17:0
 *
 *               Standard Identifier (11-bits) :
 *               0b0000 0000 0000 0000 0000 0fff ffff ffff
 *                                           |___________|
 *                                             SID10:
 *                 Standard Message Format:
 *                                             Word0 : 0b000f ffff ffff ffff
 *                                                          |____________|||___
 *                                                             SID10:0   SRR   IDE
 *                                             Word1 : 0b0000 0000 0000 0000
 *                                                            |____________|
 *                                                               EID17:6
 *                                             Word2 : 0b0000 00f0 0000 ffff
 *                                                       |_____||           |__|
 *                                                       EID5:0 RTR         DLC
 *                Extended Message Format:
 *                                          Word0 : 0b000f ffff ffff ffff
 *                                                       |____________|||___
 *                                                          SID10:0   SRR   IDE
 *                                          Word1 : 0b0000 ffff ffff ffff
 *                                                         |____________|
 *                                                               EID17:6
 *                                          Word2 : 0bffff fff0 0000 ffff
 *                                                    |_____||           |__|
 *                                                    EID5:0 RTR         DLC
 *             ide -> "0"  Message will transmit standard identifier
 *                    "1"  Message will transmit extended identifier
 *
 *            remoteTransmit -> "0" Message transmitted is a normal message
 *                              "1" Message transmitted is a remote message
 *            Standard Message Format:
 *                                          Word0 : 0b000f ffff ffff ff1f
 *                                                       |____________|||___
 *                                                          SID10:0   SRR   IDE
 *                                          Word1 : 0b0000 0000 0000 0000
 *                                                         |____________|
 *                                                            EID17:6
 *                                          Word2 : 0b0000 0010 0000 ffff
 *                                                  |_____||           |__|
 *                                                  EID5:0 RTR         DLC
 *
 *         Extended Message Format:
 *                                         Word0 : 0b000f ffff ffff ff1f
 *                                                      |____________|||___
 *                                                        SID10:0   SRR   IDE
 *                                         Word1 : 0b0000 ffff ffff ffff
 *                                                        |____________|
 *                                                          EID17:6
 *                                         Word2 : 0bffff ff10 0000 ffff
 *                                                   |_____||           |__|
 *                                                   EID5:0 RTR         DLC
 *
 * Output:        None
 *
 * Side Effects:  None
 *
 * Overview:      This function configures ECAN1 message buffer.
 *****************************************************************************/
void ecan1WriteTxMsgBufId( uint16_t buf, int32_t txIdentifier, uint16_t ide, uint16_t remoteTransmit )
{
    uint32_t    word0 = 0;

    uint32_t    word1 = 0;

    uint32_t    word2 = 0;
    uint32_t    sid10_0 = 0;
    uint32_t    eid5_0 = 0;
    uint32_t    eid17_6 = 0;

    if( ide )
    {
        eid5_0 = ( txIdentifier & 0x3F );
        eid17_6 = ( txIdentifier >> 6 ) & 0xFFF;
        sid10_0 = ( txIdentifier >> 18 ) & 0x7FF;
        word1 = eid17_6;
    }
    else
    {
        sid10_0 = ( txIdentifier & 0x7FF );
    }

    if( remoteTransmit == 1 )
    {   // Transmit Remote Frame
        word0 = ( (sid10_0 << 2) | ide | 0x2 );
        word2 = ( (eid5_0 << 10) | 0x0200 );
    }
    else
    {
        word0 = ( (sid10_0 << 2) | ide );
        word2 = ( eid5_0 << 10 );
    }

    // Obtain the Address of Transmit Buffer in DMA RAM for a given Transmit Buffer number
    if( ide )
    {
        ecan1msgBuf[buf][0] = ( word0 | 0x0002 );
    }
    else
    {
        ecan1msgBuf[buf][0] = word0;
    }

    ecan1msgBuf[buf][1] = word1;
    ecan1msgBuf[buf][2] = word2;
}

/******************************************************************************
 * Function:     void Ecan1WriteTxMsgBufData(uint16_t buf, uint16_t dataLength,
 *    uint16_t data1, uint16_t data2, uint16_t data3, uint16_t data4)
 *
 * PreCondition:  None
 *
 * Input:            buf    -> Transmit Buffer Number
 *              dataLength  -> data length in bytes.
 *    	        actual data -> data1, data2, data3, data4
 *
 * Output:        None
 *
 * Side Effects:  None
 *
 * Overview:      This function transmits ECAN data.
 *****************************************************************************/
void ecan1WriteTxMsgBufData( uint16_t buf, uint16_t dataLength, uint16_t data1, uint16_t data2, uint16_t data3,
                             uint16_t data4 )
{
    ecan1msgBuf[buf][2] = ( (ecan1msgBuf[buf][2] & 0xFFF0) + dataLength );

    ecan1msgBuf[buf][3] = data1;
    ecan1msgBuf[buf][4] = data2;
    ecan1msgBuf[buf][5] = data3;
    ecan1msgBuf[buf][6] = data4;
}

/******************************************************************************
 * Function:     void Ecan1DisableRXFilter(int16_t n)
 *
 * PreCondition:  None
 *
 * Input:          n -> Filter number [0-15]
 *
 * Output:        None
 *
 * Side Effects:  None
 *
 * Overview:          Disables RX Acceptance Filter.
 *****************************************************************************/
void ecan1DisableRXFilter( int16_t n )
{
    uint16_t    *fltEnRegAddr;
    C1CTRL1bits.WIN = 1;
    fltEnRegAddr = ( uint16_t * ) ( &C1FEN1 );
    *fltEnRegAddr = ( *fltEnRegAddr ) & ( 0xFFFF - (0x1 << n) );
    C1CTRL1bits.WIN = 0;
}



int isfreetx(int buf)
{
  switch (buf)
   {
   case 0 : return !C1TR01CONbits.TXREQ0;
   case 1 : return !C1TR01CONbits.TXREQ1;
   case 2 : return !C1TR23CONbits.TXREQ2;
   case 3 : return !C1TR23CONbits.TXREQ3;
   case 4 : return !C1TR45CONbits.TXREQ4;  
   case 5 : return !C1TR45CONbits.TXREQ5;
   case 6 : return !C1TR67CONbits.TXREQ6;
   case 7 : return !C1TR67CONbits.TXREQ7;
   }
 return 0;
}

void settxtransmit(int buf)
{
switch (buf)
   {
   case 0 : C1TR01CONbits.TXREQ0=1;	
			break;
   case 1 : C1TR01CONbits.TXREQ1=1;
			break;
   case 2 : C1TR23CONbits.TXREQ2=1;
 			break;
   case 3 : C1TR23CONbits.TXREQ3=1;
			break;
   case 4 : C1TR45CONbits.TXREQ4=1;
			break;
   case 5 : C1TR45CONbits.TXREQ5=1;
			break;
   case 6 : C1TR67CONbits.TXREQ6=1;
			break;
   case 7 : C1TR67CONbits.TXREQ7=1;
			break;
   }
}
 
int sendnormalmsg(int prio, long txidentifier,unsigned int dataLength, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4 )
{ // assumes buffer 0..7 have increasing priority
  // prio to send is nr of buffer tried first, decrease by availability. 
//
  prio++; // 1..8 
  while (prio>0)
    {
      if (isfreetx(prio-1))
       {
         ecan1WriteTxMsgBufId(prio-1,txidentifier,1,0);
         ecan1WriteTxMsgBufData(prio-1,dataLength,data1,data2,data3,data4); 
         settxtransmit(prio-1);
         return prio; // 1..8 if succesful.
       }
     prio--;
    }
   return 0;
}

//void processshorttxtdmsg(int id,
//#if defined(__dsPIC33E__)
//						__eds__  
//#endif
//                            int *msg, int msgval)
//{
//    Nop();Nop();Nop();
//    ID = id;
//    
//    
//    
//}
/**/
//void rxECAN1(int message)
//{
//	unsigned int ide=0;
//	unsigned int srr=0;
//	unsigned long id=0,d;
//    int *can1buf_ptr;
//			
//	/*
//	Standard Message Format: 
//	Word0 : 0bUUUx xxxx xxxx xxxx
//			     |____________|||
// 					SID10:0   SRR IDE(bit 0)     
//	Word1 : 0bUUUU xxxx xxxx xxxx
//			   	   |____________|
//						EID17:6
//	Word2 : 0bxxxx xxx0 UUU0 xxxx
//			  |_____||	     |__|
//			  EID5:0 RTR   	  DLC
//	word3-word6: data bytes
//	word7: filter hit code bits
//	
//	Substitute Remote Request Bit
//	SRR->	"0"	 Normal Message 
//			"1"  Message will request remote transmission
//	
//	Extended  Identifier Bit			
//	IDE-> 	"0"  Message will transmit standard identifier
//	   		"1"  Message will transmit extended identifier
//	
//	Remote Transmission Request Bit
//	RTR-> 	"0"  Message transmitted is a normal message
//			"1"  Message transmitted is a remote message
//	*/
//	/* read word 0 to see the message type */
//	ide=ecan1msgBuf[message][0] & 0x0001;	
//	srr=ecan1msgBuf[message][0] & 0x0002;	
//	
//	/* check to see what type of message it is */
//	/* message is standard identifier */
//	if(ide==0)
//	{
//          // count
//        //return;
//		id=(ecan1msgBuf[message][0] & 0x1FFC) >> 2;		
//	//	frame_type=CAN_FRAME_STD;
//	}
//	/* mesage is extended identifier */
//	else
//	{ 
//        d=(ecan1msgBuf[message][0] & 0x1FFC);		
//		id=d << 16;
//		d=ecan1msgBuf[message][1] & 0x0FFF;
//		id=id+(d << 6);
//		d=(ecan1msgBuf[message][2] & 0xFC00) >> 10;
//		id=id+d;		
//		 
//	}
//    
//
//	/* check to see what type of message it is */
//	/* RTR message */
//	if(srr==1)
//	{
//        // count
//        return;
////		message->message_type=CAN_MSG_RTR;	
//	}
//	/* normal message */
//	else
//	{
////        processshorttxtdmsg(id&65535,&ecan1msgBuf[message][3],ecan1msgBuf[message][3]);
//        ID = id;
//        can1buf_ptr = & ecan1msgBuf[message][3];
//        CAN1_DATA_buf[0] = (unsigned char)(*(can1buf_ptr) & 0x00FF);
//        CAN1_DATA_buf[1] = (unsigned char)((*(can1buf_ptr++) & 0xFF00) >> 8);
//        CAN1_DATA_buf[2] = (unsigned char)(*(can1buf_ptr) & 0x00FF);
//        CAN1_DATA_buf[3] = (unsigned char)((*(can1buf_ptr++) & 0xFF00) >> 8);
//        CAN1_DATA_buf[4] = (unsigned char)(*(can1buf_ptr) & 0x00FF);
//        CAN1_DATA_buf[5] = (unsigned char)((*(can1buf_ptr++) & 0xFF00) >> 8);
//        CAN1_DATA_buf[6] = (unsigned char)(*(can1buf_ptr) & 0x00FF);
//        CAN1_DATA_buf[7] = (unsigned char)((*(can1buf_ptr++) & 0xFF00) >> 8);      
//	}
//	
//}
//void Can1Tasks(void)
//{  
////   word fnrb,fbp,fnrbmask;       
////   fnrb=C1FIFObits.FNRB;
////   fbp=C1FIFObits.FBP;
////   if (fnrb<16)
////     {
////       fnrbmask=1<<fnrb;
////       if (C1RXFUL1&fnrbmask)
////         {
////           rxECAN1(fnrb);
////           C1RXFUL1&=~fnrbmask; 	    	
////         }
////     }
////   else
////     {
////       fnrbmask=1<<(fnrb-16);
////       if (C1RXFUL2&fnrbmask)
////         {
////  	       rxECAN1(fnrb);
////      	   C1RXFUL2&=~fnrbmask; 	    	    
////         }
////     }
//    
//int fnrb,fbp,fnrbmask;
//int flag;
//   // default way is in in order(FIFO) and simple (no loops)
//   flag=0;       
//   fnrb=C1FIFObits.FNRB;
//   fbp=C1FIFObits.FBP;
//   if (fnrb<16)
//     {
//       fnrbmask=1<<fnrb;
//       if (C1RXFUL1&fnrbmask)
//         {
//           rxECAN1(fnrb);
//           C1RXFUL1&=~fnrbmask;             
//         }
//       else 
//       {flag++;}
//     }
//   else
//     {
//       fnrbmask=1<<(fnrb-16);
//       if (C1RXFUL2&fnrbmask)
//         {
//             rxECAN1(fnrb);
//             C1RXFUL2&=~fnrbmask;                 
//         }
//       else 
//       {flag++;}
//     }
// if (flag)
//   {
//     // after overflow fnrb pointer is corrupt.
//     // and we must poll if flags are set.
//    if ((C1RXFUL1&0xFF00)>0) 
//      {
//         fnrb=8; // skip 8 write buffers
//         while ((fnrb<16) && (flag>0))
//          { 
//           fnrbmask=1<<fnrb;
//           if (C1RXFUL1&fnrbmask)
//             {
//               rxECAN1(fnrb);
//               C1RXFUL1&=~fnrbmask;             
//               flag--;
//             }  
//            fnrb++; 
//          }
//       }
//     if ((C1RXFUL2)>0) 
//       {
//         fnrb=0; 
//         while ((fnrb<16) && (flag>0))
//          { 
//           fnrbmask=1<<fnrb;
//           if (C1RXFUL2&fnrbmask)
//             {
//               rxECAN1(16+fnrb);
//               C1RXFUL2&=~fnrbmask;             
//               flag--;
//             }  
//            fnrb++; 
//          } 
//       } 
//  // if flag>0 here then something weird is going on.
//   } 
//    
//    
//}
/**/
void __attribute__((interrupt, no_auto_psv))_C1Interrupt(void)  
{
int fnrb,fbp,fnrbmask;
unsigned int ide=0;
unsigned int srr=0;
unsigned long id=0,d;
int *can1buf_ptr;

           
	IFS2bits.C1IF = 0;        // clear interrupt flag
	if(C1INTFbits.TBIF) { 
    	C1INTFbits.TBIF = 0;
    } 
    
    if(C1INTFbits.RBIF) {  
//	   rxmsg1++;
//        Nop();Nop();Nop();
        while (C1FIFObits.FNRB != C1FIFObits.FBP ) {
            fnrb=C1FIFObits.FNRB;
            fbp=C1FIFObits.FBP;
               if (fnrb<16) {
                   fnrbmask=1<<fnrb;
                   if (C1RXFUL1&fnrbmask) {
                        ide=ecan1msgBuf[fnrb][0] & 0x0001;	
                        srr=ecan1msgBuf[fnrb][0] & 0x0002;	
                        /* check to see what type of message it is */
                        if(ide==0) {
                            unsigned int tmp = 0;
//                            id=(ecan1msgBuf[fnrb][0] & 0x1FFC) >> 2;
                            tmp = tmp | ecan1msgBuf[fnrb][0];
                            id = ecan1msgBuf[fnrb][0];
                            id = id & 0x1FFC;
                            id = id >> 2;		
                        }
                        /* mesage is extended identifier */
                        else { 
                            d=(ecan1msgBuf[fnrb][0] & 0x1FFC); id=d << 16;
                            d=ecan1msgBuf[fnrb][1] & 0x0FFF; id=id+(d << 6);
                            d=(ecan1msgBuf[fnrb][2] & 0xFC00) >> 10; id=id+d;		
                        }
                        /* check to see what type of message it is */
                        /* RTR message */
                        if(srr != 1) {/* normal message */
//                            ID = id;
                            CAN1AUX_ID_buf[CAN1AUX_W_ptr][0] = id;
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][0] = (unsigned char)(ecan1msgBuf[fnrb][3] & 0x00FF);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][1] = (unsigned char)((ecan1msgBuf[fnrb][3] & 0xFF00) >> 8);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][2] = (unsigned char)(ecan1msgBuf[fnrb][4] & 0x00FF);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][3] = (unsigned char)((ecan1msgBuf[fnrb][4] & 0xFF00) >> 8);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][4] = (unsigned char)(ecan1msgBuf[fnrb][5] & 0x00FF);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][5] = (unsigned char)((ecan1msgBuf[fnrb][5] & 0xFF00) >> 8);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][6] = (unsigned char)(ecan1msgBuf[fnrb][6] & 0x00FF);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][7] = (unsigned char)((ecan1msgBuf[fnrb][6] & 0xFF00) >> 8);                            
                        }
                       C1RXFUL1&=~fnrbmask; 	    	
                     }
                 }
               else {
                   fnrbmask=1<<(fnrb-16);
                   if (C1RXFUL2&fnrbmask)
                     {
                        ide=ecan1msgBuf[fnrb][0] & 0x0001;	
                        srr=ecan1msgBuf[fnrb][0] & 0x0002;	
                        /* check to see what type of message it is */
                        if(ide==0) {
                            id=(ecan1msgBuf[fnrb][0] & 0x1FFC) >> 2;		
                        }
                        /* mesage is extended identifier */
                        else { 
                            d=(ecan1msgBuf[fnrb][0] & 0x1FFC); id=d << 16;
                            d=ecan1msgBuf[fnrb][1] & 0x0FFF; id=id+(d << 6);
                            d=(ecan1msgBuf[fnrb][2] & 0xFC00) >> 10; id=id+d;		
                        }
                        /* check to see what type of message it is */
                        /* RTR message */
                        if(srr != 1) {/* normal message */
//                            ID = id;
                            CAN1AUX_ID_buf[CAN1AUX_W_ptr][0] = id;
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][0] = (unsigned char)(ecan1msgBuf[fnrb][3] & 0x00FF);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][1] = (unsigned char)((ecan1msgBuf[fnrb][3] & 0xFF00) >> 8);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][2] = (unsigned char)(ecan1msgBuf[fnrb][4] & 0x00FF);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][3] = (unsigned char)((ecan1msgBuf[fnrb][4] & 0xFF00) >> 8);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][4] = (unsigned char)(ecan1msgBuf[fnrb][5] & 0x00FF);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][5] = (unsigned char)((ecan1msgBuf[fnrb][5] & 0xFF00) >> 8);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][6] = (unsigned char)(ecan1msgBuf[fnrb][6] & 0x00FF);
                            CAN1AUX_DATA_buf[CAN1AUX_W_ptr][7] = (unsigned char)((ecan1msgBuf[fnrb][6] & 0xFF00) >> 8); 

                        }
                       C1RXFUL2&=~fnrbmask; 	    	    
                     }
                 }
            CAN1AUX_W_ptr ++; CAN1AUX_W_ptr %= 25;
            if (CAN1AUX_W_ptr == CAN1AUX_R_ptr) {CAN1AUX_R_ptr++; CAN1AUX_R_ptr %= 25;}
        }
  	   C1INTFbits.RBIF = 0;
	}
    
    if (C1INTFbits.IVRIF == 1) {
       C1INTFbits.IVRIF = 0;
       Nop();Nop();Nop();
       if (C1INTFbits.TXBO == 1) {
        Nop();Nop();Nop();
        C1INTFbits.TXBO = 0;
//        CAN1_fault = 1;
       }
       if (C1INTFbits.TXBP == 1) {
        Nop();Nop();Nop();
        C1INTFbits.TXBP = 0;
       }
      
    }
       if (C1INTFbits.FIFOIF == 1) {
        Nop();Nop();Nop();
        C1INTFbits.FIFOIF = 0;
       }
       if (C1INTFbits.RBOVIF == 1) {
        Nop();Nop();Nop();
        C1INTEbits.RBOVIE = 0;
        C1INTFbits.RBOVIF = 0;
//        CAN1_fault = 1;
       }  
    
}

void __attribute__ ( (interrupt, no_auto_psv) ) _DMA2Interrupt( void )
{
    IFS1bits.DMA2IF = 0;    // Clear the DMA2 Interrupt Flag;
}

void __attribute__ ( (interrupt, no_auto_psv) ) _DMA3Interrupt( void )
{
    IFS2bits.DMA3IF = 0;    // Clear the DMA3 Interrupt Flag;
}

/*******************************************************************************
 End of File
*/