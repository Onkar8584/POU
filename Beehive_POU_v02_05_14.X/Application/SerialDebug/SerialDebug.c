/*
================================================================================
File name:    SerialDebug.c
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  The purpose of this object is to transmit the status of the system collected
  from other objects, forms a packet and to send it through UART. Through the
  same port it can allow the developer to change some variables. This facility
  is only for development and testing purposes.

Class Methods:
  void SerialDebugProcess(void);
    Call periodically from Scheduler (500 msec), to print the status of the
    board 500 ms once through serial port. And the same function receive the
    frame from serial port and process that also.

  static void PrintSting(const char * string, int i);
    Copy the string to the buffer. The parameters are the address of the source
    string and the index.

  static char PrintFloat(float Number, int8_t Digits, int8_t NumFrac);
    Convert the float into ASCII characters and fill it into the buffer. The
    parameters are the number to be converted, Number of digits and the digits
    after the fractions. This function return the number of digits filled in the
    buffer.

  static char PrintInteger(int16_t Number, int8_t Digits);
    Convert the integer into ASCII characters and fill it into the buffer. The
    parameters are the number to be converted and Number of digits. This
    function return the number of digits filled in the buffer.

Method Calling Requirements:
  Serial.DebugFunction() should be called once per 500 millisecond in
  scheduler.

Resources:
  UART Tx and Rx

IoTranslate requirements:
  None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-10-2019  Initial Write                            Poorana kumar G
       11-04-2019  PC-Lint warnings are cleared.            Poorana kumar G
1.1.0  01-31-2020  As per the Beta requirement changes      Poorana kumar G
                   updated the functions
2.2.0  07-16-2020  UART error code is added for temperature  Poorana kumar G
                   mode error. Macro to disable the inlet 
                   temperature display in UART is disabled.
--------------------------------------------------------------------------------
*/

#include "SerialDebug.h"
#include "FaultIndication.h"

#ifdef DEBUG_MACRO

// Debug strings for modes
static const char *modeStringsARY[] =
{
  "EEW",
  "LAV",
  "SANI",
  "UNKNO"
};

// Debug strings for errors
static const char *errorStringsARY[] =
{
  "E00",     // No Error
  "D",      // DC Supply
  "W",      // Leak Detect
  "E",      // Over run error
  "H",      // Over Heat
  "F",      // Flow sensor error
  "A",      // Flow Direction error
  "Z",      // Line Frequency monitor
  "O6",     // Inlet Thermistor Open
  "O5",     // Outlet Thermistor Open
  "O1",     // Chamber Thermistor 1 Open
  "O2",     // Chamber Thermistor 2 Open
  "O3",     // Chamber Thermistor 3 Open
  "O4",     // Chamber Thermistor 4 Open
  "S6",     // Inlet Thermistor Short
  "S5",     // Outlet Thermistor Short
  "S1",     // Chamber Thermistor 1 Short
  "S2",     // Chamber Thermistor 2 Short
  "S3",     // Chamber Thermistor 3 Short
  "S4",     // Chamber Thermistor 4 Short
  "R",      // CPU Register test
  "P",      // Program Counter test
  "I",      // Interrupt test
  "C",      // CPU Clock test
  "V",      // Code Flash CRC test
  "M",      // RAM test
  "T",      // Heater feedback test
  "U",      // EEPROM Flash CRC test
  "B",      // Button Malfunction
  "J"       // Temperature Mode Jumper
};


