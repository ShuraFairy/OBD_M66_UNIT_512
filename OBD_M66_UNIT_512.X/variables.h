/****************************************************/
/* (С) ООО "ЛУВ", 2019                              */
/* Автор: Петров Александр  						*/
/* Дата: 13.12.2018									*/
/* Описание:                                        */
/****************************************************/

#ifndef VARIABLES_H
#define	VARIABLES_H

#include <stdint.h>

//********************************** Defines ***********************************
#define LENGTH_DATA_FMS 64
//********************************* Variables **********************************
////////////////////////////////////////////////////////////////////////////////
//uint8_t bufferFMSData[LENGTH_DATA_FMS];

struct FMSData 
{
    uint8_t vehicleWeightValue1;            // Gross Combination Vehicle Weight            
    uint8_t vehicleWeightValue2;            // Общий вес транспортного средства
    
    uint8_t vehicleSpeedValue1;             // Vehicle Speed            
    uint8_t vehicleSpeedValue2;             // Скорость автомобиля
    
    uint8_t axleWeightValue1;               // Axle Weight            
    uint8_t axleWeightValue2;               // Нагрузка на оси
    
    uint8_t engineSpeedValue1;              // Engine Speed 
    uint8_t engineSpeedValue2;              // Обороты двигателя
    
    uint8_t serviceDistanceValue1;          // Service Distance 
    uint8_t serviceDistanceValue2;          // Пробег до ТО
    
    uint8_t engineTotalHoursValue1;         // Total Engine Hours
    uint8_t engineTotalHoursValue2;         // Общее количеств моточасов
    uint8_t engineTotalHoursValue3;         //
    uint8_t engineTotalHoursValue4;         //
    
    uint8_t totalVehicleDistanceValue1;     // High Resolution Total Vehicle Distance
    uint8_t totalVehicleDistanceValue2;     // Общий пробег
    uint8_t totalVehicleDistanceValue3;     //
    uint8_t totalVehicleDistanceValue4;     //
 
    uint8_t totalFuelUsedValue1;            // Total Fuel Used
    uint8_t totalFuelUsedValue2;            // Общее израсходованное топливо
    uint8_t totalFuelUsedValue3;            //
    uint8_t totalFuelUsedValue4;            //
 
    uint8_t engineCoolantTemperatureValue;  // Engine Coolant Temperature 
                                            // Температура охлождающей жидкости
    uint8_t fuelLevelValue1;                // Fuel Level 1  
                                            // Уровень топлива
    uint8_t fuelLevelValue2;                // Fuel Level 2  
                                            // Уровень топлива
    char    VIN[18];                        // Vehicle Identification
} fmsData __attribute__ ((far));
////////////////////////////////////////////////////////////////////////////////
volatile uint32_t maskIDExtendedLow  = 0x18F00000;
volatile uint32_t maskIDExtendedHigh = 0x1FFFFFFF;
volatile uint32_t maskIDExtendedFMS  = 0x00FFFF00;
////////////////////////////////////////////////////////////////////////////////
// FMS-Stantard description - Vehicle Identification: VI
// spn 237 Vehicle Identification Number
// Vehicle Identification Number (VIN) as assigned by the vehicle manufacturer.
// NOTE The ASCII character “*” is reserved as a delimiter.
// Data Length:                     Variable - up to 200 bytes followed by an "*" delimiter
// Resolution:                      ASCII, 0 offset
// Data Range:                      0 to 255 per byte Operational Range: same as data range
// Type: Measured
// Supporting Information:
// PGN reference:                   0x00FEEC(65260)
// Rep. Rate:                       10000 ms

// Annotations:
// 1) If the Vehicle ID is up to 8 Bytes (including) then it is broadcasted with 
// PGN 00FEEC containing the vehicle ID and filled with “FF” at the unused bytes.
// 2) If the Vehicle ID contains more than 8 Bytes then a TP.CM (PGN 00EC00) with 
// a minimum of two TP.DT (PGN 00EB00) will be used.

//      Transport Protocol – Connection Management (TP.CM)
//                          0x00ECFF                                             PGN Hex
//                           60671                                              PGN
// Byte 1                | Byte 2              | Byte 3            | Byte 4       | Byte 5           | Byte 6           | Byte 7              | Byte 8 
// Control               | Total message size, | Total message size| Total number | Reserved         | Parameter Group  | Parameter Group     | Parameter Group
// byte should be filled | number of bytes     | number of bytes   | of packets   | should be        | Number of        | Number              | Number
// with (0x20)           |                     |                   |              | filled with FF16 | packeted message | of packeted message | of packeted message

