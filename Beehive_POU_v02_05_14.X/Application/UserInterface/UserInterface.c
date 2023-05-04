/*
================================================================================
File name:    UserInterface.c
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  This object provides the interface between board and user. The purpose of the
  object is to provide the user interface by continuously monitors the 3 buttons
  and control the 7 segment display. 

Class Methods:
  void UserInterface(void);
    Call periodically from Scheduler (1 msec), to provide the user interface.
    Continuously check the buttons status and refresh the 7 segment display.

Method Calling Requirements:
  uI.UIFunction() should be called once per 1 millisecond in
  scheduler.

Resources:
  3 GPIOs for 3 buttons
  3 GPIOs for control the 3 digits ON/OFF
  8 GPIOs for control each segments

IoTranslate requirements:
  #define UpButtonDigIn_Read()
  #define DownButtonDigIn_Read()
  #define EnterButtonDigIn_Read()
  #define DisplayDigit1DigOut_ON()
  #define DisplayDigit1DigOut_OFF()
  #define DisplayDigit2DigOut_ON()
  #define DisplayDigit2DigOut_OFF()
  #define DisplayDigit3DigOut_ON()
  #define DisplayDigit3DigOut_OFF()
  #define DisplaySegADigOut_High()
  #define DisplaySegADigOut_Low()
  #define DisplaySegBDigOut_High()
  #define DisplaySegBDigOut_Low()
  #define DisplaySegCDigOut_High()
  #define DisplaySegCDigOut_Low()
  #define DisplaySegDDigOut_High()
  #define DisplaySegDDigOut_Low()
  #define DisplaySegEDigOut_High()
  #define DisplaySegEDigOut_Low()
  #define DisplaySegFDigOut_High()
  #define DisplaySegFDigOut_Low()
  #define DisplaySegGDigOut_High()
  #define DisplaySegGDigOut_Low()
  #define DisplaySegDotDigOut_High()
  #define DisplaySegDotDigOut_Low()

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                                Poorana kumar G
1.1.0  01-30-2020  As per the Beta requirement changes          Poorana kumar G
                   updated the functions
2.1.0  05-14-2020  Error display method is changed as per       Poorana kumar G
                   the requirement changes.
2.2.0  07-16-2020  Standby heat enable/disable is blocked       Poorana kumar G
                   when mode is other than Lavatory. And
                   target temperature edit is blocked in
                   Emergency Eye wash mode.
2.3.1  12-28-2020  Display is turned off in normal condition    Dnyaneshwar
                   after 30 secs to save Power
2.3.2  05-13-2021  When Display is in OFF state and error       Dnyaneshwar
                   occurs display turns ON to Flash error
--------------------------------------------------------------------------------
*/

#include "UserInterface.h"
#include "ModeCheck.h"
#include "NonVol.h"
#include "FaultIndication.h"
#include "Scheduler.h"

// Seven segment decoded data
static const uint8_t sevenSegCode[] =
{
  0xC0,   // 0
  0xF9,   // 1
  0xA4,   // 2
  0xB0,   // 3
  0x99,   // 4
  0x92,   // 5
  0x82,   // 6
  0xF8,   // 7
  0x80,   // 8
  0x90    // 9
};

// This error codes will be displayed in 7 segment display 
static const uint16_t errorCodesDisplayARY[] = 
{
  NO_FAULTS,
  ERROR_CODE_DC_SUPPLY,
  ERROR_CODE_LEAKAGE,
  ERROR_CODE_OVER_RUN,
  ERROR_CODE_OVER_HEAT,
  ERROR_CODE_FLOW_SENSOR_ERROR,
  ERROR_CODE_FLOW_DIRECTION,
  ERROR_CODE_AC_LINE_FREQUENCY,
  ERROR_CODE_THERMISTOR1_OPEN,
  ERROR_CODE_THERMISTOR2_OPEN,
  ERROR_CODE_THERMISTOR3_OPEN,
  ERROR_CODE_THERMISTOR4_OPEN,
  ERROR_CODE_THERMISTOR5_OPEN,
  ERROR_CODE_THERMISTOR6_OPEN,
  ERROR_CODE_THERMISTOR1_SHORT,
  ERROR_CODE_THERMISTOR2_SHORT,
  ERROR_CODE_THERMISTOR3_SHORT,
  ERROR_CODE_THERMISTOR4_SHORT,
  ERROR_CODE_THERMISTOR5_SHORT,
  ERROR_CODE_THERMISTOR6_SHORT,
  ERROR_CODE_CPU_REGISTER_TEST,
  ERROR_CODE_PROGRAM_COUNTER_TEST,
  ERROR_CODE_INTERRUPT_TEST,
  ERROR_CODE_CLOCK_TEST,
  ERROR_CODE_FLASH_CRC_TEST,
  ERROR_CODE_RAM_TEST,
  ERROR_CODE_IO_TEST,
  ERROR_CODE_EEPROM_CRC_TEST,
  ERROR_CODE_BUTTON_MALFUNCTION,
  ERROR_CODE_TEMPERATURE_MODE,
//  ERROR_CODE_SCALE_BANK1,
//  ERROR_CODE_SCALE_BANK2,
  ERROR_CODE_SCALE_DETECTION
};

char flag_err_disp = 0;

void display_error(uint16_t fault_code);

extern uint8_t flag_Bank1Disable,flag_Bank2Disable;