/*
================================================================================
Method name:  PrintSting
                    
Originator:   Poorana kumar G

Description: 
  Copy the string to the buffer. The parameters are the address of the source
  string and the index.

  This method should be called using PrintSting().

Resources:
  None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-10-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

static int16_t PrintSting(const char * string, int16_t i)
{
  int16_t j = 0;

  while(string[j]){
    Serial.debugTxARY[i++] = string[j++];
  }
  Serial.debugTxARY[i]=0;

  return i;
}

/*
================================================================================
Method name:  PrintInteger
                    
Originator:   Poorana kumar G

Description: 
  Convert the integer into ASCII characters and fill it into the buffer. The
  parameters are the number to be converted and Number of digits. This function
  return the number of digits filled in the buffer.

  This method should be called using PrintInteger().

Resources:
  None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-10-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

static int8_t PrintInteger(int16_t number, int8_t digits, int8_t forceNeg)
{
  int16_t i = 0;
  int8_t value = 0;
  int8_t started = 0;
  int8_t j = 0;

  if(number == 0) {
    Serial.debugTxARY[j++] = '0';
  }
  else {
    // If number is negative number or force negative is non zero
    if ( (number < 0) || forceNeg) {
      Serial.debugTxARY[j++] = '-';
    }

    if (number < 0) {
      number = -number;
    }

    started = 0; //To detect the starting non zero character
    for ( i=10000; (i>0 && j < digits); i /= 10) {
      value = 0;

      if ( number >= i) {
        started = 1;
      	value = number / i;
        number = number - (value * i);
      }
      value = value + '0';
      if ( started) {
        Serial.debugTxARY[j++] = value;
      }
    }
  }

  Serial.debugTxARY[j] = 0;

  return j;
}


/*
================================================================================
Method name:  PrintFloat
                    
Originator:   Poorana kumar G

Description: 
  Convert the float into ASCII characters and fill it into the buffer. The
  parameters are the number to be converted, Number of digits and the digits
  after the fractions. This function return the number of digits filled in the
  buffer.

  This method should be called using PrintFloat().

Resources:
  None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-10-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

static int8_t PrintFloat(float number, int8_t digits, int8_t numFrac)
{
  int8_t digitPrinted;
  int8_t left;
  int8_t forceNeg = 0;
  long whole;

  if ( number < 0) {
    number = -number;
    forceNeg = 1;
  }	

  whole = (long) number;
  number = number - whole;

  digitPrinted = PrintInteger(whole, digits, forceNeg);

  if ( digitPrinted < digits) {
    Serial.debugTxARY[digitPrinted++] = '.';
  }

  left = digits - digitPrinted;
  if(left > numFrac) {
    left = numFrac;
  }
  left = left + digitPrinted;

  for( ; digitPrinted < left; ) {
    number = number * 10;
    whole = (long) number;
    Serial.debugTxARY[digitPrinted++] = whole + '0';
    number = number - whole;
  }
  return digitPrinted;
}


/*
================================================================================
Method name:  SerialDebugProcess
                    
Originator:   Poorana kumar G

Description: 
  Call periodically from Scheduler (500 msec), to print the status of the board
  500 ms once through serial port. And the same function receive the frame from
  serial port and process that also.

  This method should be called using Serial.DebugFunction().

Resources:
  None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-10-2019  Initial Write                            Poorana kumar G
       11-04-2019  PC-Lint warnings are cleared by adding   Poorana kumar G
                   "void" in "UART1_WriteBuffer"
                   function call
1.1.0  01-31-2020  As per the Beta requirement changes      Poorana kumar G
                   added some more debug status prints.
2.2.0  07-16-2020  Macro to disable the inlet temperature   Poorana kumar G
                   display in UART is disabled.
--------------------------------------------------------------------------------
*/

