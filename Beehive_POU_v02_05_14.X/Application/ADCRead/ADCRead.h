/*
================================================================================
File name:    ADCRead.h
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  This class is responsible for reading all the 4 analog input channels one by
  one periodically. It will read the converted data and process it.

Class Methods:
  void ADCRead(void);
    Call periodically from Scheduler (125msec), to trigger the ADC conversion of
    one channel. And after that, Check the completion status and read the
    converted data and checks the minimum maximum ranges for thermistors digital
    data, Moisture detectors digital value and power supply voltage also.

Method Calling Requirements:
  adcRead.ReadFunction() should be called once per 1 second in scheduler.

Resources:
  8 ADC channel inputs

IoTranslate requirements:
  The following #defines are used in ADC read operation.

  #define ADCREAD_CHANNEL_SELECT(ChannelIndex)
  #define ADCREAD_START_SAMPLING()
  #define ADCREAD_CONVERSION_COMPLETE()
  #define ADCREAD_READ_REGISTER()

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                            Poorana kumar G
1.1.0  02-04-2020  As per the Beta requirement changes      Poorana kumar G
                   updated the functions
2.2.0  07-16-2020  Macro to disable the inlet temperature   Poorana kumar G
                   ADC conversion is added.
--------------------------------------------------------------------------------
*/

#ifndef _ADCREAD_H
#define	_ADCREAD_H


#include "Build.h"
#include "IoTranslate.h"
#include "FaultIndication.h"
#include "Scheduler.h"


// Enums for ADC conversion status
typedef enum {
  ADC_CONV_SH_DISCHARGE_START = 0,
  ADC_CONV_SH_DISCHARGE_END,
  ADC_CONV_DUMMY_READ,
  ADC_CONV_SAMPLING_START,
  ADC_CONV_SAMPLING_END,
  ADC_CONV_READ_DATA
}ADCState_ETYP;


//CLASS OBJECT DEFINITION
typedef struct {
//  Public Variables
  struct {
    uint8_t thermistor1DetectedFLG:1;
    uint8_t thermistor2DetectedFLG:1;
    uint8_t thermistor3DetectedFLG:1;
    uint8_t thermistor4DetectedFLG:1;
    uint8_t validThermistorsFLG:1;
  }flags;
  uint16_t adcDataARYW[TOTAL_ADC_CHANNELS];
  uint16_t adcDataFilterARYW[TOTAL_ADC_CHANNELS];

// Public Methods
  bool (*ReadFunction)(void);           // The read function used periodically
  void (*DetectThermistor)(void);       // 

// Private Variables
  uint8_t adcChannelIndex;              // Index into ADC channel
  uint8_t connectThermistor;            // Number of Connected Thermistor
  uint8_t powerONADCDetTimer;           // Timer to delay the detection of ADC
  ADCState_ETYP adcStatus;              // ADC Conversion status
} ADCRead_STYP;


// DEFINE CLASS OBJECT DEFAULTS
#ifdef DISABLE_INLET_THERMISTOR

#define ADC_READ_DEFAULTS {                                 \
                            {0,0,0,0},                      \
                            {0,0,0,0,0,0,0,0},              \
                            {0,0,0,0,0,0,0,0},              \
                            &ADCRead,                       \
                            &chamberThermistorDectection,   \
                            OUTLET_TEMPERATURE,             \
                            0,                              \
                            POWERON_ADC_DETECTION_TIME,     \
                            ADC_CONV_SH_DISCHARGE_START     \
                          }

#else

#define ADC_READ_DEFAULTS {                                 \
                            {0,0,0,0},                      \
                            {0,0,0,0,0,0,0,0},              \
                            {0,0,0,0,0,0,0,0},              \
                            &ADCRead,                       \
                            &chamberThermistorDectection,   \
                            INLET_TEMPERATURE,              \
                            POWERON_ADC_DETECTION_TIME,     \
                            ADC_CONV_SH_DISCHARGE_START     \
                          }
#endif


// OTHER DEFINITIONS
// Equivalent ADC count for 32°F or 0°C
#define THERMISTOR_OPEN_ADC_COUNT               900
// 10% less from (4096 * 2) = 0.5 V
#define THERMISTOR_SHORT_ADC_COUNT              7506        // values changed because mike Jan Updates require short count to be at 205F but actual was 200F
                                                            // 7372 = 200F, 7656 = 210F, 7506 = 205F
// (4096 - ADC count for 1.5V) * 2
#define THERMISTOR_ANALOG_PIN_OPEN_THRESHOLD    5734

#define MOISTURE_DETECTOR_LEAK_COUNT        1966    //(4096 * 48 / 100)
#define MOISTURE_DETECTOR_UPPER_THRESHOLD   2458    //(4096 * 60 / 100)
#define POWER_SUPPLY_ADC_COUNT              2048    // Count for 2.5V
#define POWER_SUPPLY_TOLERANCE              5       // 5%
#define POWER_SUPPLY_MIN_VOLTAGE            4500    // in mVolt
#define ADC_FILTER_SHIFTS                   2       // 2^2 = 4 count avg
#define POWERON_ADC_DETECTION_TIME          4       // 4 * 0.5s = 2 Sec
//#define POWERON_ADC_DETECTION_TIME          20      // 20 * 0.1s = 2 Sec
#define MINIMUM_THERMISOR_COUNTS            3

// ADC Array index 
#define INLET_TEMPERATURE                   0
#define OUTLET_TEMPERATURE                  1
#define CHAMBER_TEMPERATURE1                2
#define CHAMBER_TEMPERATURE2                3
#define CHAMBER_TEMPERATURE3                4
#define CHAMBER_TEMPERATURE4                5
#define MOISTURE_DETECTOR                   6
#define VCC_VOLTAGE                         7

//  CLASS METHOD PROTOTYPES
void chamberThermistorDectection(void);
bool ADCRead(void);


// EXTERN VARIABLES
extern ADCRead_STYP adcRead;

#endif /* _ADCREAD_H */