/*
================================================================================
Method name:  DisplayPortPlaceData
                    
Originator:   Poorana kumar G

Description: 
  Place the data into 7 segment port.

  This method should be called using DisplayPortPlaceData().

Resources:
  8 GPIOs for control each segments of the display

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       02-03-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

static void DisplayPortPlaceData(uint8_t value)
{
  // Segment A GPIO status
  if ( value & SEG_A_POSITION) {
    DisplaySegADigOut_High();
  }
  else {
    DisplaySegADigOut_Low();
  }

  // Segment B GPIO status
  if ( value & SEG_B_POSITION) {
    DisplaySegBDigOut_High();
  }
  else {
    DisplaySegBDigOut_Low();
  }

  // Segment C GPIO status
  if ( value & SEG_C_POSITION) {
    DisplaySegCDigOut_High();
  }
  else {
    DisplaySegCDigOut_Low();
  }

  // Segment D GPIO status
  if ( value & SEG_D_POSITION) {
    DisplaySegDDigOut_High();
  }
  else {
    DisplaySegDDigOut_Low();
  }

  // Segment E GPIO status
  if ( value & SEG_E_POSITION) {
    DisplaySegEDigOut_High();
  }
  else {
    DisplaySegEDigOut_Low();
  }

  // Segment F GPIO status
  if ( value & SEG_F_POSITION) {
    DisplaySegFDigOut_High();
  }
  else {
    DisplaySegFDigOut_Low();
  }

  // Segment G GPIO status
  if ( value & SEG_G_POSITION) {
    DisplaySegGDigOut_High();
  }
  else {
    DisplaySegGDigOut_Low();
  }

  // Segment Dot GPIO status
  if ( value & SEG_DOT_POSITION) {
    DisplaySegDotDigOut_High();
  }
  else {
    DisplaySegDotDigOut_Low();
  }
}


/*
================================================================================
Method name:  TemperatureToDigit
                    
Originator:   Poorana kumar G

Description: 
  Covert the temperature into 7 segment data.

  This method should be called using uI.DigitConvert().

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

void TemperatureToDigit(uint16_t Temperature)
{
  // Calculate 100s digit
  uI.digit1Data = sevenSegCode[Temperature / 100];

  Temperature = (Temperature % 100);

  // Calculate 10s digit
  uI.digit2Data = sevenSegCode[Temperature / 10];

  // Calculate 1s digit
  uI.digit3Data = sevenSegCode[Temperature % 10];

  // If stand by heating is enabled, turn ON dot in 1s digit
  if ( nonVol.settings.flags.standbyHeatEnFLG) {
    uI.digit3Data &= DOT_ON;
  }
}


/*
================================================================================
Method name:  ErrorCodeToDigit
                    
Originator:   Poorana kumar G

Description: 
  Covert the error code into 7 segment data.

  This method should be called using uI.DigitConvert(uint16_t).

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                            Poorana kumar G
2.1.0  05-14-2020  Error code conversion is updated as 3    Poorana kumar G
                   digit conversion.
--------------------------------------------------------------------------------
*/

void ErrorCodeToDigit(uint16_t ErrorCode)
{
  // To ensure the error code is not more than 3 digits
  ErrorCode = ErrorCode % 1000;

  // Display 100s digit
  uI.digit1Data = sevenSegCode[ErrorCode / 100];

  ErrorCode = (ErrorCode % 100);

  // Calculate 10s digit
  uI.digit2Data = sevenSegCode[ErrorCode / 10];

  // Calculate 1s digit
  uI.digit3Data = sevenSegCode[ErrorCode % 10];
}


