/*
================================================================================
File name:    UserInterface.h
                    
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
    Call periodically from Scheduler (1msec), to provide the user interface.
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
  #define DisplayPortDigOut_PlaceData(Data)

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-24-2019  Initial Write                            Poorana kumar G
1.1.0  01-30-2020  As per the Beta requirement changes      Poorana kumar G
                   updated the functions
2.1.0  05-14-2020  Error display method is changed as per   Poorana kumar G
                   the requirement changes.
2.3.0  09-15-2020  Power on display digits ON time is       Poorana kumar G
                   changed as 500ms 
--------------------------------------------------------------------------------
*/

#ifndef _USERINTERFACE_H
#define _USERINTERFACE_H

#include "Build.h"
#include "IoTranslate.h"


// Enums for display status
typedef enum {
  DISPLAY_STATUS_POWER_ON,
  DISPLAY_STATUS_NORMAL,
  DISPLAY_STATUS_FLASHING,
  DISPLAY_STATUS_OFF        
}DispState_ETYP;

// Enums for buttons status
typedef enum {
  BUTTON_STATUS_IDLE,
  BUTTON_STATUS_PRESSED,
  BUTTON_STATUS_LONG_PRESSED,
  BUTTON_STATUS_WAIT_FOR_RELEASE,
  BUTTON_STATUS_MALFUNCTION
}ButtonState_ETYP;

//CLASS OBJECT DEFINITION
typedef struct {
//Public Variables
  struct {
    // To turn OFF the display while flashing 0-OFF 1-ON
    uint8_t displayEnFLG:1;
    // To turn OFF the display while flashing 0-OFF 1-ON
    uint8_t editedFLG:1;
    // To turn OFF the display while flashing 0-OFF 1-ON
    uint8_t errorDisplayFLG:1;
  } flags;
  // Target temperature
  uint16_t targetTemperature;
  // Temp Target temperature
  uint16_t tempTargetTemperature;

// Public Methods
  // The function to be called in scheduler
  bool (*UIFunction)(void);
  void (*ResetUI)(void);

// Private Variables
  // The seven segment data to be displayed in digit 1
  uint8_t digit1Data;
  // The seven segment data to be displayed in digit 2
  uint8_t digit2Data;
  // The seven segment data to be displayed in digit 3
  uint8_t digit3Data;
  // Power ON seqeuence digit state
  uint8_t powerONDigitState;
  // The seven segment displays status
  DispState_ETYP displayState;
  // Display flashing timer
  uint16_t displayFlashTimerW;
  // Display flashing exit timer
  uint16_t displayFlashExitTimerW;
  // Display ON timer
  uint16_t displayONTimerW;
  // Power ON Display timer
  uint16_t powerONDisplayTimerW;
  // Up Button Debounce timer
  uint8_t upButtonDebounceTimer;
  // Down Button Debounce timer
  uint8_t downButtonDebounceTimer;
  // Enter Button Debounce timer
  uint8_t enterButtonDebounceTimer;
  // Up Button status
  ButtonState_ETYP upButtonState;
  // Down Button status
  ButtonState_ETYP downButtonState;
  // Enter Button status
  ButtonState_ETYP enterButtonState;
  // Up button long press timer
  uint16_t upButtonLongPressTimerW;
  // Down button long press timer
  uint16_t downButtonLongPressTimerW;
  // Enter button long press timer
  uint16_t enterButtonLongPressTimerW;
  // Up button malfunction timer
  uint16_t upButtonMalfunctionTimerW;
  // Down button malfunction timer
  uint16_t downButtonMalfunctionTimerW;
  // Enter button malfunction timer
  uint16_t enterButtonMalfunctionTimerW;
  // To speed up increment of setting
  uint16_t incrementTimerW;
  // To speed up decrement of setting
  uint16_t decrementTimerW;
  // Error index of displaying error
  uint8_t faultIndex;
  // Error indication timer
  uint16_t faultIndicationTimerW;

// Private Methods
  void (*DigitConvert) (uint16_t);
  void (*ErrorConvert) (uint16_t);
  void (*RawData)(uint8_t, uint8_t, uint8_t);
  void (*Displaycontrol)(void);
  void (*Buttons)(void);
} UserInterface_STYP;


