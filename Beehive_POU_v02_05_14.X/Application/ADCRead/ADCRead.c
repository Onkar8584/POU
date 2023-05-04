/*
================================================================================
File name:    ADCRead.c
                    
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

  The following functions are used in ADC read operation.
  ADCReadTriggerDischarge()
  ADCReadStopDischarge()
  readADCIOStatus()
  
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                            Poorana kumar G
       11-04-2019  PC-Lint warning is cleared by adding     Poorana kumar G
                   "static" in "LowPassFilter" function.
1.1.0  02-04-2020  As per the Beta requirement changes      Poorana kumar G
                   updated the functions
2.2.0  07-16-2020  Macro to disable the inlet temperature   Poorana kumar G
                   ADC conversion is added.
2.3.0  09-15-2020  Averaging is made common for all the     Poorana kumar G
                   analog channels 
--------------------------------------------------------------------------------
*/

#include "ADCRead.h"

// List of channels to be converted
static const ADC1_CHANNEL ADCReadChannels[] =
{
  ADC1_INLET_TEMPERATURE_ADC, 
  ADC1_OUTLET_TEMPERATURE_ADC,
  ADC1_CHAMBER_TEMPERATURE1,
  ADC1_CHAMBER_TEMPERATURE2,
  ADC1_CHAMBER_TEMPERATURE3,
  ADC1_CHAMBER_TEMPERATURE4,
  ADC1_MOISTURE_DETECT_ADC,   
  ADC1_VCC_VOLTAGE_ADC,       
};

// Thermistor open errors list
static const Errors_ETYP thermOpenErrors[] =
{
  THERMISTOR1_OPEN_ERROR,
  THERMISTOR2_OPEN_ERROR,
  THERMISTOR3_OPEN_ERROR,
  THERMISTOR4_OPEN_ERROR,
  THERMISTOR5_OPEN_ERROR,
  THERMISTOR6_OPEN_ERROR
};

// Thermistor short errors list
static const Errors_ETYP thermShortErrors[] =
{
  THERMISTOR1_SHORT_ERROR,
  THERMISTOR2_SHORT_ERROR,
  THERMISTOR3_SHORT_ERROR,
  THERMISTOR4_SHORT_ERROR,
  THERMISTOR5_SHORT_ERROR,
  THERMISTOR6_SHORT_ERROR,
};

#define DebounceIterations 7

 uint16_t debug_PowerADCLocalW = 0;
 uint16_t PowerADCLocalW = 0;
 
extern uint16_t Timer_1ms4;
extern uint8_t flag_1msTimer4;

uint8_t flag_once4 = 0, debounce_call4 = 0;

uint8_t Flag_Error_Leak = 0;

/*
================================================================================
Method name:  LowPassFilter
                    
Originator:   Poorana kumar G

Description: 
  The purpose of this function is used to do the low pass filter operation on
  ADC's output.

  This is an inline function and this method should be called using
  LowPassFilter().

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-14-2019  Initial Write                        Poorana kumar G
       11-04-2019  PC-Lint warning is cleared by        Poorana kumar G
                   adding "static".
--------------------------------------------------------------------------------
*/

static inline uint16_t LowPassFilter(uint16_t *filterRegPTRW, uint16_t rawDataW)
{
  if ( *filterRegPTRW != 0) {
    // Allow the 1/4 of the change in ADC count.
    (*filterRegPTRW) -= (*filterRegPTRW >> ADC_FILTER_SHIFTS);
    (*filterRegPTRW) += rawDataW;
  }
  else {
    // Initialize the filter register.
    (*filterRegPTRW) = (rawDataW << ADC_FILTER_SHIFTS);
  }

  return ((*filterRegPTRW) >> ADC_FILTER_SHIFTS);
}