/*
================================================================================
Method name:  DisplayRawData
                    
Originator:   Poorana kumar G

Description: 
  Copy the raw data into each digits data

  This method should be called using uI.RawData().

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       01-30-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

void DisplayRawData(uint8_t digit1, uint8_t digit2, uint8_t digit3)
{
  uI.digit1Data = digit1;

  uI.digit2Data = digit2;

  uI.digit3Data = digit3;
}


/*
================================================================================
Method name:  DisplayFunction
                    
Originator:   Poorana kumar G

Description: 
  Control the display port and digit ON OFF GPIOs.

  This method should be called using uI.Displaycontrol().

Resources:
  3 GPIOs for control the 3 digits ON/OFF

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/

void DisplayFunction(void)
{
  static uint8_t displayIndex = 0;

  if ( uI.flags.displayEnFLG) {
    displayIndex++;

    switch ( displayIndex) {
      case DIGIT1_INDEX:
        // Turn ON digit 1 and Turn OFF others
        DisplayDigit2DigOut_OFF();
        DisplayDigit3DigOut_OFF();

        // If all segments to be OFF, simply OFF the digit
        if(uI.digit1Data == ALL_SEG_OFF) {
          DisplayDigit1DigOut_OFF();
        }
        else {
          // Place digit 1 data in GPIO port
          DisplayPortPlaceData(uI.digit1Data);
          DisplayDigit1DigOut_ON();
        }
      break;   

      case DIGIT2_INDEX:
        // Turn ON digit 2 and Turn OFF others
        DisplayDigit1DigOut_OFF();
        DisplayDigit3DigOut_OFF();

        // If all segments to be OFF, simply OFF the digit
        if(uI.digit2Data == ALL_SEG_OFF) {
          DisplayDigit2DigOut_OFF();
        }
        else {
          // Place digit 2 data in GPIO port
          DisplayPortPlaceData(uI.digit2Data);
          DisplayDigit2DigOut_ON();
        }
      break;   

      case DIGIT3_INDEX:
        // Turn ON digit 3 and Turn OFF others
        DisplayDigit1DigOut_OFF();
        DisplayDigit2DigOut_OFF();

        // If all segments to be OFF, simply OFF the digit
        if(uI.digit3Data == ALL_SEG_OFF) {
          DisplayDigit3DigOut_OFF();
        }
        else {
          // Place digit 3 data in GPIO port
          DisplayPortPlaceData(uI.digit3Data);
          DisplayDigit3DigOut_ON();
          }
      break;   

      case DIGITS_OFF_INDEX:
        // Turn OFF all the digits
        DisplayDigit1DigOut_OFF();
        DisplayDigit2DigOut_OFF();
        DisplayDigit3DigOut_OFF();
      break;   

      case DIGITS_TIMER_MAX:
        // Go to first index
        displayIndex = (DIGIT1_INDEX - 1);
      break;   

      default:
        // Nothing to do
      break;   
    }
  }
  else {
    displayIndex = 0;

    // Turn OFF all the digits
    DisplayDigit1DigOut_OFF();
    DisplayDigit2DigOut_OFF();
    DisplayDigit3DigOut_OFF();
  }
}

/*
================================================================================
Method name:  ResetUpButtonState
                    
Originator:   Poorana kumar G

Description: 
  Reset the UP button state enum and the timer variables.

  This method should be called using ResetUpButtonState().

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       01-31-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/
static inline void ResetUpButtonState(void)
{
  uI.upButtonState = BUTTON_STATUS_IDLE;
  uI.upButtonDebounceTimer = BUTTON_DEBOUNCE_TIME;
  uI.upButtonLongPressTimerW = BUTTON_LONG_PRESS_TIME;
  uI.upButtonMalfunctionTimerW = BUTTON_MALFUNCTION_TIME;
}


/*
================================================================================
Method name:  ResetDownButtonState
                    
Originator:   Poorana kumar G

Description: 
  Reset the DOWN button state enum and the timer variables.

  This method should be called using ResetDownButtonState().

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       01-31-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/
static inline void ResetDownButtonState(void)
{
  uI.downButtonState = BUTTON_STATUS_IDLE;
  uI.downButtonDebounceTimer = BUTTON_DEBOUNCE_TIME;
  uI.downButtonLongPressTimerW = BUTTON_LONG_PRESS_TIME;
  uI.downButtonMalfunctionTimerW = BUTTON_MALFUNCTION_TIME;
}


/*
================================================================================
Method name:  ResetEnterButtonState
                    
Originator:   Poorana kumar G

Description: 
  Reset the ENTER button state enum and the timer variables.

  This method should be called using ResetEnterButtonState().

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       01-31-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/
static inline void ResetEnterButtonState(void)
{
  uI.enterButtonState = BUTTON_STATUS_IDLE;
  uI.enterButtonDebounceTimer = BUTTON_DEBOUNCE_TIME;
  uI.enterButtonLongPressTimerW = BUTTON_LONG_PRESS_TIME;
  uI.enterButtonMalfunctionTimerW = BUTTON_MALFUNCTION_TIME;
}


/*
================================================================================
Method name:  ResetUIUserProgress
                    
Originator:   Poorana kumar G

Description: 
  To reset the user progress when temperature mode is changed.

  This method should be called using ResetEnterButtonState().

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       07-15-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
*/
void ResetUIUserProgress(void)
{
  // When the user configuration is going on & temperature mode is switched.
  if ( uI.displayState == DISPLAY_STATUS_FLASHING) {
    uI.displayState = DISPLAY_STATUS_NORMAL;
    uI.displayONTimerW = DISPLAY_ON_TIME;

    // Reset the edited flag
    uI.flags.editedFLG = 0;

    // Enable the display flag to show the temperature on display
    uI.flags.displayEnFLG = 1;

    // Move the state if the button is pressed
    if ( (uI.upButtonState == BUTTON_STATUS_PRESSED) ||     \
            (uI.upButtonState == BUTTON_STATUS_LONG_PRESSED)) {
      uI.upButtonState = BUTTON_STATUS_WAIT_FOR_RELEASE;
    }

    // Move the state if the button is pressed
    if ( (uI.downButtonState == BUTTON_STATUS_PRESSED) ||     \
            (uI.downButtonState == BUTTON_STATUS_LONG_PRESSED)) {
      uI.downButtonState = BUTTON_STATUS_WAIT_FOR_RELEASE;
    }
    
    // Move the state if the button is pressed
    if ( (uI.enterButtonState == BUTTON_STATUS_PRESSED) ||     \
            (uI.enterButtonState == BUTTON_STATUS_LONG_PRESSED)) {
      uI.enterButtonState = BUTTON_STATUS_WAIT_FOR_RELEASE;
    }
  }
}


/*
================================================================================
Method name:  ButtonsRead
                    
Originator:   Poorana kumar G

Description: 
  Read the 3 buttons debounce it and execute the relevant operation to it.

  This method should be called using uI.Buttons().

  UP Button:
    Press Event         Normal                  Flashing
    Short press         Go to Flashing          Increment temperature
    Press & Hold 3 sec  None                    Increment temperature speedy

  DOWN Button:
    Press Event         Normal                  Flashing
    Short press         Go to Flashing          Decrement temperature
    Press & Hold 3 sec  Toggle °F/°C setting    Decrement temperature speedy

  ENETR Button:
    Press Event         Normal                  Flashing
    Short press         None                    Save the edited temperature
    Press & Hold 3 sec  Toggle standby setting  None

Resources:
  3 GPIOs for 3 buttons

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                            Poorana kumar G
1.1.0  01-31-2020  As per the beta requirement, button      Poorana kumar G
                   malfunction check is added to check 30
                   seconds continuous press
2.2.0  07-16-2020  Standby heat enable/disable is blocked   Poorana kumar G
                   when mode is other than Lavatory. And
                   target temperature edit is blocked in
                   Emergency Eye wash mode.
--------------------------------------------------------------------------------
*/