//      Transport Protocol – Data Transfer (TP.DT)
//                          0x00EBFF                                             PGN Hex
//                           60415                                               PGN
// Byte 1   | Byte 2     | Byte 3     | Byte 4     | Byte 5     | Byte 6     | Byte 7     | Byte 8     |
// Sequence | Packetized | Packetized | Packetized | Packetized | Packetized | Packetized | Packetized |
// Number   | Data       | Data       | Data       | Data       | Data       | Data       | Data       |
struct VIN 
{
    uint32_t id          : 29;
    uint8_t              : 3; 
    uint8_t  can_number  : 2;
    uint8_t  filt_number : 4;		
    uint16_t             : 10;
    char     Value[18];                  
} vin __attribute__ ((far));

////////////////////////////////////////////////////////////////////////////////
// FMS-Stantard description - Combination Vehicle Weight : CVW
// ID - 
// spn 1760 Gross Combination Vehicle Weight
// The total weight of the truck and all attached trailers.
// Data Length:                     2 bytes
// Resolution:                      10 kg/bit, 0 offset
// Data Range:                      0 to 642,550 kg Operational Range: same as data range
// Type:                            Measured
// Supporting Information:
//PGN reference:                    0x00FE70(65136)
// Rep. Rate:                       10000 ms
union __attribute__ ((far))  {
    uint64_t VEHICLEWEIGHT;
    struct 
    {
        uint8_t  value1      : 8;
        uint8_t  value2      : 8;
        uint8_t  can_number  : 2;
        uint8_t  filt_number : 4;		
        uint16_t             : 10;
        uint32_t id          : 29;
        uint8_t              : 3;        
    };
} VEHICLEWEIGHTbits;
////////////////////////////////////////////////////////////////////////////////
// FMS-Stantard description - Cruise Control/Vehicle Speed: CCVS
// ID - 
// spn 84 - Wheel-Based Vehicle Speed - Speed of the vehicle as calculated from wheel or tailshaft speed.
// Data Length:                     2 bytes
// Resolution:                      1/256 km/h per bit , 0 offset
// Data Range:                      0 to 250.996 km/h
// Type:                            Measured
// Suspect Parameter Number:        84
// Parameter Group Number (PGN):    0x00FEF1(65265)
// Rep. Rate:                       100 ms
union __attribute__ ((far))  {
    uint64_t VEHICLESPEED;
    struct 
    {
        uint8_t  value1      : 8;
        uint8_t  value2      : 8;
        uint8_t  can_number  : 2;
        uint8_t  filt_number : 4;		
        uint16_t             : 10;
        uint32_t id          : 29;
        uint8_t              : 3;        
    };
} VEHICLESPEEDbits;
////////////////////////////////////////////////////////////////////////////////
// FMS-Stantard description - Vehicle Weight: WW
// ID -  - считанное с машины
// spn 582 - Axle Weight - Total mass imposed by the tires on the road surface at the specified axle.
// Data Length:                     2 bytes
// Resolution:                      0.5 kg/bit , 0 offset
// Data Range:                      0 to 32,127.5 kg
// Type:                            Measured
// Suspect Parameter Number:        582
// Parameter Group Number (PGN):    0x00FEEA(65258)
// Rep. Rate:                       1000 ms
union __attribute__ ((far))  {
    uint64_t AXLEWEIGHT;
    struct 
    {
        uint8_t  value1      : 8;
        uint8_t  value2      : 8;
        uint8_t  can_number  : 2;
        uint8_t  filt_number : 4;		
        uint16_t             : 10;
        uint32_t id          : 29;
        uint8_t              : 3;        
    };
} AXLEWEIGHTbits;
////////////////////////////////////////////////////////////////////////////////
// FMS-Stantard description - Electronic Engine Controller #1: EEC1
// ID -  - считанное с машины
// spn 190 - Engine Speed - Actual engine speed which is calculated over a minimum crankshaft angle of 720 degrees divided by
// the number of cylinders.
// Data Length:                     2 bytes
// Resolution:                      0.125 rpm/bit , 0 offset
// Data Range:                      0 to 8,031.875 rpm
// Type:                            Measured
// Suspect Parameter Number:        190
// Parameter Group Number (PGN):    0x00F004(61444)
// Rep. Rate:                       20 ms
union __attribute__ ((far))  {
    uint64_t ENGINESPEED;
    struct 
    {
        uint8_t  value1      : 8;
        uint8_t  value2      : 8;
        uint8_t  can_number  : 2;
        uint8_t  filt_number : 4;		
        uint16_t             : 10;
        uint32_t id          : 29;
        uint8_t              : 3;        
    };
} ENGINESPEEDbits;
////////////////////////////////////////////////////////////////////////////////
// FMS-Stantard description - Service Information: SERV
// ID - 0x18FEC027 - считанное с машины
// spn 914 - Service Distance - The distance which can be traveled by the vehicle before the next 
// service inspection is required. A negative distance is transmitted if the service inspection 
// has been passed. The component that requires service is identified by the
// service component identification (see SPN 911-913, 1379, and 1584).
// Data Length:                     2 bytes
// Resolution:                      5 km/bit , -160,635 km offset
// Data Range:                      -160,635 to 160,640 km
// Type:                            Measured
// Suspect Parameter Number:        914
// Parameter Group Number (PGN):    0x00FEC0 (65216)
// Rep. Rate:                       1000 ms
union __attribute__ ((far))  {
    uint64_t SERVICEDISTANCE;
    struct 
    {
        uint8_t  value1      : 8;
        uint8_t  value2      : 8;
        uint8_t  can_number  : 2;
        uint8_t  filt_number : 4;		
        uint16_t             : 10;
        uint32_t id          : 29;
        uint8_t              : 3;        
    };
} SERVICEDISTANCEbits;
////////////////////////////////////////////////////////////////////////////////
// FMS-Stantard description - Engine Temperature 1: ET1
// ID - 0x18FEEE00 - считанное с машины
// spn 110 - Engine Coolant Temperature - Temperature of liquid found in engine cooling system.
// Data Length:                     1 byte
// Resolution:                      1 deg C/bit , -40 deg C offset
// Data Range:                      -40 to 210 deg C
// Type:                            Measured
// Suspect Parameter Number:        110
// Parameter Group Number (PGN):    0x00FEEE (65262)
// Rep. Rate:                       1000 ms
union __attribute__ ((far))  {
    uint64_t EGINECOOLANTTEMPERATURE;
    struct 
    {
        uint8_t  value       : 8;
        uint8_t  can_number  : 2;
        uint8_t  filt_number : 4;		
        uint8_t              : 2;
        uint32_t id          : 29;
        uint8_t              : 3;
        uint16_t             : 16;              
    };
} EGINECOOLANTTEMPERATUREbits;
////////////////////////////////////////////////////////////////////////////////
// FMS-Stantard description - Engine Hours, Revolutions: HOURS
// ID - 0x18FEE527 - считанное с машины
// spn 247 - Total Engine Hours - Accumulated time of operation of engine.
// Data Length:                     4 bytes
// Resolution:                      0.05 hr/bit , 0 offset
// Data Range:                      0 to 210,554,060.75 hr
// Type:                            Measured
// Suspect Parameter Number:        247
// Parameter Group Number (PGN):    0x00FEE5 (65253)
// Rep. Rate:                       1000 ms
union __attribute__ ((far))  {
    uint64_t ENGINETOTALHOURSCONF;
    struct 
    {
        uint8_t  can_number  : 2;
        uint8_t  filt_number : 4;
        uint16_t             : 10;
        uint32_t id          : 29;
        uint8_t              : 3;
        uint16_t             : 16;              
    };
} ENGINETOTALHOURSCONFbits;
union __attribute__ ((far))  {
    uint32_t ENGINETOTALHOURS;
    struct 
    {        
		uint8_t  value1      : 8;
        uint8_t  value2      : 8;
        uint8_t  value3      : 8;
        uint8_t  value4      : 8;                    
    };
} ENGINETOTALHOURSbits;
////////////////////////////////////////////////////////////////////////////////
// FMS-Stantard description - High Resolution Vehicle Distance: VHDR
// ID - 0x18FEC1EE - считанное с машины
// spn 917 - High Resolution Total Vehicle Distance - Accumulated distance traveled by the vehicle during its
// operation. NOTE - See SPN 245 for alternate resolution.
// Data Length:                     4 bytes
// Resolution:                      5 m/bit , 0 offset
// Data Range:                      0 to 21,055,406 km
// Type:                            Measured
// Suspect Parameter Number:        917
// Parameter Group Number (PGN):    0x00FEC1 (65217)
// Rep. Rate:                       1000 ms
union __attribute__ ((far))  {
    uint64_t TOTALVEHICLEDISTANCECONF;
    struct 
    {
        uint8_t  can_number  : 2;
        uint8_t  filt_number : 4;
        uint16_t             : 10;
        uint32_t id          : 29;
        uint8_t              : 3;
        uint16_t             : 16;              
    };
} TOTALVEHICLEDISTANCECONFbits;
union __attribute__ ((far))  {
    uint32_t TOTALVEHICLEDISTANCE;
    struct 
    {        
		uint8_t  value1      : 8;
        uint8_t  value2      : 8;
        uint8_t  value3      : 8;
        uint8_t  value4      : 8;                    
    };
} TOTALVEHICLEDISTANCEbits;
////////////////////////////////////////////////////////////////////////////////
// FMS-Stantard description - Fuel Consumption: LFC
// ID - 0x18FEE927 - считанное с машины
// spn 250 - Total Fuel Used - Accumulated amount of fuel used during vehicle operation.
// Data Length:                     4 bytes
// Resolution:                      0.5 L/bit , 0 offset
// Data Range:                      0 to 2,105,540,607.5 L
// Type:                            Measured
// Suspect Parameter Number:        250
// Parameter Group Number (PGN):    0x00FEE9(65257)
// Rep. Rate:                       1000 ms
union __attribute__ ((far))  {
    uint64_t TOTALFUELUSEDCONF;
    struct 
    {
        uint8_t  can_number  : 2;
        uint8_t  filt_number : 4;
        uint16_t             : 10;
        uint32_t id          : 29;
        uint8_t              : 3;
        uint16_t             : 16;              
    };
} TOTALFUELUSEDCONFbits;
union __attribute__ ((far))  {
    uint32_t TOTALFUELUSED;
    struct 
    {        
		uint8_t  value1      : 8;
        uint8_t  value2      : 8;
        uint8_t  value3      : 8;
        uint8_t  value4      : 8;                    
    };
} TOTALFUELUSEDbits;
////////////////////////////////////////////////////////////////////////////////
// FMS-Stantard description - Dash Display1: DD1
// ID - 0x18FEFC21 - считанное с машины
// spn 96 - Fuel Level 1 - Ratio of volume of fuel to the total volume of fuel storage container.
// Data Length:                     1 byte
// Resolution:                      0.4 %/bit , 0 offset
// Data Range:                      0 to 100 %
// Type:                            Measured
// Suspect Parameter Number:        96
// Parameter Group Number (PGN):    0x00FEFC (65276)
// Rep. Rate:                       1000 ms