// DEFINE CLASS OBJECT DEFAULTS
#define USER_INTERFACE_DEFAULTS { {1,0,0},                  \
                                  0,                        \
                                  0,                        \
                                  &UserInterface,           \
                                  &ResetUIUserProgress,     \
                                  0,                        \
                                  0,                        \
                                  0,                        \
                                  0,                        \
                                  DISPLAY_STATUS_POWER_ON,  \
                                  DISPLAY_FLASH_TIME,       \
                                  DISPLAY_FLASH_EXIT_TIME,  \
                                  DISPLAY_ON_TIME,          \
                                  0,                        \
                                  BUTTON_DEBOUNCE_TIME,     \
                                  BUTTON_DEBOUNCE_TIME,     \
                                  BUTTON_DEBOUNCE_TIME,     \
                                  BUTTON_STATUS_IDLE,       \
                                  BUTTON_STATUS_IDLE,       \
                                  BUTTON_STATUS_IDLE,       \
                                  BUTTON_LONG_PRESS_TIME,   \
                                  BUTTON_LONG_PRESS_TIME,   \
                                  BUTTON_LONG_PRESS_TIME,   \
                                  BUTTON_MALFUNCTION_TIME,  \
                                  BUTTON_MALFUNCTION_TIME,  \
                                  BUTTON_MALFUNCTION_TIME,  \
                                  0,                        \
                                  0,                        \
                                  0,                        \
                                  0,                        \
                                  &TemperatureToDigit,      \
                                  &ErrorCodeToDigit,        \
                                  &DisplayRawData,          \
                                  &DisplayFunction,         \
                                  &ButtonsRead,             \
                                }

#define BUTTON_DEBOUNCE_TIME        (20 / USER_INTERFACE_INTERVAL)   // 20 msec
#define BUTTON_LONG_PRESS_TIME      (3000 / USER_INTERFACE_INTERVAL) // 3 sec
#define BUTTON_MALFUNCTION_TIME     (30000 / USER_INTERFACE_INTERVAL) // 30 sec
#define DISPLAY_FLASH_TIME          (1000 / USER_INTERFACE_INTERVAL) // 1 sec
#define DISPLAY_FLASH_EXIT_TIME     (15000 / USER_INTERFACE_INTERVAL) // 15 sec
#define DISPLAY_POWER_ON_CYCLE_TIME (500 / USER_INTERFACE_INTERVAL) // 0.5 sec
#define INCDEC_NORMAL_TIME          (500 / USER_INTERFACE_INTERVAL)  // 500 msec
#define INCDEC_LONGPRESS_TIME       (100 / USER_INTERFACE_INTERVAL)  // 100 msec
#define FAULT_INDICATION_TIME       (2000 / USER_INTERFACE_INTERVAL) // 2 sec
#define DISPLAY_ON_TIME             (30000 / USER_INTERFACE_INTERVAL) // 30 sec

#define DIGIT1_INDEX                1
#define DIGIT2_INDEX                2
#define DIGIT3_INDEX                3
#define DIGITS_OFF_INDEX            4
#define DIGITS_TIMER_MAX            5

#define SEG_A_POSITION              0x01
#define SEG_B_POSITION              0x02
#define SEG_C_POSITION              0x04
#define SEG_D_POSITION              0x08
#define SEG_E_POSITION              0x10
#define SEG_F_POSITION              0x20
#define SEG_G_POSITION              0x40
#define SEG_DOT_POSITION            0x80

#define DOT_ON                      0x7F
#define SEVEN_SEG_CODE_FOR_E        0x86
#define SEVEN_SEG_CODE_FOR_r        0xAF
#define ALL_SEG_OFF                 0xFF

//  CLASS METHOD PROTOTYPES
void TemperatureToDigit(uint16_t temperature);
void DisplayFunction(void);
void DisplayRawData(uint8_t digit1, uint8_t digit2, uint8_t digit3);
void ButtonsRead(void);
bool UserInterface(void);
void ErrorCodeToDigit(uint16_t errorCode);
void ResetUIUserProgress(void);


// EXTERN VARIABLES
extern UserInterface_STYP uI;

#endif /* _USERINTERFACE_H */