void ButtonsRead(void)
{
  // Up Button state machine
  switch(uI.upButtonState) {
    case BUTTON_STATUS_IDLE:
      // If UP button is pressed
      if ( UpButtonDigIn_Read() == false) {
        // If debounce timer expires switch the state
        if ( uI.upButtonDebounceTimer-- == 0) {
          uI.upButtonState = BUTTON_STATUS_PRESSED;

          if ( uI.displayState == DISPLAY_STATUS_FLASHING) {
            uI.incrementTimerW = 0;
          }
          // Ignore the button press at power ON
          else if ( uI.displayState == DISPLAY_STATUS_POWER_ON){
            uI.upButtonState = BUTTON_STATUS_IDLE;
          }
          // Ignore Button Press when display is off and turn on display
          else if ( uI.displayState == DISPLAY_STATUS_OFF){
            uI.upButtonState = BUTTON_STATUS_IDLE;
            uI.displayState = DISPLAY_STATUS_NORMAL;
            uI.flags.displayEnFLG = 1;
//            uI.displayONTimerW = DISPLAY_ON_TIME; 
          }
          uI.displayONTimerW = DISPLAY_ON_TIME;
        }
        //Reload the Flash Exit timer if user input is detected
        uI.displayFlashExitTimerW = DISPLAY_FLASH_EXIT_TIME;
      }
      // If UP button is not pressed
      else {
        uI.upButtonDebounceTimer = BUTTON_DEBOUNCE_TIME;
      }
    break;

    case BUTTON_STATUS_PRESSED:
      // If UP button is still pressed
      if ( UpButtonDigIn_Read() == false) {
        if ( uI.displayState == DISPLAY_STATUS_FLASHING) {
          // Down count the increment timer
          if ( uI.incrementTimerW != 0) {
            uI.incrementTimerW--; 
          }
          else {
            uI.incrementTimerW = INCDEC_NORMAL_TIME;

            // Increment the temperature when display is flashing
            if ( uI.targetTemperature < modeCheck.temperatureMax) {
              uI.flags.editedFLG = 1;
              uI.targetTemperature++;
              uI.DigitConvert(uI.targetTemperature);
              
            }
          }
        }
        // Down count the long press timer
        if ( uI.upButtonLongPressTimerW-- == 0) {
          uI.upButtonLongPressTimerW = BUTTON_LONG_PRESS_TIME;
          uI.upButtonMalfunctionTimerW = BUTTON_MALFUNCTION_TIME;
          uI.upButtonState = BUTTON_STATUS_LONG_PRESSED;
        }
      }
      // IF UP button is released
      else {
        // Reset the status
        ResetUpButtonState();

        // Reaction for short press - Flash the display
        if ( uI.displayState == DISPLAY_STATUS_NORMAL) {
          // Target temperature change by user is only accepted in Lavatory & Sanitation modes
          if ( (nonVol.settings.temperatureMode == LAVATORY_MODE) ||  \
                (nonVol.settings.temperatureMode == SANITATION_MODE)) {
            uI.displayState = DISPLAY_STATUS_FLASHING;
            uI.tempTargetTemperature = uI.targetTemperature;


            // Turn OFF all the digits
            DisplayDigit1DigOut_OFF();
            DisplayDigit2DigOut_OFF();
            DisplayDigit3DigOut_OFF();
          }
        }
      }
    break;

    case BUTTON_STATUS_LONG_PRESSED:
      // If UP button is still pressed
      if ( UpButtonDigIn_Read() == false) {
        if ( uI.displayState == DISPLAY_STATUS_FLASHING) {
          // Down count the increment timer
          if ( uI.incrementTimerW != 0) {
            uI.incrementTimerW--; 
          }
          else {
            uI.incrementTimerW = INCDEC_LONGPRESS_TIME;

            // Do the fast increment on target temperature
            if ( uI.targetTemperature < modeCheck.temperatureMax) {
              uI.targetTemperature++;
              uI.DigitConvert(uI.targetTemperature);
            }
          }

          if ( --uI.upButtonMalfunctionTimerW == 0) {
            faultIndication.Error(BUTTON_MALFUNCTION_ERROR);
            uI.upButtonState = BUTTON_STATUS_MALFUNCTION;
          }
        }
        // If user configuration is not enabled just move to next state
        else {
          uI.upButtonMalfunctionTimerW = BUTTON_MALFUNCTION_TIME;
          uI.upButtonState = BUTTON_STATUS_WAIT_FOR_RELEASE;
        }
        
        //Reload the Flash Exit timer if user input is detected
        uI.displayFlashExitTimerW = DISPLAY_FLASH_EXIT_TIME;
      }
      // If UP button released, reset the state
      else {
        ResetUpButtonState();
      }
    break;

    case BUTTON_STATUS_WAIT_FOR_RELEASE:
      // If UP button still pressed, down count the malfunction timer
      if ( UpButtonDigIn_Read() == false) {
        if ( --uI.upButtonMalfunctionTimerW == 0) {
          // When timer expired, report error and move to malfunction state
          faultIndication.Error(BUTTON_MALFUNCTION_ERROR);
          uI.upButtonState = BUTTON_STATUS_MALFUNCTION;
        }
      }
      // If UP button released, reset the state
      else {
        ResetUpButtonState();
      }
    break;

    case BUTTON_STATUS_MALFUNCTION:
      if ( UpButtonDigIn_Read() == true) {
        // Clear the error when other buttons are not in malfunction state
        if ( (uI.downButtonState != BUTTON_STATUS_MALFUNCTION) && \
                (uI.enterButtonState != BUTTON_STATUS_MALFUNCTION)) {
          faultIndication.Clear(BUTTON_MALFUNCTION_ERROR);
        }

        ResetUpButtonState();
      }
    break;

    default:
      // To avoid false triggering, reset the state
      ResetUpButtonState();
    break;
  }

  // Down Button state machine
  switch(uI.downButtonState) {
    case BUTTON_STATUS_IDLE:
      // If DOWN button is pressed
      if ( DownButtonDigIn_Read() == false) {
        // If debounce timer expires switch the state
        if ( uI.downButtonDebounceTimer-- == 0) {
          uI.downButtonState = BUTTON_STATUS_PRESSED;

          if ( uI.displayState == DISPLAY_STATUS_FLASHING) {
            uI.decrementTimerW = 0;
          }
          // Ignore the button press at power ON
          else if ( uI.displayState == DISPLAY_STATUS_POWER_ON){
            uI.downButtonState = BUTTON_STATUS_IDLE;
          }
          // Ignore Button Press when display is off and turn on display
          else if ( uI.displayState == DISPLAY_STATUS_OFF){
            uI.downButtonState = BUTTON_STATUS_IDLE;
            uI.displayState = DISPLAY_STATUS_NORMAL;
//            uI.displayONTimerW = DISPLAY_ON_TIME; 
            uI.flags.displayEnFLG = 1;
          }
          uI.displayONTimerW = DISPLAY_ON_TIME;
        }
        
        //Reload the Flash Exit timer if user input is detected
        uI.displayFlashExitTimerW = DISPLAY_FLASH_EXIT_TIME;
      }
      // If DOWN button is not pressed
      else {
        uI.downButtonDebounceTimer = BUTTON_DEBOUNCE_TIME;
      }
    break;

    case BUTTON_STATUS_PRESSED:
      // If DOWN button is still pressed
      if ( DownButtonDigIn_Read() == false) {
        if ( uI.displayState == DISPLAY_STATUS_FLASHING) {
          // Down count the decrement timer
          if ( uI.decrementTimerW != 0) {
            uI.decrementTimerW--; 
          }
          else {
            uI.decrementTimerW = INCDEC_NORMAL_TIME;

            // Decrement the temperature when display is flashing
            if ( uI.targetTemperature > modeCheck.temperatureMin) {
              uI.flags.editedFLG = 1;
              uI.targetTemperature--;
              uI.DigitConvert(uI.targetTemperature);
            }
          }
        }

        // Down count the long press timer
        if ( uI.downButtonLongPressTimerW-- == 0) {
          uI.downButtonLongPressTimerW = BUTTON_LONG_PRESS_TIME;
          uI.downButtonMalfunctionTimerW = BUTTON_MALFUNCTION_TIME;
          uI.downButtonState = BUTTON_STATUS_LONG_PRESSED;
        }
      }
      // If DOWN button is released
      else {
        // Reset the status
        ResetDownButtonState();

        // Reaction for short press - Flash the display
        if ( uI.displayState == DISPLAY_STATUS_NORMAL) {
          // Target temperature change by user is only accepted in Lavatory & Sanitation modes
          if ( (nonVol.settings.temperatureMode == LAVATORY_MODE) ||  \
                (nonVol.settings.temperatureMode == SANITATION_MODE)) {
            uI.displayState = DISPLAY_STATUS_FLASHING;
            uI.tempTargetTemperature = uI.targetTemperature;

            // Turn OFF all the digits
            DisplayDigit1DigOut_OFF();
            DisplayDigit2DigOut_OFF();
            DisplayDigit3DigOut_OFF();
          }
        }
      }
    break;

    case BUTTON_STATUS_LONG_PRESSED:
      // If DOWN button is still pressed
      if ( DownButtonDigIn_Read() == false) {
        if ( uI.displayState == DISPLAY_STATUS_FLASHING) {
          // Down count the decrement timer
          if ( uI.decrementTimerW != 0) {
            uI.decrementTimerW--; 
          }
          else {
            uI.decrementTimerW = INCDEC_LONGPRESS_TIME;

            // Do the fast decrement on target temperature
            if ( uI.targetTemperature > modeCheck.temperatureMin) {
              uI.targetTemperature--;
              uI.DigitConvert(uI.targetTemperature);
            }
          }

          // When timer expired, report error and move to malfunction state
          if ( --uI.downButtonMalfunctionTimerW == 0) {
            faultIndication.Error(BUTTON_MALFUNCTION_ERROR);
            uI.downButtonState = BUTTON_STATUS_MALFUNCTION;
          }
        }
        else {
          // If user configuration is not enabled, toggle the °F or °C flag
          if ( nonVol.settings.flags.fahrenheitCelsiusSelFLG == 0) {
            nonVol.settings.flags.fahrenheitCelsiusSelFLG = 1;
            uI.targetTemperature = FtoCconvert(uI.targetTemperature);

            modeCheck.temperatureMin = FtoCconvert(modeCheck.temperatureMin);
            modeCheck.temperatureMax = FtoCconvert(modeCheck.temperatureMax);
          }
          else {
            nonVol.settings.flags.fahrenheitCelsiusSelFLG = 0;
            uI.targetTemperature = CtoFconvert(uI.targetTemperature);

            modeCheck.temperatureMin = \
                    userTemperatureMinARYW[nonVol.settings.temperatureMode];
            modeCheck.temperatureMax = \
                    userTemperatureMaxARYW[nonVol.settings.temperatureMode];

            // To avoid the decimal error in minimum temperature.
            if ( (uI.targetTemperature < modeCheck.temperatureMin) || \
                    (uI.targetTemperature > modeCheck.temperatureMax)) {
                uI.targetTemperature = modeCheck.temperatureMin;
            }
          }

          nonVol.settings.targetTemperature = uI.targetTemperature;
          // Store the changes into non volatile memory
          nonVol.write();

          // Convert for display it
          uI.DigitConvert(uI.targetTemperature);

          uI.downButtonMalfunctionTimerW = BUTTON_MALFUNCTION_TIME;
          uI.downButtonState = BUTTON_STATUS_WAIT_FOR_RELEASE;
        }
        
        //Reload the Flash Exit timer if user input is detected
        uI.displayFlashExitTimerW = DISPLAY_FLASH_EXIT_TIME;
      }
      // If DOWN button released, reset the state
      else {
        ResetDownButtonState();
      }
    break;

    case BUTTON_STATUS_WAIT_FOR_RELEASE:
      // If DOWN button still pressed, down count the malfunction timer
      if ( DownButtonDigIn_Read() == false) {
        // When timer expired, report error and move to malfunction state
        if ( --uI.downButtonMalfunctionTimerW == 0) {
          faultIndication.Error(BUTTON_MALFUNCTION_ERROR);
          uI.downButtonState = BUTTON_STATUS_MALFUNCTION;
        }
      }
      // If DOWN button released, reset the state
      else {
        ResetDownButtonState();
      }
    break;

    case BUTTON_STATUS_MALFUNCTION:
      if ( DownButtonDigIn_Read() == true) {
        // Clear the error when other buttons are not in malfunction state
        if ( (uI.upButtonState != BUTTON_STATUS_MALFUNCTION) && \
                (uI.enterButtonState != BUTTON_STATUS_MALFUNCTION)) {
          faultIndication.Clear(BUTTON_MALFUNCTION_ERROR);
        }

        ResetDownButtonState();
      }
    break;

    default:
      // To avoid false triggering reset the state
      ResetDownButtonState();
    break;
  }

  // Enter Button state machine
  switch(uI.enterButtonState) {
    case BUTTON_STATUS_IDLE:
      // If ENTER button is pressed
      if ( EnterButtonDigIn_Read() == false) {
        // If debounce timer expires switch the state
        if ( uI.enterButtonDebounceTimer-- == 0) {
          uI.enterButtonState = BUTTON_STATUS_PRESSED;

          // Store the edited temperature when display is flashing
          if ( uI.displayState == DISPLAY_STATUS_FLASHING) {
            // Enable the display flag to continue the display
            uI.flags.displayEnFLG = 1;

            // If user edited the temperature setting, store it in flash
            if (uI.flags.editedFLG) {
              uI.flags.editedFLG = 0;

              nonVol.settings.targetTemperature = uI.targetTemperature;

              nonVol.write();

              // Update the target temperature ADC counts
              NonVolUpdateTargetTemperature();
            }
            uI.displayState = DISPLAY_STATUS_NORMAL;
          }
          // Ignore the button press at power ON
          else if ( uI.displayState == DISPLAY_STATUS_POWER_ON){
            uI.enterButtonState = BUTTON_STATUS_IDLE;
          }
          // Ignore Button Press when display is off and turn on display
          else if ( uI.displayState == DISPLAY_STATUS_OFF){
            uI.enterButtonState = BUTTON_STATUS_IDLE;
            uI.displayState = DISPLAY_STATUS_NORMAL;
//            uI.displayONTimerW = DISPLAY_ON_TIME;
            uI.flags.displayEnFLG = 1;
          }
          uI.displayONTimerW = DISPLAY_ON_TIME;
        }
      }
      // If ENTER button is not pressed
      else {
        uI.enterButtonDebounceTimer = BUTTON_DEBOUNCE_TIME;
      }
    break;

    case BUTTON_STATUS_PRESSED:
      // If ENTER button is still pressed
      if ( EnterButtonDigIn_Read() == false) {
        // Down count the long press timer
        if ( uI.enterButtonLongPressTimerW-- == 0) {
          uI.enterButtonLongPressTimerW = BUTTON_LONG_PRESS_TIME;
          uI.enterButtonState = BUTTON_STATUS_LONG_PRESSED;
        }
      }
      // If ENTER button is released
      else {
        // Reset the status
        ResetEnterButtonState();
      }
    break;

    case BUTTON_STATUS_LONG_PRESSED:
      // If ENTER button is still pressed
      if ( EnterButtonDigIn_Read() == false) {
        // If long press detected during normal display
        if ( uI.displayState == DISPLAY_STATUS_NORMAL) {
          // Standby heat can be enabled or disabled only in Lavatory mode
          if( nonVol.settings.temperatureMode == LAVATORY_MODE) {
            // Toggle the standby heat operation setting
            if ( nonVol.settings.flags.standbyHeatEnFLG == 0) {
              nonVol.settings.flags.standbyHeatEnFLG = 1;
            }
            else {
              nonVol.settings.flags.standbyHeatEnFLG = 0;
            }

            // Store it into non volatile memory
            nonVol.write();

            uI.DigitConvert(uI.targetTemperature);
          }
        }
        uI.enterButtonMalfunctionTimerW = BUTTON_MALFUNCTION_TIME;
        uI.enterButtonState = BUTTON_STATUS_WAIT_FOR_RELEASE;
      }
      // If ENTER button released, reset the state
      else {
        ResetEnterButtonState();
      }
    break;

    case BUTTON_STATUS_WAIT_FOR_RELEASE:
      // If ENTER button still pressed, down count the malfunction timer
      if ( EnterButtonDigIn_Read() == false) {
        // When timer expired, report error and move to malfunction state
        if ( --uI.enterButtonMalfunctionTimerW == 0) {
          faultIndication.Error(BUTTON_MALFUNCTION_ERROR);
          uI.enterButtonState = BUTTON_STATUS_MALFUNCTION;
        }
      }
      // If DOWN button released, reset the state
      else {
        ResetEnterButtonState();
      }
    break;

    case BUTTON_STATUS_MALFUNCTION:
      if ( EnterButtonDigIn_Read() == true) {
        // Clear the error when other buttons are not in malfunction state
        if ( (uI.upButtonState != BUTTON_STATUS_MALFUNCTION) && \
                (uI.downButtonState != BUTTON_STATUS_MALFUNCTION)) {
          faultIndication.Clear(BUTTON_MALFUNCTION_ERROR);
        }

        ResetEnterButtonState();
      }
    break;

    default:
      // To avoid false triggering reset the state
      ResetEnterButtonState();
    break;
  }
}