/*
================================================================================
Method name:  checkThermistorError
                    
Originator:   Poorana kumar G

Description: 
  The purpose of this function is to detect the open & short errors of the
  thermistors based on the ADC output.

  This method should be called using checkThermistorError().

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       05-12-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

static void checkThermistorError(uint8_t thermistorNum)
{
  if ( thermistorNum <= CHAMBER_TEMPERATURE4) {
    uint16_t adcCount = adcRead.adcDataARYW[thermistorNum];
    bool ioStatus = 0;

    // Thermistor ADC count error check
    if ( adcCount < THERMISTOR_OPEN_ADC_COUNT) {
      // Declare thermistor open error
      faultIndication.Error(thermOpenErrors[thermistorNum]);
    }
    else if(adcCount > THERMISTOR_ANALOG_PIN_OPEN_THRESHOLD) {
      // Read the GPIO status
      ioStatus = readADCIOStatus(thermistorNum);

      if ( ioStatus == true) {
        // Declare thermistor open error
        faultIndication.Error(thermOpenErrors[thermistorNum]);
      }
      else {
        // Clear thermistor open error
        faultIndication.Clear(thermOpenErrors[thermistorNum]);
      }
    }
    else {
      // Clear thermistor open error
      faultIndication.Clear(thermOpenErrors[thermistorNum]);
    }

    if ( (adcCount > THERMISTOR_SHORT_ADC_COUNT) && (ioStatus == false)) {
      // If really short, declare thermistor short error
      faultIndication.Error(thermShortErrors[thermistorNum]);
    }
    else {
      // Clear short error
      faultIndication.Clear(thermShortErrors[thermistorNum]);
    }
  }
}


/*
================================================================================
Method name:  checkVccError
                    
Originator:   Poorana kumar G

Description: 
  The purpose of this function is to detect the error in the DC supply of the
  controller based on the ADC output.

  This method should be called using checkVccError().

Resources:
  None
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       05-12-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

static void checkVccError(void)
{
//  uint16_t PowerADCLocalW = 0;

//  debug_PowerADCLocalW = adcRead.adcDataARYW[VCC_VOLTAGE];
  
//  debug_PowerADCLocalW = PowerADCLocalW;
  
  // Check power supply error
  PowerADCLocalW = POWER_SUPPLY_ADC_COUNT +                             \
          ((POWER_SUPPLY_ADC_COUNT * POWER_SUPPLY_TOLERANCE) / 100);
        
  if ( adcRead.adcDataARYW[VCC_VOLTAGE] > PowerADCLocalW) {
    // Declare power supply error
    faultIndication.Error(DC_SUPPLY_ERROR);
  }
  else {
    PowerADCLocalW = POWER_SUPPLY_ADC_COUNT -                             \
            ((POWER_SUPPLY_ADC_COUNT * POWER_SUPPLY_TOLERANCE) / 100);

    if ( adcRead.adcDataARYW[VCC_VOLTAGE] < PowerADCLocalW) {
      // Declare power supply error
      faultIndication.Error(DC_SUPPLY_ERROR);
    }
    else {
      PowerADCLocalW = ((uint32_t) adcRead.adcDataARYW[VCC_VOLTAGE] * 2 * \
              ADC_REF_VOLTAGE) / ADC_FULL_COUNT;

      if ( PowerADCLocalW < POWER_SUPPLY_MIN_VOLTAGE) {
        // Declare power supply error
        faultIndication.Error(DC_SUPPLY_ERROR);
      }
      else {
        faultIndication.Clear(DC_SUPPLY_ERROR);
      }
    }
  }
}


/*
================================================================================
Method name:  chamberThermistorDectection
                    
Originator:   Poorana kumar G

Description: 
  The purpose of this function is used to detect the inserted chamber thermistors
  during power up .

  This method should be called using adcRead.DetectThermistor().

Resources:
  readADCIOStatus() from IoTranslate
================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       02-04-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

void chamberThermistorDectection(void)
{
//    Reset the counter for measuring the connected themistors
    adcRead.connectThermistor = 0;
    // Check that chamber temperature 1 ADC count is in range
  if ( (adcRead.adcDataARYW[CHAMBER_TEMPERATURE1] < THERMISTOR_SHORT_ADC_COUNT) && \
          (adcRead.adcDataARYW[CHAMBER_TEMPERATURE1] > THERMISTOR_OPEN_ADC_COUNT)) {
    adcRead.flags.thermistor1DetectedFLG = true;
    adcRead.connectThermistor++;
  }
  else if (adcRead.adcDataARYW[CHAMBER_TEMPERATURE1] >= THERMISTOR_SHORT_ADC_COUNT) {
    if ( readADCIOStatus(CHAMBER_TEMPERATURE1) == false) {
      // If IO is high means analog pin is open. Enable to declare thermistor error
      adcRead.flags.thermistor1DetectedFLG = true;
      adcRead.connectThermistor++;
    }
  }
  else {
    // Nothing To Do
  }

  // Check that chamber temperature 2 ADC count is in range
  if ( (adcRead.adcDataARYW[CHAMBER_TEMPERATURE2] < THERMISTOR_SHORT_ADC_COUNT) && \
          (adcRead.adcDataARYW[CHAMBER_TEMPERATURE2] > THERMISTOR_OPEN_ADC_COUNT)) {
    adcRead.flags.thermistor2DetectedFLG = true;
    adcRead.connectThermistor++;
  }
  else if (adcRead.adcDataARYW[CHAMBER_TEMPERATURE2] >= THERMISTOR_SHORT_ADC_COUNT) {
    if ( readADCIOStatus(CHAMBER_TEMPERATURE2) == false) {
      // If IO is high means analog pin is open. Enable to declare thermistor error
      adcRead.flags.thermistor2DetectedFLG = true;
      adcRead.connectThermistor++;
    }
  }
  else {
    // Nothing To Do
  }

  // Check that chamber temperature 3 ADC count is in range
  if ( (adcRead.adcDataARYW[CHAMBER_TEMPERATURE3] < THERMISTOR_SHORT_ADC_COUNT) && \
          (adcRead.adcDataARYW[CHAMBER_TEMPERATURE3] > THERMISTOR_OPEN_ADC_COUNT)) {
    adcRead.flags.thermistor3DetectedFLG = true;
    adcRead.connectThermistor++;
  }
  else if (adcRead.adcDataARYW[CHAMBER_TEMPERATURE3] >= THERMISTOR_SHORT_ADC_COUNT) {
    if ( readADCIOStatus(CHAMBER_TEMPERATURE3) == false) {
      // If IO is high means analog pin is open. Enable to declare thermistor error
      adcRead.flags.thermistor3DetectedFLG = true;
      adcRead.connectThermistor++;
    }
  }
  else {
    // Nothing To Do
  }

  // Check that chamber temperature 4 ADC count is in range
  if ( (adcRead.adcDataARYW[CHAMBER_TEMPERATURE4] < THERMISTOR_SHORT_ADC_COUNT) && \
          (adcRead.adcDataARYW[CHAMBER_TEMPERATURE4] > THERMISTOR_OPEN_ADC_COUNT)) {
    adcRead.flags.thermistor4DetectedFLG = true;
    adcRead.connectThermistor++;
  }
  else if (adcRead.adcDataARYW[CHAMBER_TEMPERATURE4] >= THERMISTOR_SHORT_ADC_COUNT) {
    if ( readADCIOStatus(CHAMBER_TEMPERATURE4) == false) {
      // If IO is high means analog pin is open. Enable to declare thermistor error
      adcRead.flags.thermistor4DetectedFLG = true;
      adcRead.connectThermistor++;
    }
  }
  else {
    // Nothing To Do
  }

  // If no thermistors are detected consider chamber thermistor 1 detected
  // to indicate the error code on 7 segment display.
  if ( (adcRead.flags.thermistor1DetectedFLG == false) && \
          (adcRead.flags.thermistor2DetectedFLG == false) && \
          (adcRead.flags.thermistor3DetectedFLG == false) && \
          (adcRead.flags.thermistor4DetectedFLG == false)) {
    adcRead.flags.thermistor1DetectedFLG = true;
  }
  if(adcRead.connectThermistor >= MINIMUM_THERMISOR_COUNTS){
      adcRead.flags.validThermistorsFLG = true;
  }
  else{
      adcRead.flags.validThermistorsFLG = false;
  }
}


/*
================================================================================
Method name:  ADCRead
                    
Originator:   Poorana kumar G

Description: 
  Call periodically from Scheduler (60 msec), to trigger the ADC conversion of
  one channel. And after that, Check the completion status and read the
  converted data and checks the minimum maximum ranges for thermistors digital
  data, Moisture detectors digital value and power supply voltage also.

  This method should be called using adcRead.ReadFunction().

Resources:
  8 ADC channel inputs. They are
    Inlet Temperature
    Outlet Temperature
    4 Chamber Thermistors
    Leak Detector
    Power supply (5V)

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                            Poorana kumar G
1.1.0  02-04-2020  As per the Beta requirement changes      Poorana kumar G
                   chamber detection function is added.
                   Thermistor open & short detection based
                   on detected result is added.
                   Leak detector upper threshold check is
                   added & error clear removed to force the
                   user to power OFF & ON the board. 
2.2.0  07-16-2020  Macro to disable the inlet temperature   Poorana kumar G
                   ADC conversion is added.
2.3.0  09-15-2020  Averaging is made common for all the     Poorana kumar G
                   analog channels 
--------------------------------------------------------------------------------
*/

