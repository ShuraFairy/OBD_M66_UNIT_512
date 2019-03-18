/*******************************************************************************
  ECAN Configuration source file

  Company:
    Microchip Technology Inc.

  File Name:
    ecan2_config.c

  Summary:
    Initializes and configures the ECAN1 and DMA modules.

  Description:
    This source file initializes the DMA and configures two DMA channels one
    each for transmission and reception. The ECAN is also initialized, its clock
    configured to be the system clock itself and the filters are also configured
    to accept a particular message.
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
#include "ecan2_config.h"

extern volatile int rxmsg2;
extern volatile unsigned int CAN2AUX_R_ptr, CAN2AUX_W_ptr;
extern unsigned long ID2;
extern volatile unsigned int CAN2_fault;
extern unsigned int Fp2;

extern unsigned char __attribute__ ((far)) CAN2_DATA_buf [8];
extern volatile unsigned char __attribute__ ((far)) CAN2AUX_DATA_buf[25][8];
extern volatile unsigned long __attribute__ ((far)) CAN2AUX_ID_buf[25][1];

extern union __attribute__ ((far))  {
    unsigned int CAN2SETTINGS;
   struct 
    {
		unsigned char on:1;
        unsigned char ide:1;
        unsigned int  bitrate:10;
        unsigned int :4;
    };
} CAN2SETTINGSbits;
extern union __attribute__ ((far))  {
    unsigned long CAN2CFG;
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
        unsigned char          :4;        
    };
} CAN2CFGbits;

/* Dma Initialization for ECAN1 Transmission */
void dma5init(void){
     #if defined(__dsPIC33E__)
	 DMAPWC=0; DMARQC=0;
     #else
     DMACS0=0;
     #endif
     DMA1CON=0x2020;
	 DMA1PAD=0x0542;	/* ECAN 2 (C2TXD) */
 	 DMA1CNT=0x0007;
	 DMA1REQ=0x0047;	/* ECAN 2 Transmit */
     
    #ifdef _HAS_DMA_
    DMA1STAL = __builtin_dmaoffset( ecan2msgBuf );
    DMA1STAH = __builtin_dmapage( ecan2msgBuf );
    #else
    DMA1STAL = (uint16_t)(int_least24_t)(&ecan2msgBuf);
    DMA1STAH = 0;
    #endif

	 DMA1CONbits.CHEN=1;
}

/* Dma Initialization for ECAN1 Reception */
void dma4init(void){

	 #if defined(__dsPIC33E__)
	 DMAPWC=0; DMARQC=0;
     #else
     DMACS0=0;
     #endif
     DMA0CON=0x0020;
	 DMA0PAD=0x0540;	/* ECAN 2 (C2RXD) */
 	 DMA0CNT=0x0007;
	 DMA0REQ=0x0037;	/* ECAN 2 Receive */
     
    #ifdef _HAS_DMA_
    DMA1STAL = __builtin_dmaoffset( ecan2msgBuf );
    DMA1STAH = __builtin_dmapage( ecan2msgBuf );
    #else
    DMA0STAL = (uint16_t)(int_least24_t)(&ecan2msgBuf);
    DMA0STAH = 0;
    #endif

	 DMA0CONbits.CHEN=1;	 
}