/*
================================================================================
Method name:  UserInterface
                    
Originator:   Poorana kumar G

Description: 
  Call periodically from Scheduler (1msec), to provide the user interface.
  Continuously check the buttons status and refresh the 7 segment display.

  This method should be called using uI.UIFunction().

Resources:
  3 GPIOs for 3 buttons
  3 GPIOs for control the 3 digits ON/OFF
  8 GPIOs for control each segments

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                            Poorana kumar G
1.1.0  01-30-2020  Power on display sequence is changed as  Poorana kumar G
                   per the beta changes.
2.1.0  05-14-2020  Error display method is changed as per   Poorana kumar G
                   the requirement changes.
--------------------------------------------------------------------------------
*/


bool UserInterface(void)
{
  // Read the buttons status through state machine
  uI.Buttons();

  // Power ON Display routine
  if ( uI.displayState == DISPLAY_STATUS_POWER_ON) {
    // Down count the timer
    if ( uI.powerONDisplayTimerW != 0) {
        --uI.powerONDisplayTimerW;
    }
    else {
      // Reload the time once it is expired
      uI.powerONDisplayTimerW = DISPLAY_POWER_ON_CYCLE_TIME;

      switch (uI.powerONDigitState) {
        case 0:
          // 1st digit displayed as '8' and rest digits are OFF
          uI.RawData(sevenSegCode[8], ALL_SEG_OFF, ALL_SEG_OFF);
        break;

        case 1:
          // 2nd digit displayed as '8' and rest digits are OFF
          uI.RawData(ALL_SEG_OFF, sevenSegCode[8], ALL_SEG_OFF);
        break;

        case 2:
          // 3rd digit displayed as '8' and rest digits are OFF
          uI.RawData(ALL_SEG_OFF, ALL_SEG_OFF, sevenSegCode[8]);
        break;

        case 3:
          // 1st digit's dot alone ON and rest digits are OFF
          uI.RawData(DOT_ON, ALL_SEG_OFF, ALL_SEG_OFF);
        break;

        case 4:
          // 2nd digit's dot alone ON and rest digits are OFF
          uI.RawData(ALL_SEG_OFF, DOT_ON, ALL_SEG_OFF);
        break;

        case 5:
         // 3rd digit's dot alone ON and rest digits are OFF
         uI.RawData(ALL_SEG_OFF, ALL_SEG_OFF, DOT_ON);
        break;

        default:
          // Switch the status to NORMAL
          uI.displayState = DISPLAY_STATUS_NORMAL;

          // Convert the temperature into 7 segment digits
          uI.DigitConvert(uI.targetTemperature);
        break;
      }
      // Move to next state
      uI.powerONDigitState++;
    }
  }
  else if ( faultIndication.faultCount != NO_FAULTS) {

        if((faultIndication.faultCount == 1) && (faultIndication.faultsListARY[uI.faultIndex] == OVER_HEAT_ERROR) && \
            (flowDetector.flags.flowDetectedFLG == false))
        {
            if ( uI.displayState == DISPLAY_STATUS_OFF) {
                uI.flags.errorDisplayFLG = 0;
            }
            else if(uI.displayONTimerW != 0) {
                uI.DigitConvert(uI.targetTemperature);
                uI.displayONTimerW--;
            }
            else {
                uI.displayState = DISPLAY_STATUS_OFF;
                uI.displayONTimerW = DISPLAY_ON_TIME;
                uI.flags.displayEnFLG = 0;
                CelsiusLEDDigOut_OFF();
                FahrenheitLEDDigOut_OFF();
            }
        }
        else {
            uI.displayONTimerW = DISPLAY_ON_TIME;

            // If Display is OFF and Error Occurs will turn ON Display for no time limit to show error
            if ( uI.displayState == DISPLAY_STATUS_OFF) {
                uI.flags.displayEnFLG = 1;
                uI.flags.errorDisplayFLG = 0;
                uI.displayState = DISPLAY_STATUS_NORMAL;
//                uI.displayONTimerW = DISPLAY_ON_TIME;
            }
        }
        // Down count the timer
        if ( uI.faultIndicationTimerW) {
          uI.faultIndicationTimerW--;
        }
        else {
            // Reload the timer
            uI.faultIndicationTimerW = FAULT_INDICATION_TIME;

            // Toggle the flag status
            uI.flags.errorDisplayFLG ^= 1;

            //     Dont display error if single fault of over heat error occurs in flow condition off
          if((faultIndication.faultCount == 1) && (faultIndication.faultsListARY[uI.faultIndex] == OVER_HEAT_ERROR) && \
                  (flowDetector.flags.flowDetectedFLG == false))  
          {
              uI.DigitConvert(uI.targetTemperature);
          }
          else 
          {
            if((faultIndication.faultsListARY[uI.faultIndex] == OVER_HEAT_ERROR) && \
                (flowDetector.flags.flowDetectedFLG == false))  
            {
                uI.faultIndex++;
            }
            if ( uI.flags.errorDisplayFLG == 1) 
            {
                // Display the string "Err"
                uI.RawData(SEVEN_SEG_CODE_FOR_E, SEVEN_SEG_CODE_FOR_r, SEVEN_SEG_CODE_FOR_r);
            }
            else 
            {
                // Convert the error code into seven segment data
                Errors_ETYP errorID = faultIndication.faultsListARY[uI.faultIndex];
                uI.ErrorConvert(errorCodesDisplayARY[errorID]);
            }

            if ( uI.flags.errorDisplayFLG == 1) {
              // Display the string "Err"
              uI.RawData(SEVEN_SEG_CODE_FOR_E, SEVEN_SEG_CODE_FOR_r, SEVEN_SEG_CODE_FOR_r);
            }
            else {
            // Convert the error code into seven segment data
            Errors_ETYP errorID = faultIndication.faultsListARY[uI.faultIndex];
            uI.ErrorConvert(errorCodesDisplayARY[errorID]);

//             if ( faultIndication.errorExists(SCALE_DETECTION_ERROR) == false) {
                uI.faultIndex++;
                if ( uI.faultIndex >= faultIndication.faultCount) {
                  uI.faultIndex = 0;
                }
//             }
            }
          }
        }
    }
  // When the user configuration is going on....
  else if ( uI.displayState == DISPLAY_STATUS_FLASHING) {
    // Down count the flashing timer
    if ( --uI.displayFlashTimerW == 0) {
      // Toggle the flag to ON/OFF the display
      uI.flags.displayEnFLG ^= 1;

      //Reload the timer
      uI.displayFlashTimerW = DISPLAY_FLASH_TIME;
    }
    
    if ( --uI.displayFlashExitTimerW == 0) {
      // Exit flashing of the display if no user intervention is found in limited time
      uI.flags.displayEnFLG = 1;
      uI.displayState = DISPLAY_STATUS_NORMAL;
      uI.displayONTimerW = DISPLAY_ON_TIME;
      uI.targetTemperature = uI.tempTargetTemperature;
      uI.DigitConvert(uI.targetTemperature);
      
      //Reload the timer
      uI.displayFlashExitTimerW = DISPLAY_FLASH_EXIT_TIME;
    }
  }
  //  when display is ON wait for 30 secs and turn OFF display
  else if ( uI.displayState == DISPLAY_STATUS_NORMAL) {
    // Down count the timer
    if(uI.displayONTimerW != 0) {
        uI.displayONTimerW--;
    }
    else {
        uI.displayState = DISPLAY_STATUS_OFF;
        uI.displayONTimerW = DISPLAY_ON_TIME;
        uI.flags.displayEnFLG = 0;
        CelsiusLEDDigOut_OFF();
        FahrenheitLEDDigOut_OFF();
    }
     
  }
  else {
    // Nothing To Do
      
//       if ( faultIndication.errorExists(SCALE_DETECTION_ERROR) == false) 
       {
      
        if((flag_Bank1Disable)  ||
           (flag_Bank2Disable))
            {
                if((faultIndication.faultCount == 1) && (faultIndication.faultsListARY[uI.faultIndex] == OVER_HEAT_ERROR) && \
                    (flowDetector.flags.flowDetectedFLG == false))
                {
                    if ( uI.displayState == DISPLAY_STATUS_OFF) {
                        uI.flags.errorDisplayFLG = 0;
                    }
                    else if(uI.displayONTimerW != 0) {
                        uI.DigitConvert(uI.targetTemperature);
                        uI.displayONTimerW--;
                    }
                    else {
                        uI.displayState = DISPLAY_STATUS_OFF;
                        uI.displayONTimerW = DISPLAY_ON_TIME;
                        uI.flags.displayEnFLG = 0;
                        CelsiusLEDDigOut_OFF();
                        FahrenheitLEDDigOut_OFF();
                    }
                }
                else {
                    uI.displayONTimerW = DISPLAY_ON_TIME;

                    // If Display is OFF and Error Occurs will turn ON Display for no time limit to show error
                    if ( uI.displayState == DISPLAY_STATUS_OFF) {
                        uI.flags.displayEnFLG = 1;
                        uI.flags.errorDisplayFLG = 0;
                        uI.displayState = DISPLAY_STATUS_NORMAL;
        //                uI.displayONTimerW = DISPLAY_ON_TIME;
                    }
                }
                // Down count the timer
                if ( uI.faultIndicationTimerW) {
                  uI.faultIndicationTimerW--;
                }
                else {
                    // Reload the timer
                    uI.faultIndicationTimerW = FAULT_INDICATION_TIME;

                    // Toggle the flag status
                    uI.flags.errorDisplayFLG ^= 1;

                    //     Dont display error if single fault of over heat error occurs in flow condition off
                  if((faultIndication.faultCount == 1) && (faultIndication.faultsListARY[uI.faultIndex] == OVER_HEAT_ERROR) && \
                          (flowDetector.flags.flowDetectedFLG == false))  
                  {
                      uI.DigitConvert(uI.targetTemperature);
                  }
                  else 
                  {
                    if((faultIndication.faultsListARY[uI.faultIndex] == OVER_HEAT_ERROR) && \
                        (flowDetector.flags.flowDetectedFLG == false))  
                    {
                        uI.faultIndex++;
                    }
                    if ( uI.flags.errorDisplayFLG == 1) 
                    {
                        // Display the string "Err"
                        uI.RawData(SEVEN_SEG_CODE_FOR_E, SEVEN_SEG_CODE_FOR_r, SEVEN_SEG_CODE_FOR_r);
                    }
                    else 
                    {
                        // Convert the error code into seven segment data
                        Errors_ETYP errorID = faultIndication.faultsListARY[uI.faultIndex];
//                        if(flag_Bank1Disable)
//                            uI.ErrorConvert(errorCodesDisplayARY[31]);
//                        if(flag_Bank2Disable)
//                        else
//                            uI.ErrorConvert(errorCodesDisplayARY[32]);
                    }

                    if ( uI.flags.errorDisplayFLG == 1) {
                      // Display the string "Err"
                      uI.RawData(SEVEN_SEG_CODE_FOR_E, SEVEN_SEG_CODE_FOR_r, SEVEN_SEG_CODE_FOR_r);
                    }
                    else {
                    // Convert the error code into seven segment data
                    Errors_ETYP errorID = faultIndication.faultsListARY[uI.faultIndex];
//                         if(flag_Bank1Disable)
//                            uI.ErrorConvert(errorCodesDisplayARY[31]);
//                        if(flag_Bank2Disable)
//                         else
//                            uI.ErrorConvert(errorCodesDisplayARY[32]);

                        uI.faultIndex++;
                        if ( uI.faultIndex >= faultIndication.faultCount) {
                          uI.faultIndex = 0;
                        }
                    }
                  }
                }
            }
        
       }
  }

  uI.Displaycontrol();
  
  return TASK_COMPLETED;
}

void display_error(uint16_t fault_code)
{
               uI.faultIndicationTimerW = FAULT_INDICATION_TIME;
    switch(flag_err_disp)
    {
        
    
        case 0:
              // Display the string "Err"
              uI.RawData(SEVEN_SEG_CODE_FOR_E, SEVEN_SEG_CODE_FOR_r, SEVEN_SEG_CODE_FOR_r);
              flag_err_disp = 1;
              break;
            
        case 1:
            
                flag_err_disp = 2;
            break;
            
            
        case 2:
            // Convert the error code into seven segment data
//            Errors_ETYP errorID = faultIndication.faultsListARY[uI.faultIndex];
//            uI.ErrorConvert(errorCodesDisplayARY[errorID]);
            uI.ErrorConvert(fault_code);
            flag_err_disp = 3;
            break;
            
        case 3:
                    
                flag_err_disp = 0;
            break;            
           
    }
}