bool SerialDebugProcess(void)
{
  uint8_t digitCount = 0;
  uint8_t beginSecNumber = 0;
  uint8_t firstNonSpace = 0;
  uint8_t data = 0;
  uint8_t i = 0;
  uint8_t len = 0;
  volatile uint16_t tempW = 0;
  float tempFloatVal = 0;

  // Increment the timestamp
  Serial.debugTimeStampW++;

  switch ( Serial.debugRxStatus) {
    case DEBUG_SERIAL_STATE_IDLE:
      digitCount = UART1_ReadBuffer(Serial.debugRxARY, 0xFF);
      if ( digitCount != 0) {
        if ( Serial.debugRxARY[0] == '?') {
          Serial.debugRxStatus = DEBUG_SERIAL_STATE_WAITING;
          Serial.debugRxIndex = digitCount;
        }
      }
      else {
        // Timestamp conversion and print
        digitCount = PrintInteger(Serial.debugTimeStampW, 5, 0);
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

//#ifdef DISABLE_INLET_THERMISTOR
//        digitCount = PrintSting("XX,\t", 0);
//        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);
//#else
//        // Inlet temperature conversion and print
//        tempW = adcCountToTemperature(Tin);
//        digitCount = PrintInteger(tempW, 3, 0);
//        digitCount = PrintSting(",\t", digitCount);
//        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);
//#endif
        
        // Power cycle in Power mode and print
        digitCount = PrintInteger(optoCouplerControl.powerMode, 2, 0);
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // Outlet temperature conversion and print
        tempW = adcCountToTemperature(Tout);
        digitCount = PrintInteger(tempW, 3, 0);
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // User set temperature conversion and print
        tempW = nonVol.settings.targetTemperature;
        if ( nonVol.settings.flags.fahrenheitCelsiusSelFLG) {
          tempW = CtoFconvert(tempW);
        }
        digitCount = PrintInteger(tempW, 3, 0);
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // Chamber temperature - 1
        if ( adcRead.flags.thermistor1DetectedFLG) {
          tempW = adcCountToTemperature(adcRead.adcDataARYW[CHAMBER_TEMPERATURE1]);
          digitCount = PrintInteger(tempW, 3, 0);
        }
        else {
          digitCount = PrintSting("XX", 0);
        }
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // Chamber temperature - 2
        if ( adcRead.flags.thermistor2DetectedFLG) {
          tempW = adcCountToTemperature(adcRead.adcDataARYW[CHAMBER_TEMPERATURE2]);
          digitCount = PrintInteger(tempW, 3, 0);
        }
        else {
          digitCount = PrintSting("XX", 0);
        }
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // Chamber temperature - 3
        if ( adcRead.flags.thermistor3DetectedFLG) {
          tempW = adcCountToTemperature(adcRead.adcDataARYW[CHAMBER_TEMPERATURE3]);
          digitCount = PrintInteger(tempW, 3, 0);
        }
        else {
          digitCount = PrintSting("XX", 0);
        }
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // Chamber temperature - 4
        if ( adcRead.flags.thermistor4DetectedFLG) {
          tempW = adcCountToTemperature(adcRead.adcDataARYW[CHAMBER_TEMPERATURE4]);
          digitCount = PrintInteger(tempW, 3, 0);
        }
        else {
          digitCount = PrintSting("XX", 0);
        }
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // Faults list print
        if (faultIndication.faultCount == NO_FAULTS) {
          digitCount = PrintSting(errorStringsARY[0], 0);
        }
        else {
          tempW = 0;
          digitCount = 0;
          while ( tempW < faultIndication.faultCount) {
            digitCount = PrintSting(errorStringsARY[faultIndication.faultsListARY[tempW]], digitCount);
            digitCount = PrintSting(" ", digitCount);

            tempW++;
          }
        }
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // "Proportional" term conversion and print
        digitCount = PrintFloat(PID_CONST_KP, 6, 4);
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // "Integral" term conversion and print
        digitCount = PrintFloat(PID_CONST_KI, 6, 4);
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // "Differential" term for increasing conversion and print
        digitCount = PrintFloat(PID_CONST_KDI, 6, 4);
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // "Differential" term for decreasing conversion and print
        digitCount = PrintFloat(PID_CONST_KDD, 6, 4);
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // High error threshold conversion and print
        digitCount = PrintFloat(PID_CONST_HI_ERR_THRESH, 6, 4);
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // Pre-burn limit conversion and print
        digitCount = PrintFloat(PID_CONST_PREBURN_LIMIT, 6, 4);
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // Power cycle conversion and print
        digitCount = PrintInteger(optoCouplerControl.powerCycle, 3, 0);
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);
        
        // Relay control status print
        digitCount = PrintInteger((uint16_t)tempControl.relayStatus, 1, 0);
        digitCount = PrintSting(",\t", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);
        
        digitCount = PrintInteger((uint16_t)Relay1SupplyStatusDigIn_Read(), 1, 0);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);
        digitCount = PrintInteger((uint16_t)Relay2SupplyStatusDigIn_Read(), 1, 0);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);
        
        // Relay On/OFF state
        if ( (tempControl.relayStatus == RELAY_CONTROL_CONTROL)             \
                || (tempControl.relayStatus == RELAY_CONTROL_SHUTDOWN)      \
                || (tempControl.relayStatus == RELAY_CONTROL_STBYHEAT)      \
                || (tempControl.relayStatus == RELAY_CONTROL_LOWFLOW)      \
                || (tempControl.relayStatus == RELAY_CONTROL_DRY_FIRE_WAIT)) {
          digitCount = PrintSting("1,\t", 0);
        }
        else {
          digitCount = PrintSting("0,\t", 0);
        }
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // Flow detection status
        if ( flowDetector.flags.flowDetectedFLG == true) {
          digitCount = PrintFloat(Get_flowIn_Gallons(), 6, 4);
          digitCount = PrintSting(",\t", digitCount);
        }
        else {
          digitCount = PrintSting("FLOWOFF,\t", 0);
        }
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // Up button status
        if ( uI.upButtonState == BUTTON_STATUS_IDLE) {
          digitCount = PrintSting("BUO,\t", 0);
        }
        else {
          digitCount = PrintSting("BUC,\t", 0);
        }
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // Down button status
        if ( uI.downButtonState == BUTTON_STATUS_IDLE) {
          digitCount = PrintSting("BDO,\t", 0);
        }
        else {
          digitCount = PrintSting("BDC,\t", 0);
        }
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        // Enter button status
        if ( uI.enterButtonState == BUTTON_STATUS_IDLE) {
          digitCount = PrintSting("BEO,\t", 0);
        }
        else {
          digitCount = PrintSting("BEC,\t", 0);
        }
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

        digitCount = PrintSting(modeStringsARY[nonVol.settings.temperatureMode], 0);
        digitCount = PrintSting("\r\n", digitCount);
        (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);
      }
    break;

    case DEBUG_SERIAL_STATE_WAITING:
      // Check any data received
      digitCount = UART1_ReadBuffer(&Serial.debugRxARY[Serial.debugRxIndex], 0xFF);
      if ( digitCount != 0) {
        Serial.debugRxIndex += digitCount;

        for(digitCount = 0; digitCount < Serial.debugRxIndex; digitCount++) {
          // If Enter button is pressed in keyboard
          if ( Serial.debugRxARY[digitCount] == '\r') {
            Serial.debugRxARY[digitCount] = 0;

            digitCount = PrintSting("\r\n", 0);
            (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);

            Serial.debugRxStatus = DEBUG_SERIAL_STATE_COMMAND_READY;
            break;
          }
        }
      }
    break;

    case DEBUG_SERIAL_STATE_COMMAND_READY:
      // Process the command received
      switch (Serial.debugRxARY[1]) {
        //Wants to force power
        case ('f') :
        case ('F') :
          optoCouplerControl.forcePowerCycle = \
                  abs(atoi((char *)&Serial.debugRxARY[2]));

          if ( optoCouplerControl.forcePowerCycle > MAXPOWER_POWER_CYCLE) {
            optoCouplerControl.forcePowerCycle = MAXPOWER_POWER_CYCLE;
          }
        break;

        // Enter ?pnn vvvvvvvv n is a number from 0 to 5, vvvvv is the
        // integerized version of the parameter
        //Expect a space between the parameter number and the value
        case ('p') : 
		case ('P') :
            
          //Find the length of the string
          Serial.debugRxARY[0] = ' ';
          Serial.debugRxARY[1] = ' '; //replace ?P just for consistency
          for (i = 0; Serial.debugRxARY[i]; i++) {
          } //look for end of string marker

          len = i;

          //find first non ' ' character
          for ( i = 0; ((i < len) && (Serial.debugRxARY[i] == ' ')); i++) {
          }
          firstNonSpace = i;
          // If there are no non-space characters quit
          if ( firstNonSpace == len) {
            break;
          }
          // Find the beginning of the second number
          for ( i = firstNonSpace; ((i < len) && \
                  (Serial.debugRxARY[i] != ' ')); i++) {
          }
          beginSecNumber = i;
          // If the second number field does not exist quit
          if ( beginSecNumber == len) {
            break;
          }

          // Mark the end of the first field
          Serial.debugRxARY[beginSecNumber] = 0;
          beginSecNumber++;
          data = atoi((char *)&Serial.debugRxARY[firstNonSpace]);

          // If the parameter number (data) is valid then
          if ( data < START_OF_FLOW_PARAMETER) {
            nonVol.settings.pidConstantsARYF[data] = \
                    (float) atof((char *)&Serial.debugRxARY[beginSecNumber]);
            nonVol.write();
          }
          else if ( data >= START_OF_FLOW_PARAMETER){
              
                switch(data)
                {
                    case FLOW_LOWER_BOUNDRY_PARAM:
                      flowDetector.flowLowerBoundryW = (float) atof((char *)&Serial.debugRxARY[beginSecNumber]);
                    break;
                    
                    case FLOW_HYSTERESIS_OFFSET_PARAM:
                        tempFloatVal = (float) atof((char *)&Serial.debugRxARY[beginSecNumber]);
                        
                        if((tempFloatVal >= LOW_FLOW_HYST_OFF_LOWER_BOUNDRY) && (tempFloatVal <= LOW_FLOW_HYST_OFF_UPPER_BOUNDRY))
                        {
                            flowDetector.flowHysteresisOffsetW = tempFloatVal;
                        }
                        else
                        {
                            flowDetector.flowHysteresisOffsetW = LOW_FLOW_HYSTERESIS_OFFSET_DEFAULT;
                        }
                    break;
                    
                    case DRY_FIRE_THRESHOLD_PARAM:
                        tempW = (uint16_t) atoi((char *)&Serial.debugRxARY[beginSecNumber]);
                        if((tempW >= DRY_FIRE_THRESHOLD_LOWER_LIMIT) && (tempW <= DRY_FIRE_THRESHOLD_UPPER_LIMIT))
                        {
                            tempControl.dryFireThresholdW = temperatureToADCCount(tempW);
                        }
                        else
                        {
                            tempControl.dryFireThresholdW = temperatureToADCCount(DRY_FIRE_THRESHOLD_DEFAULT_LIMIT);
                        }                    
                    break;
                    
                    default:
                        // do nothing
                    break;
                }
                
          }
        break;

        case ('v') :
        case ('V') :
          Serial.PrintVersion();
        break;

        case ('z') :
        case ('Z') :
          tempControl.integralF = 0.;
        break;

        default:
        break;
      }
      Serial.debugRxStatus = DEBUG_SERIAL_STATE_IDLE;
    break;

    default:
    break;
  }
  
  return TASK_COMPLETED;
}


/*
================================================================================
Method name:  SerialPrintVersion
                    
Originator:   Poorana kumar G

Description: 
  Call at power ON and whenever command received to print the software version.

  This method should be called using Serial.PrintVersion().

Resources:
  None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-10-2019  Initial Write                        Poorana kumar G
       11-04-2019  PC-Lint warnings are cleared by      Poorana kumar G
                   adding "void" in "UART1_WriteBuffer"
                   function call
--------------------------------------------------------------------------------
*/

void SerialPrintVersion(void)
{
  uint8_t digitCount = 0;

  digitCount = PrintSting("\r\n", digitCount);
  digitCount = PrintSting(SWVersion, digitCount);
  digitCount = PrintSting("\r\n", digitCount);
  (void) UART1_WriteBuffer(Serial.debugTxARY, digitCount);
}

#endif /* DEBUG_MACRO */