void ecan2ClkInit(void){

/* FCAN is selected to be FCY*/
/* FCAN = FCY = 40MHz */
	C2CTRL1bits.CANCKS = CANCKSVAL;

    if (CAN2SETTINGSbits.bitrate == 500) C2CFG1bits.BRP = 0;
    if (CAN2SETTINGSbits.bitrate == 250) C2CFG1bits.BRP = 1;
    if (CAN2SETTINGSbits.bitrate == 125) C2CFG1bits.BRP = 3;
    if (CAN2SETTINGSbits.bitrate == 100) C2CFG1bits.BRP = 4;
    if (CAN2SETTINGSbits.bitrate == 83)  C2CFG1bits.BRP = 5;
    if (CAN2SETTINGSbits.bitrate == 62)  C2CFG1bits.BRP = 7;    
    if (CAN2SETTINGSbits.bitrate == 50)  C2CFG1bits.BRP = 9;
    if (CAN2SETTINGSbits.bitrate == 33)  C2CFG1bits.BRP = 14;
/*
Bit Time = (Sync Segment + Propagation Delay + Phase Segment 1 + Phase Segment 2)=20*TQ
Phase Segment 1 = 8TQ
Phase Segment 2 = 6Tq
Propagation Delay = 5Tq
Sync Segment = 1TQ
CiCFG1<BRP> =(FCAN /(2 ×N×FBAUD))– 1
*/

	/* Synchronization Jump Width set to 4 TQ */
//	C2CFG1bits.SJW = 0x3;
	/* Baud Rate Prescaler */
//	C2CFG1bits.BRP = BRP_VAL;
// Äëÿ òàêòîâîé 10 ÌÃö
    if (Fp2 == 10) {
        /* Synchronization Jump Width set to 4 TQ */
        C2CFG1bits.SJW    = CAN2CFGbits.sjw_slp; //0x3;        
        /* Phase Segment 1 time is 4 TQ */
        C2CFG2bits.SEG1PH = CAN2CFGbits.seg1_slp;// 0x3;                                        // 4 TQ
        /* Phase Segment 2 time is set to be programmable */
        C2CFG2bits.SEG2PHTS = 0x1; // freely programmable
        /* Phase Segment 2 time is 3 TQ */
        C2CFG2bits.SEG2PH = CAN2CFGbits.seg2_slp; //0x2;                                      // 3 TQ
        /* Propagation Segment time is 3 TQ */
        C2CFG2bits.PRSEG  = CAN2CFGbits.prseg_slp; //0x1;                                       // 2 TQ
        /* Bus line is sampled three times at the sample point */
        C2CFG2bits.SAM    = CAN2CFGbits.sam_slp; // 0x1;	
    }
    else { // Äëÿ òàêòîâîé 20 ÌÃö
        /* Synchronization Jump Width set to 4 TQ */
        C2CFG1bits.SJW    = CAN2CFGbits.sjw;//0x3;        
        /* Phase Segment 1 time is 8 TQ */
        C2CFG2bits.SEG1PH = CAN2CFGbits.seg1;//0x7;                                        // 8 TQ
        /* Phase Segment 2 time is set to be programmable */
        C2CFG2bits.SEG2PHTS = 0x1; // freely programmable
        /* Phase Segment 2 time is 6 TQ */
        C2CFG2bits.SEG2PH = CAN2CFGbits.seg2;//0x5;                                      // 6 TQ
        /* Propagation Segment time is 5 TQ */
        C2CFG2bits.PRSEG  = CAN2CFGbits.prseg;//0x4;                                       // 5 TQ
        /* Bus line is sampled three times at the sample point */
        C2CFG2bits.SAM    = CAN2CFGbits.sam;//0x1;        
    }    
//    if (Fp2 == 10) {
//        /* Phase Segment 1 time is 4 TQ */
//        C2CFG2bits.SEG1PH=0x3;                                        // 4 TQ
//        /* Phase Segment 2 time is set to be programmable */
//        C2CFG2bits.SEG2PHTS = 0x1; // freely programmable
//        /* Phase Segment 2 time is 3 TQ */
//        C2CFG2bits.SEG2PH = 0x2;                                      // 3 TQ
//        /* Propagation Segment time is 3 TQ */
//        C2CFG2bits.PRSEG = 0x1;                                       // 2 TQ
//        /* Bus line is sampled three times at the sample point */
//        C2CFG2bits.SAM = 0x1;	
//    }
//    else { // Äëÿ òàêòîâîé 20 ÌÃö    
//        /* Phase Segment 1 time is 8 TQ */
//        C2CFG2bits.SEG1PH=0x7;
//        /* Phase Segment 2 time is set to be programmable */
//        C2CFG2bits.SEG2PHTS = 0x1;
//        /* Phase Segment 2 time is 6 TQ */
//        C2CFG2bits.SEG2PH = 0x5;
//        /* Propagation Segment time is 5 TQ */
//        C2CFG2bits.PRSEG = 0x4;
//        /* Bus line is sampled three times at the sample point */
//        C2CFG2bits.SAM = 0x1;
//    }
	
}