bool ADCRead(void)
{
  bool retVal = TASK_NOT_COMPLETED;
  uint16_t dummyValueW = 0;

  // ADC Conversion state machine
  switch(adcRead.adcStatus)
  {
    // Trigger discharge of sample & hold capacitor
    case ADC_CONV_SH_DISCHARGE_START:
      ADCReadTriggerDischarge();
      adcRead.adcStatus = ADC_CONV_SH_DISCHARGE_END;
      break;

    // Stop discharge of sample & hold capacitor
    case ADC_CONV_SH_DISCHARGE_END:
      ADCReadStopDischarge();
      adcRead.adcStatus = ADC_CONV_DUMMY_READ;
      break;
      
      // Do a dummy read to clear the status
    case ADC_CONV_DUMMY_READ:
      if ( ADCREAD_CONVERSION_COMPLETE() == true) {
        dummyValueW = ADCREAD_READ_REGISTER();

        // Just to avoid PC-Lint error
        (void) dummyValueW;

        adcRead.adcStatus = ADC_CONV_SAMPLING_START;
      }
      break;

    // Select actual channel & Start sampling
    case ADC_CONV_SAMPLING_START:
      ADCREAD_CHANNEL_SELECT(ADCReadChannels[adcRead.adcChannelIndex]);
      ADCREAD_START_SAMPLING();
      adcRead.adcStatus = ADC_CONV_SAMPLING_END;
      break;

    // Stop sampling. After stopped automatically conversion started
    case ADC_CONV_SAMPLING_END:
      ADCREAD_STOP_SAMPLING();
      adcRead.adcStatus = ADC_CONV_READ_DATA;
      break;
      
    case ADC_CONV_READ_DATA:
      // Check ADC conversion status
      if ( ADCREAD_CONVERSION_COMPLETE() == true) {
        // Read the register which have the digital data
        adcRead.adcDataARYW[adcRead.adcChannelIndex] = ADCREAD_READ_REGISTER();

        debug_PowerADCLocalW = adcRead.adcDataARYW[VCC_VOLTAGE];
        
        adcRead.adcDataARYW[adcRead.adcChannelIndex] =                            \
                LowPassFilter(&adcRead.adcDataFilterARYW[adcRead.adcChannelIndex],\
                adcRead.adcDataARYW[adcRead.adcChannelIndex]);

        // Double temperatures so when we average we do not lose resolution
        // After averaged that it is converted to the format of actual one used.
        if ( (adcRead.adcChannelIndex != MOISTURE_DETECTOR) &&          \
                (adcRead.adcChannelIndex != VCC_VOLTAGE)) {
          adcRead.adcDataARYW[adcRead.adcChannelIndex] =                \
                  (ADC_FULL_COUNT - adcRead.adcDataARYW[adcRead.adcChannelIndex]) * 2;
        }

        // After power ON thermistor detection completed
        if ( adcRead.powerONADCDetTimer == 0 ) {
          switch(adcRead.adcChannelIndex) {
            // Inlet & Outlet thermistor ADC count error check
            case INLET_TEMPERATURE:
            case OUTLET_TEMPERATURE:
              checkThermistorError(adcRead.adcChannelIndex);
              break;

            case CHAMBER_TEMPERATURE1:
              // If Chamber thermistor 1 is detected
              if ( adcRead.flags.thermistor1DetectedFLG == true) {
                // Chamber thermistor 1 ADC count error check
                checkThermistorError(CHAMBER_TEMPERATURE1);
              }
              break;

            case CHAMBER_TEMPERATURE2:
              // If Chamber thermistor 2 is detected
              if ( adcRead.flags.thermistor2DetectedFLG == true) {
                // Chamber thermistor 2 ADC count error check
                checkThermistorError(CHAMBER_TEMPERATURE2);
              }
              break;

            case CHAMBER_TEMPERATURE3:
              // If Chamber thermistor 3 is detected
              if ( adcRead.flags.thermistor3DetectedFLG == true) {
                // Chamber thermistor 3 ADC count error check
                checkThermistorError(CHAMBER_TEMPERATURE3);
              }
              break;

            case CHAMBER_TEMPERATURE4:
              // If Chamber thermistor 4 is detected
              if ( adcRead.flags.thermistor4DetectedFLG == true) {
                // Chamber thermistor 4 ADC count error check
                checkThermistorError(CHAMBER_TEMPERATURE4);
              }
              break;

            case MOISTURE_DETECTOR:
              // Moisture detector ADC count error check
              // Clearing leak detection error only by power reset
              if ( (adcRead.adcDataARYW[MOISTURE_DETECTOR] < MOISTURE_DETECTOR_LEAK_COUNT) ||\
                      (adcRead.adcDataARYW[MOISTURE_DETECTOR] > MOISTURE_DETECTOR_UPPER_THRESHOLD)) {
                // Declare leak detection error
//                faultIndication.Error(LEAKAGE_ERROR);
                  Flag_Error_Leak = 1;
              }
              
              if(Flag_Error_Leak)
              {
//                  debounce_call3++;
                  if(flag_once4)
                  {
                      flag_once4 = 1;
                      Timer_1ms4 = 0;
                      flag_1msTimer4 = 0;                      
                  }
                  if(flag_1msTimer4)
                  {
                    if ( (adcRead.adcDataARYW[MOISTURE_DETECTOR] < MOISTURE_DETECTOR_LEAK_COUNT) ||\
                       (adcRead.adcDataARYW[MOISTURE_DETECTOR] > MOISTURE_DETECTOR_UPPER_THRESHOLD)) 
                    {
                        debounce_call4++;                        
                    }
                    else
                    {
                        debounce_call4 = 0; 
                    }
                  }
                      
              }
              
              if(debounce_call4 > DebounceIterations)
              {
                  faultIndication.Error(LEAKAGE_ERROR);
              }
              break;

            case VCC_VOLTAGE:
              // DC Supply voltage monitor
              checkVccError();
              break;
              
            default:
              break;
          }
        }

        // Increment the channel index and if converted all channels
        if ( ++adcRead.adcChannelIndex >= TOTAL_ADC_CHANNELS) {
          // Reset the channel index
#ifdef DISABLE_INLET_THERMISTOR
          // If inlet thermistor disabled, start with Outlet thermistor
          adcRead.adcChannelIndex = OUTLET_TEMPERATURE;
#else
          adcRead.adcChannelIndex = INLET_TEMPERATURE;
#endif

          // Wait for timer to become 0 to detect the chamber thermistors
          if ( adcRead.powerONADCDetTimer != 0 ) {
            adcRead.powerONADCDetTimer--;

            if ( adcRead.powerONADCDetTimer == 0 ) {
              adcRead.DetectThermistor();
            }
          }
        }

        retVal = TASK_COMPLETED;

        adcRead.adcStatus = ADC_CONV_SH_DISCHARGE_START;
      }
      break;

      default:
      break;
  }

  return retVal;
}