// spn 38 Fuel Level 2
// Ratio of volume of fuel to the total volume of fuel in the second or right-side storage container. 
// When Fuel Level 2 is not used, Fuel Level 1 (SPN 96) represents the total fuel in all fuel storage containers.
// Data Length:                     1 byte
// Resolution:                      0.4 %/bit, 0 offset
// Data Range:                      0 to 100 % Operational Range: same as data range
// Type:                            Measured
// Supporting Information:
// PGN reference:                   0x00FEFC(65276)
union __attribute__ ((far))  {
    uint64_t FUELLEVEL;
    struct 
    {
        uint8_t  value1      : 8;
        uint8_t  value2      : 8;
        uint8_t  can_number  : 2;
        uint8_t  filt_number : 4;        
        uint16_t             : 10;      
        uint32_t id          : 29;        
        uint8_t              : 3;                     
    };
} FUELLEVELbits;
////////////////////////////////////////////////////////////////////////////////
union __attribute__ ((far))  {
    unsigned long long CANTESTPRMS;
    struct 
    {
		unsigned char can_number    : 2;
        unsigned char               : 2;
        unsigned char filt_number   : 4;
        unsigned char bit_mask      : 8;        
        unsigned char value1        : 8;
        unsigned char value2        : 8;
        unsigned long id            : 29;
        unsigned char byte          : 3;        
    };
} CANTESTPRMSbits;    
////////////////////////////////////////////////////////////////////////////////
//union __attribute__ ((far))  {
//    unsigned long long CANBUTTONPRMS;
//   struct 
//    {
//		unsigned char can_number:2;
//        unsigned char :2;
//        unsigned char filt_number:4;
//        unsigned char bit_mask:8;        
//        unsigned char value1:8;
//        unsigned char value2:8;
//        unsigned long id:29;
//        unsigned char byte:3;        
//    };
//} CANBUTTONPRMSbits;
//****************************** Functions prototype ***************************
void initTruckVariables(void);




#endif	/* VARIABLES_H */