void ecan2Init(void){

/* Request Configuration Mode */
	C2CTRL1bits.REQOP=4;
	while(C2CTRL1bits.OPMODE!=4);

	ecan2ClkInit();	

	C2FCTRLbits.FSA=0b01000;		/* 0b01000 FIFO Starts at Message Buffer 8 */
	C2FCTRLbits.DMABS=0b110;		/* 0b110 32 CAN Message Buffers in DMA RAM */
	
/*	Filter Configuration

	ecan1WriteRxAcptFilter(int n, long identifier, unsigned int exide,unsigned int bufPnt,unsigned int maskSel)

	n = 0 to 15 -> Filter number/

	identifier -> SID <10:0> : EID <17:0> 

	exide = 0 -> Match messages with standard identifier addresses 
	exide = 1 -> Match messages with extended identifier addresses 

	bufPnt = 0 to 14  -> RX Buffer 0 to 14
	bufPnt = 15 -> RX FIFO Buffer

	maskSel = 0	->	Acceptance Mask 0 register contains mask
	maskSel = 1	->	Acceptance Mask 1 register contains mask
	maskSel = 2	->	Acceptance Mask 2 register contains mask
	maskSel = 3	->	No Mask Selection
	
*/
    
  

/*	Mask Configuration

	ecan1WriteRxAcptMask(int m, long identifierMask, unsigned int mide, unsigned int exide)

	m = 0 to 2 -> Mask Number

	identifier -> SID <10:0> : EID <17:0> 

	mide = 0 -> Match either standard or extended address message if filters match 
	mide = 1 -> Match only message types that correpond to 'exide' bit in filter

	exide = 0 -> Match messages with standard identifier addresses 
	exide = 1 -> Match messages with extended identifier addresses 
	
*/
Nop();Nop();Nop();

/* Enter Normal Mode */
	C2CTRL1bits.REQOP=0;
	while(C2CTRL1bits.OPMODE!=0);
	
/* ECAN transmit/receive message control */

	C2RXFUL1=C2RXFUL2=C2RXOVF1=C2RXOVF2=0x0000;
	C2TR01CONbits.TXEN0=1;			/* ECAN1, Buffer 0 is a Transmit Buffer */
	C2TR01CONbits.TXEN1=1;			/* ECAN1, Buffer 0 is a Transmit Buffer */
	C2TR23CONbits.TXEN2=1;			/* ECAN1, Buffer 0 is a Transmit Buffer */
	C2TR23CONbits.TXEN3=1;			/* ECAN1, Buffer 0 is a Transmit Buffer */
	C2TR45CONbits.TXEN4=1;			/* ECAN1, Buffer 0 is a Transmit Buffer */
	C2TR45CONbits.TXEN5=1;			/* ECAN1, Buffer 0 is a Transmit Buffer */
	C2TR67CONbits.TXEN6=1;			/* ECAN1, Buffer 0 is a Transmit Buffer */
	C2TR67CONbits.TXEN7=1;			/* ECAN1, Buffer 0 is a Transmit Buffer */


	C2TR01CONbits.TX0PRI=0b00; 		/* Message Buffer 0 Priority Level */
	C2TR01CONbits.TX1PRI=0b00; 		/* Message Buffer 0 Priority Level */
	C2TR23CONbits.TX2PRI=0b01; 		/* Message Buffer 0 Priority Level */
	C2TR23CONbits.TX3PRI=0b01; 		/* Message Buffer 0 Priority Level */
	C2TR45CONbits.TX4PRI=0b10; 		/* Message Buffer 0 Priority Level */
	C2TR45CONbits.TX5PRI=0b10; 		/* Message Buffer 0 Priority Level */
	C2TR67CONbits.TX6PRI=0b11; 		/* Message Buffer 0 Priority Level */
	C2TR67CONbits.TX7PRI=0b11; 		/* Message Buffer 0 Priority Level */

}

extern void can2start(void)
{
//ìîå    
    C2FEN1 = 0;
    
    C2INTEbits.RBIE = 0;
    rxmsg2 = 0; 
    
	ecan2Init();
	dma4init();	
	dma5init();
    MSTRPR = 0x0020;
	IEC3bits.C2IE = 1;
    
    C2INTF = 0;
	C2INTEbits.TBIE = 1;	
	C2INTEbits.RBIE = 1;
    
//    C2INTEbits.IVRIE  = 1;
//    C2INTEbits.FIFOIE = 1;
//    C2INTEbits.RBOVIE = 1; 
    
} 
