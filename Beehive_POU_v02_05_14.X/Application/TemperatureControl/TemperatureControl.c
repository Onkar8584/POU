/*
================================================================================
File name:    TemperatureControl.h
                    
Originator:   Poorana kumar G

Platform:     dsPIC33EV64GM006
Compiler:     XC16 V1.40
IDE     :     MPLAB X IDE V5.20

Description: 
  The purpose of this object to monitor the temperature of inlet & outlet by
  using the converted ADC value and control the relays used for heaters based
  on the water flow detector input as well as decides the power cycle to be
  provided to the opto couplers using the PID constants & algorithm.

Class Methods:
  void TemperatureControl(void);
    Call periodically from Scheduler (500 msec), to control the temperature of
    inlet and outlet temperature based on the thermistor analog data and flow
    detection using PID algorithm.

  void PIDCalculation(void);
    Called when the the relay control status is "RELAY_CONTROL_CONTROL" to
    calculate the power cycle to be applied to the opto coupler.

Method Calling Requirements:
  tempControl.Control() should be called once per 500 millisecond in
  scheduler.

Resources:
  2 GPIOs for 2 Relays

IoTranslate requirements:
  #define RelayControl1DigOut_ON()      // To turn ON the Relay1
  #define RelayControl1DigOut_OFF()     // To turn OFF the Relay1
  #define RelayControl2DigOut_ON()      // To turn ON the Relay2
  #define RelayControl2DigOut_OFF()     // To turn OFF the Relay2

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-09-2019  Initial Write                        Poorana kumar G
       24-10-2019  A condition is added to check        Poorana kumar G
                   standby en/dis when changing the
                   relay state from RELAY_CONTROL_SHUTDOWN
                   after 45 seconds time.
       11-04-2019  PC-Lint warnings are cleared.        Poorana kumar G
1.1.0  01-30-2020  As per the Beta requirement changes  Poorana kumar G
                   updated the functions
--------------------------------------------------------------------------------
 */

#include "TemperatureControl.h"

/*
================================================================================
Method name:  isAnyChamberTempAboveTarget
                    
Originator:   Poorana kumar G

Description: 
  Call the function to check any one of the detected chamber temperature is
  above target temperature. Returns 1 if any chamber temperature is above the
  target. Otherwise it returns 0.

  This method should be called using isAnyChamberTempAboveTarget().

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-14-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
 */

static bool
isAnyChamberTempAboveTarget (void)
{
  bool retVal = false;

  // Check that chamber 1 temperature is above target
  if ((adcRead.flags.thermistor1DetectedFLG == true) &&        \
          (tempControl.temperatureARYW[CHAMBER_TEMPERATURE1] >= tempControl.targetADCHalfUnitsW))
    {
      retVal = true;
    }

  // Check that chamber 2 temperature is above target
  if ((adcRead.flags.thermistor2DetectedFLG == true) &&        \
          (tempControl.temperatureARYW[CHAMBER_TEMPERATURE2] >= tempControl.targetADCHalfUnitsW))
    {
      retVal = true;
    }

  // Check that chamber 3 temperature is above target
  if ((adcRead.flags.thermistor3DetectedFLG == true) &&        \
          (tempControl.temperatureARYW[CHAMBER_TEMPERATURE3] >= tempControl.targetADCHalfUnitsW))
    {
      retVal = true;
    }

  // Check that chamber 4 temperature is above target
  if ((adcRead.flags.thermistor4DetectedFLG == true) &&        \
          (tempControl.temperatureARYW[CHAMBER_TEMPERATURE4] >= tempControl.targetADCHalfUnitsW))
    {
      retVal = true;
    }

  return retVal;
}

/*
================================================================================
Method name:  checkDryFireEvent
                    
Originator:   Poorana kumar G

Description: 
  Call the function to check all the detected chamber temperature rise.
  Returns 1 if any 1 of the chamber temperature rising faster. Otherwise
  it returns 0.

  This method should be called using checkDryFireEvent().

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       09-14-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
 */

static bool
checkDryFireEvent (void)
{
  bool retVal = false;

  // Check the chamber 1 temperature rise
  if ((adcRead.flags.thermistor1DetectedFLG == true) &&        \
          (Chamber1Rise >= tempControl.dryFireThresholdW))
    {
      retVal = true;
    }

  // Check the chamber 2 temperature rise
  if ((adcRead.flags.thermistor2DetectedFLG == true) &&        \
          (Chamber2Rise >= tempControl.dryFireThresholdW))
    {
      retVal = true;
    }

  // Check the chamber 3 temperature rise
  if ((adcRead.flags.thermistor3DetectedFLG == true) &&        \
          (Chamber3Rise >= tempControl.dryFireThresholdW))
    {
      retVal = true;
    }

  // Check the chamber 4 temperature rise
  if ((adcRead.flags.thermistor4DetectedFLG == true) &&        \
          (Chamber4Rise >= tempControl.dryFireThresholdW))
    {
      retVal = true;
    }

  return retVal;
}

/*
================================================================================
Method name:  TemperatureControl
                    
Originator:   Poorana kumar G

Description: 
  Call periodically from Scheduler (500 msec), to control the temperature of
  inlet and outlet temperature based on the thermistor analog data and flow
  detection using PID algorithm.

  This method should be called using tempControl.Control().

Resources:
  2 GPIOs for 2 Relays

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-09-2019  Initial Write                            Poorana kumar G
       24-10-2019  A condition is added to check standby    Poorana kumar G
                   en/dis when changing the relay state
                   from RELAY_CONTROL_SHUTDOWN after 45
                   seconds time.
       11-04-2019  PC-Lint warnings are cleared by          Poorana kumar G
                   segregating switch cases "default" and 
                   "RELAY_CONTROL_ERROR"
1.1.0  01-30-2020  As per beta requirement a new relay      Poorana kumar G
                   state "RELAY_CONTROL_ERROR_WAIT" is
                   added to wait 30 sec after error cleared.
2.3.0  09-15-2020  Dry Fire detection & Dry fire wait state Poorana kumar G
                   are added. Standby heating is changed to
                   monitor the chamber thermistors.
2.3.1  12-25-2020  Dry Fire detection temp range changed    Dnyaneshwar
                   are added.
                   Relay shut down time changed from 5 mins 
                   to 30 seconds to save power.
--------------------------------------------------------------------------------
 */

bool
TemperatureControl (void)
{
  uint8_t i = 0;

  // Loop through i-0 to 5
  for (i = INLET_TEMPERATURE; i <= CHAMBER_TEMPERATURE4; i++)
    {
      tempControl.temperature2backARYW[i] = tempControl.temperaturePrevARYW[i];
      tempControl.temperaturePrevARYW[i] = tempControl.temperatureARYW[i];
      tempControl.temperatureARYW[i] = adcRead.adcDataARYW[i];

      // Calculate the temperature raise in past 1 second
      tempControl.dtOfAverageARYW[i] = (tempControl.temperatureARYW[i] -   \
            tempControl.temperature2backARYW[i]) / 2;

      // Reset the temperature raise if any error in thermistor
      if ((tempControl.temperature2backARYW[i] < THERMISTOR_OPEN_ADC_COUNT) || \
           (tempControl.temperature2backARYW[i] > THERMISTOR_SHORT_ADC_COUNT))
        {
          tempControl.dtOfAverageARYW[i] = 0;
        }
    }

  // Calculate the change in outlet temperature
  tempControl.outletTemperaturePrevW = tempControl.outletTemperatureW;
  tempControl.outletTemperatureW = Tout;
  tempControl.dtOutletTemperatureW =                                        \
          tempControl.outletTemperatureW - tempControl.outletTemperaturePrevW;

  // Check the any connected thermistor's temperature is above too hot limit
  if ((adcRead.flags.thermistor1DetectedFLG == true) &&                                        \
          (adcRead.adcDataARYW[CHAMBER_TEMPERATURE1] < THERMISTOR_SHORT_ADC_COUNT) &&          \
          (adcRead.adcDataARYW[CHAMBER_TEMPERATURE1] > tempControl.overHeatADCHalfUnits) &&    \
          (faultIndication.errorExists (THERMISTOR3_OPEN_ERROR) == false))
    {
      faultIndication.Error (OVER_HEAT_ERROR);
      tempControl.flags.thermistor1OverHeatFLG = true;
    }
  else if ((adcRead.flags.thermistor2DetectedFLG == true) &&                                   \
          (adcRead.adcDataARYW[CHAMBER_TEMPERATURE2] < THERMISTOR_SHORT_ADC_COUNT) &&          \
          (adcRead.adcDataARYW[CHAMBER_TEMPERATURE2] > tempControl.overHeatADCHalfUnits) &&    \
          (faultIndication.errorExists (THERMISTOR4_OPEN_ERROR) == false))
    {
      faultIndication.Error (OVER_HEAT_ERROR);
      tempControl.flags.thermistor2OverHeatFLG = true;
    }
  else if ((adcRead.flags.thermistor3DetectedFLG == true) &&                                   \
          (adcRead.adcDataARYW[CHAMBER_TEMPERATURE3] < THERMISTOR_SHORT_ADC_COUNT) &&          \
          (adcRead.adcDataARYW[CHAMBER_TEMPERATURE3] > tempControl.overHeatADCHalfUnits) &&    \
          (faultIndication.errorExists (THERMISTOR5_OPEN_ERROR) == false))
    {
      faultIndication.Error (OVER_HEAT_ERROR);
      tempControl.flags.thermistor3OverHeatFLG = true;
    }
  else if ((adcRead.flags.thermistor4DetectedFLG == true) &&                                   \
          (adcRead.adcDataARYW[CHAMBER_TEMPERATURE4] < THERMISTOR_SHORT_ADC_COUNT) &&          \
          (adcRead.adcDataARYW[CHAMBER_TEMPERATURE4] > tempControl.overHeatADCHalfUnits) &&    \
          (faultIndication.errorExists (THERMISTOR6_OPEN_ERROR) == false))
    {
      faultIndication.Error (OVER_HEAT_ERROR);
      tempControl.flags.thermistor4OverHeatFLG = true;
    }
  else
    {
      faultIndication.Clear (OVER_HEAT_ERROR);
      tempControl.flags.thermistor1OverHeatFLG = false;
      tempControl.flags.thermistor2OverHeatFLG = false;
      tempControl.flags.thermistor3OverHeatFLG = false;
      tempControl.flags.thermistor4OverHeatFLG = false;
    }

#ifndef DISABLE_INLET_THERMISTOR
  // Zero the flag. Let algorithm will decide what need to do.
  tempControl.flags.shutDownFLG = 0;

  // Shutdown if Inlet temperature is greater than outlet temperature
  if (((Tin - Tout) > TDiffForShutDown))
    {
      tempControl.flags.shutDownFLG = 1;
    }

  // Shutdown if Inlet temperature is greater than set point
  if (Tin > tempControl.targetADCHalfUnitsW)
    {
      tempControl.flags.shutDownFLG = 1;
    }

  // Reverse flow detection. If Tin is raising faster than Tout during water flow
  if ((faultIndication.faultCount == NO_FAULTS) &&             \
          (flowDetector.flags.flowDetectedFLG) &&               \
          ((TinRise > TinMinimumRiseLimitForSignificant) &&     \
          ((ToutRise) < ToutMaximumRiseLimitForSignificant)))
    {
      // Tin rise is confirmed two consecutive times before report error
      if (tempControl.flags.reverseFlowFLG)
        {
          faultIndication.Error (FLOW_DIRECTION_ERROR);
        }
      else
        {
          tempControl.flags.reverseFlowFLG = 1;
        }
    }
  else
    {
      tempControl.flags.reverseFlowFLG = 0;
    }
#endif

  // If errors in the buffer OFF relay control
  if (faultIndication.faultCount != NO_FAULTS)
    {
      tempControl.relayStatus = RELAY_CONTROL_ERROR;
      tempControl.prevRelayStatus = RELAY_CONTROL_ERROR_WAIT;
    }

  switch (tempControl.relayStatus)
    {
    case RELAY_CONTROL_INITIAL:
      // If water flow detected
      if (flowDetector.flags.flowDetectedFLG)
        {
            // If dry fire timer is expired, start control
            if (tempControl.dryFireWaitTimerW == 0)
            {
              tempControl.relayStatus = RELAY_CONTROL_CONTROL;
              tempControl.prevRelayStatus = RELAY_CONTROL_INITIAL;
            }
            else
            {
              tempControl.relayStatus = RELAY_CONTROL_DRY_FIRE_WAIT;
              tempControl.prevRelayStatus = RELAY_CONTROL_INITIAL;
            }
            // Switch ON both the relays
            RelayControl1DigOut_ON ();
            RelayControl2DigOut_ON ();
        }
        // If no water flow detected
      else
        {
          tempControl.relayStatus = RELAY_CONTROL_STBYCOOL;
          tempControl.prevRelayStatus = RELAY_CONTROL_INITIAL;
        }
      break;

    case RELAY_CONTROL_CONTROL:
      // Switch ON both the relays
      // check lowFlow condition 
      if ((flowDetector.flags.flowDetectedFLG == true) &&       \
              (check_Flow_Threshold () == FLOW_SENSOR_ERROR))
        {
          if (tempControl.flags.lowFlowRelayControlFLG == true)
            { // If relay 1 is already on turn on relay 2
//              RelayControl1DigOut_OFF ();
//              RelayControl2DigOut_ON ();
//              tempControl.flags.lowFlowRelayControlFLG = true;
              RelayControl2DigOut_OFF ();
              RelayControl1DigOut_ON ();
            }
          else if (tempControl.flags.lowFlowRelayControlFLG == false)
            {// If relay 2 is already on turn on relay 1
//              RelayControl2DigOut_OFF ();
//              RelayControl1DigOut_ON ();
//              tempControl.flags.lowFlowRelayControlFLG = false;
              RelayControl1DigOut_OFF ();
              RelayControl2DigOut_ON ();
            }
          tempControl.relayStatus = RELAY_CONTROL_LOWFLOW;
          tempControl.prevRelayStatus = RELAY_CONTROL_CONTROL;
        }
      else
        {
          RelayControl1DigOut_ON ();
          RelayControl2DigOut_ON ();
        }

      // If temperature rise of any detected chambers is huge
      if (checkDryFireEvent () == true)
        {
          // Load the timer and switch to dry fire wait state
          tempControl.dryFireWaitTimerW = DRY_FIRE_WAIT_TIME;
          tempControl.relayStatus = RELAY_CONTROL_DRY_FIRE_WAIT;
          tempControl.prevRelayStatus = RELAY_CONTROL_CONTROL;
        }
        // If water flow stopped or shut down flag is set
      else if ((!flowDetector.flags.flowDetectedFLG) || \
              tempControl.flags.shutDownFLG)
        {
          tempControl.relayStatus = RELAY_CONTROL_SHUTDOWN;
          tempControl.prevRelayStatus = RELAY_CONTROL_CONTROL;
          tempControl.shutDownCounterW = SHUT_DOWN_TIMEOUT;
        }
      else
        {
          // Do Nothing
        }
      break;

    case RELAY_CONTROL_LOWFLOW:
        if ((flowDetector.flags.flowDetectedFLG == true) &&       \
              (check_Flow_Threshold () == NO_ERROR))
        {
            RelayControl1DigOut_ON ();
            RelayControl2DigOut_ON ();
            if (tempControl.dryFireWaitTimerW == 0)
            {
              tempControl.relayStatus = RELAY_CONTROL_CONTROL;
              tempControl.prevRelayStatus = RELAY_CONTROL_LOWFLOW;
            }
            else
            {
              tempControl.relayStatus = RELAY_CONTROL_DRY_FIRE_WAIT;
              tempControl.prevRelayStatus = RELAY_CONTROL_LOWFLOW;
            }
        }
        // If water flow stopped or shut down flag is set
        if ((!flowDetector.flags.flowDetectedFLG) || \
              tempControl.flags.shutDownFLG){
            tempControl.relayStatus = RELAY_CONTROL_SHUTDOWN;
            tempControl.prevRelayStatus = RELAY_CONTROL_LOWFLOW;
            tempControl.shutDownCounterW = SHUT_DOWN_TIMEOUT;
        }
      break;

    case RELAY_CONTROL_SHUTDOWN:
      // Down count the shut down counter
      if (tempControl.shutDownCounterW)
        {
          tempControl.shutDownCounterW--;

          // If water flow detected, decide the mode to be switched
          if (flowDetector.flags.flowDetectedFLG)
            {
              if (tempControl.dryFireWaitTimerW != 0)
                {
                    tempControl.relayStatus = RELAY_CONTROL_DRY_FIRE_WAIT;
                    tempControl.prevRelayStatus = RELAY_CONTROL_SHUTDOWN;
                    RelayControl1DigOut_ON ();
                    RelayControl2DigOut_ON ();
                }
              else if (!tempControl.flags.shutDownFLG)
                {
                    if (check_Flow_Threshold () == FLOW_SENSOR_ERROR)
                    {
                      if (tempControl.flags.lowFlowRelayControlFLG == true) 
                        { // If previously relay 2 or no relay was ON now turn on relay 1
            //              RelayControl1DigOut_OFF ();
            //              RelayControl2DigOut_ON ();
//                          tempControl.flags.lowFlowRelayControlFLG = false;
                          RelayControl2DigOut_OFF ();
                          RelayControl1DigOut_ON ();
                        }
                      else if (tempControl.flags.lowFlowRelayControlFLG == false)
                        {// If previously relay 1  was ON now turn on relay 2
            //              RelayControl2DigOut_OFF ();
            //              RelayControl1DigOut_ON ();
//                          tempControl.flags.lowFlowRelayControlFLG = true;
                          RelayControl1DigOut_OFF ();
                          RelayControl2DigOut_ON ();
                        }
                      tempControl.relayStatus = RELAY_CONTROL_LOWFLOW;
                      tempControl.prevRelayStatus = RELAY_CONTROL_STBYCOOL;
                    }
                    else
                    {
                        tempControl.relayStatus = RELAY_CONTROL_CONTROL;
                        tempControl.prevRelayStatus = RELAY_CONTROL_SHUTDOWN;
                        RelayControl1DigOut_ON ();
                        RelayControl2DigOut_ON ();
                    }
                }
              else
                {
                  // Do Nothing
                }
            }
        }
      else
        {
          // If Standby Heat is enabled by user
          if ((nonVol.settings.flags.standbyHeatEnFLG) &&    \
                (tempControl.dryFireWaitTimerW == 0) &&    \
                (isAnyChamberTempAboveTarget () == false))
            {
              tempControl.relayStatus = RELAY_CONTROL_STBYHEAT;
              tempControl.prevRelayStatus = RELAY_CONTROL_SHUTDOWN;
              
              // Switch ON both the relays
              RelayControl1DigOut_ON ();
              RelayControl2DigOut_ON ();
            }
            // If Standby Heat is disabled by user
          else
            {
              tempControl.relayStatus = RELAY_CONTROL_STBYCOOL;
              tempControl.prevRelayStatus = RELAY_CONTROL_SHUTDOWN;

              // Switch OFF both the relays
              RelayControl1DigOut_OFF ();
              RelayControl2DigOut_OFF ();
            }
        }
      break;

    case RELAY_CONTROL_STBYCOOL:
      // If water flow detected
      if (flowDetector.flags.flowDetectedFLG)
        {
          // If dry fire timer is expired, start control
            if (tempControl.dryFireWaitTimerW == 0)
            {
                if (check_Flow_Threshold () == FLOW_SENSOR_ERROR)
                {
                  if (tempControl.flags.lowFlowRelayControlFLG == false) 
                    { // If previously relay 2 or no relay was ON now turn on relay 1
        //              RelayControl1DigOut_OFF ();
        //              RelayControl2DigOut_ON ();
                      tempControl.flags.lowFlowRelayControlFLG = true;
                      RelayControl2DigOut_OFF ();
                      RelayControl1DigOut_ON ();
                    }
                  else if (tempControl.flags.lowFlowRelayControlFLG == true)
                    {// If previously relay 1  was ON now turn on relay 2
        //              RelayControl2DigOut_OFF ();
        //              RelayControl1DigOut_ON ();
                      tempControl.flags.lowFlowRelayControlFLG = false;
                      RelayControl1DigOut_OFF ();
                      RelayControl2DigOut_ON ();
                    }
                  tempControl.relayStatus = RELAY_CONTROL_LOWFLOW;
                  tempControl.prevRelayStatus = RELAY_CONTROL_STBYCOOL;
                }
                else
                {
                  tempControl.relayStatus = RELAY_CONTROL_CONTROL;
                  tempControl.prevRelayStatus = RELAY_CONTROL_STBYCOOL;
                  // Switch ON both the relays
                  RelayControl1DigOut_ON ();
                  RelayControl2DigOut_ON ();
                }
            }
            else
            {
              tempControl.relayStatus = RELAY_CONTROL_DRY_FIRE_WAIT;
              tempControl.prevRelayStatus = RELAY_CONTROL_STBYCOOL;
              // Switch ON both the relays
              RelayControl1DigOut_ON ();
              RelayControl2DigOut_ON ();
            }
              
            // Switch ON both the relays
            /*RelayControl1DigOut_ON ();
            RelayControl2DigOut_ON ();*/
        }
        // If standby heat is enabled
      else if ((nonVol.settings.flags.standbyHeatEnFLG) && \
              (tempControl.dryFireWaitTimerW == 0) &&      \
              (isAnyChamberTempAboveTarget () == false))
        {
          tempControl.relayStatus = RELAY_CONTROL_STBYHEAT;
          tempControl.prevRelayStatus = RELAY_CONTROL_STBYCOOL;

          // Switch ON both the relays
          RelayControl1DigOut_ON ();
          RelayControl2DigOut_ON ();
        }
      else
        {
          // Switch OFF both the relays
          RelayControl1DigOut_OFF ();
          RelayControl2DigOut_OFF ();
        }
      break;

    case RELAY_CONTROL_STBYHEAT:
      // If temperature rise of any detected chambers is huge
      if (checkDryFireEvent () == true)
        {
          // Load the dry fire timer and switch to stand by cool state
          tempControl.dryFireWaitTimerW = DRY_FIRE_WAIT_TIME;
          tempControl.relayStatus = RELAY_CONTROL_STBYCOOL;
          tempControl.prevRelayStatus = RELAY_CONTROL_STBYHEAT;
        }
        // If water flow detected
      else if (flowDetector.flags.flowDetectedFLG)
        {
          // If dry fire timer is expired, start control
          if (tempControl.dryFireWaitTimerW == 0)
            {
                if (check_Flow_Threshold () == FLOW_SENSOR_ERROR)
                {
                  if (tempControl.flags.lowFlowRelayControlFLG == true) 
                    { // If previously relay 2 or no relay was ON now turn on relay 1
        //              RelayControl1DigOut_OFF ();
        //              RelayControl2DigOut_ON ();
//                          tempControl.flags.lowFlowRelayControlFLG = false;
                      RelayControl2DigOut_OFF ();
                      RelayControl1DigOut_ON ();
                    }
                  else if (tempControl.flags.lowFlowRelayControlFLG == false)
                    {// If previously relay 1  was ON now turn on relay 2
        //              RelayControl2DigOut_OFF ();
        //              RelayControl1DigOut_ON ();
//                          tempControl.flags.lowFlowRelayControlFLG = true;
                      RelayControl1DigOut_OFF ();
                      RelayControl2DigOut_ON ();
                    }
                  tempControl.relayStatus = RELAY_CONTROL_LOWFLOW;
                  tempControl.prevRelayStatus = RELAY_CONTROL_STBYCOOL;
                }
                else
                {  
                    tempControl.relayStatus = RELAY_CONTROL_CONTROL;
                    tempControl.prevRelayStatus = RELAY_CONTROL_STBYHEAT;
                    // Switch ON both the relays
                    RelayControl1DigOut_ON ();
                    RelayControl2DigOut_ON ();
                }
            }
          else
            {
                tempControl.relayStatus = RELAY_CONTROL_DRY_FIRE_WAIT;
                tempControl.prevRelayStatus = RELAY_CONTROL_STBYHEAT;
                // Switch ON both the relays
                RelayControl1DigOut_ON ();
                RelayControl2DigOut_ON ();
            }
        }
        // If target temperature is reached
        // If standby heat is disabled during this state
      else if ((isAnyChamberTempAboveTarget () == true)    \
              || (!nonVol.settings.flags.standbyHeatEnFLG))
        {
          tempControl.relayStatus = RELAY_CONTROL_STBYCOOL;
          tempControl.prevRelayStatus = RELAY_CONTROL_STBYHEAT;

          // Switch OFF both the relays
          RelayControl1DigOut_OFF ();
          RelayControl2DigOut_OFF ();
        }
      else
        {
          // Nothing to do
        }
      break;

    case RELAY_CONTROL_ERROR:
      // If errors cleared
      if (faultIndication.faultCount == NO_FAULTS)
        {
          tempControl.errorWaitCounterW = ERROR_WAIT_TIME;
          tempControl.relayStatus = RELAY_CONTROL_ERROR_WAIT;
          tempControl.prevRelayStatus = RELAY_CONTROL_ERROR;
        }
      else
        {
          // Switch OFF both the relays
          RelayControl1DigOut_OFF ();
          RelayControl2DigOut_OFF ();
        }
      break;

    case RELAY_CONTROL_ERROR_WAIT:
      // Down count the error count timer
      if (tempControl.errorWaitCounterW)
        {
          tempControl.errorWaitCounterW--;
        }
      else
        {
          tempControl.relayStatus = RELAY_CONTROL_INITIAL;
          tempControl.prevRelayStatus = RELAY_CONTROL_ERROR_WAIT;
        }
      break;

    case RELAY_CONTROL_DRY_FIRE_WAIT:
      // If water flow is detected, down count the dry fire timer
      if (flowDetector.flags.flowDetectedFLG)
        {
          if (--tempControl.dryFireWaitTimerW == 0)
            {
              if (check_Flow_Threshold () == FLOW_SENSOR_ERROR)
                {
                  if (tempControl.flags.lowFlowRelayControlFLG == true) 
                    { // If previously relay 2 or no relay was ON now turn on relay 1
        //              RelayControl1DigOut_OFF ();
        //              RelayControl2DigOut_ON ();
//                          tempControl.flags.lowFlowRelayControlFLG = false;
                      RelayControl2DigOut_OFF ();
                      RelayControl1DigOut_ON ();
                    }
                  else if (tempControl.flags.lowFlowRelayControlFLG == false)
                    {// If previously relay 1  was ON now turn on relay 2
        //              RelayControl2DigOut_OFF ();
        //              RelayControl1DigOut_ON ();
//                          tempControl.flags.lowFlowRelayControlFLG = true;
                      RelayControl1DigOut_OFF ();
                      RelayControl2DigOut_ON ();
                    }
                  tempControl.relayStatus = RELAY_CONTROL_LOWFLOW;
                  tempControl.prevRelayStatus = RELAY_CONTROL_STBYCOOL;
                }
                else
                {  
                    tempControl.relayStatus = RELAY_CONTROL_CONTROL;
                    tempControl.prevRelayStatus = RELAY_CONTROL_DRY_FIRE_WAIT;
                    // Switch ON both the relays
                    RelayControl1DigOut_ON ();
                    RelayControl2DigOut_ON ();
                }
              
            }
        }
      else
        {
          // When flow is stopped, just move to shutdown state
          tempControl.shutDownCounterW = SHUT_DOWN_TIMEOUT;
          tempControl.relayStatus = RELAY_CONTROL_SHUTDOWN;
          tempControl.prevRelayStatus = RELAY_CONTROL_DRY_FIRE_WAIT;
        }
      break;

    default:
      // Switch OFF both the relays
      RelayControl1DigOut_OFF ();
      RelayControl2DigOut_OFF ();
      break;
    }

  switch (tempControl.relayStatus)
    {
    case RELAY_CONTROL_CONTROL:
      // Do PID algorithm
      tempControl.PIDFunction ();
      break;

    case RELAY_CONTROL_STBYHEAT:
      // Control heater in power cycle 120
      optoCouplerControl.powerCycle = STANDBY_POWER_CYCLE;
      break;
      
    case RELAY_CONTROL_LOWFLOW:
      // Control heater in Low flow condition
      tempControl.PIDFunction ();
      break;

    default:
      // Keep the Opto-coupler in OFF state
      optoCouplerControl.powerCycle = POWER_CYCLE_OFF;
      tempControl.integralF = 0.0f;
      break;
    }

  return TASK_COMPLETED;
}

/*
================================================================================
Method name:  PIDCalculation
                    
Originator:   Poorana kumar G

Description: 
  Called when the the relay control status is "RELAY_CONTROL_CONTROL" to
  calculate the power cycle to be applied to the opto coupler.

  This method should be called using tempControl.PIDCalculation().

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       10-09-2019  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
 */

void
PIDCalculation (void)
{
  float fpower = 0.0f;
  int16_t errorW = 0;

  errorW = (tempControl.targetADCHalfUnitsW - tempControl.outletTemperatureW) / 2;
  tempControl.integralF = tempControl.integralF + errorW;

  // Limit the integral
  if (tempControl.integralF > eeIntegralLimit)
    {
      tempControl.integralF = eeIntegralLimit;
    }
  if (tempControl.integralF < 0.0f)
    {
      tempControl.integralF = 0.0f;
    }

  // 'P' Term and 'I' Term
  fpower = (PID_CONST_KP * errorW) + (PID_CONST_KI * tempControl.integralF);

  // If target temperature is near reduce the power cycle based on rate of
  // change of outlet temperature
  if (errorW < ErrorLimitForKDToKickIn)
    {
      if (tempControl.dtOutletTemperatureW > 0)
        {
          fpower = fpower - (PID_CONST_KDI * tempControl.dtOutletTemperatureW / 2);
        }
      else
        {
          fpower = fpower - (PID_CONST_KDD * tempControl.dtOutletTemperatureW / 2);
        }
    }

  // This is the preburn, dump in a bunch of power
  // If so cold its below hi_err_thresh AND not increasing "enough"
  if ((errorW * 2 > PID_CONST_HI_ERR_THRESH) &&                            \
          (tempControl.dtOutletTemperatureW < PID_CONST_PREBURN_LIMIT))
    {
      fpower = fpower + (PID_CONST_PREBURN_LIMIT * errorW);
    }

  //If so cold its below hi_err_thresh, back off proportional to the rate,
  // basically more damping,
  if ((errorW * 2) > PID_CONST_HI_ERR_THRESH)
    {
      fpower = fpower -                                                       \
            (PID_CONST_PREBURN_LIMIT * tempControl.dtOutletTemperatureW / 2);
    }

  // Limit the power
  if (fpower > MAXPOWER_POWER_CYCLE)
    {
      fpower = MAXPOWER_POWER_CYCLE;
    }

  // Limit the power
  if (fpower < 0.0f)
    {
      fpower = 0;
    }

  optoCouplerControl.powerCycle = (unsigned char) fpower;
}

/*
================================================================================
Method name:  adcCountToTemperature
                    
Originator:   Poorana kumar G

Description: 
  The purpose of this function is to convert the ADC count into its equivalent
  temperature.

  This method should be called using adcCountToTemperature().

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       05-11-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
 */

uint16_t
adcCountToTemperature (uint16_t adcCount)
{
  if (adcCount <= ADC_COUNT_MIDPOINT)
    {
      return (uint16_t) ((adcCount * DegPerADHalfUnitbelowMid) + DegAt0VoltbelowMid);
    }
  else
    {
      return (uint16_t) (((adcCount - ADC_COUNT_MIDPOINT) * DegPerADHalfUnitAbvoveMid) + DegAt0VoltAbvoveMid);
    }
}

/*
================================================================================
Method name:  temperatureToADCCount
                    
Originator:   Poorana kumar G

Description: 
  The purpose of this function is to convert the temperature into its equivalent
  ADC count.

  This method should be called using adcCountToTemperature().

Resources:
 None

================================================================================
 History:	
-*-----*-----------*------------------------------------*-----------------------
       05-11-2020  Initial Write                        Poorana kumar G
--------------------------------------------------------------------------------
 */

uint16_t
temperatureToADCCount (uint16_t temperature)
{
  if (temperature <= TEMPERATURE_MIDPOINT)
    {
      return (uint16_t) ((temperature - DegAt0VoltbelowMid) / DegPerADHalfUnitbelowMid);
    }
  else
    {
      return (uint16_t) ((temperature + DegAbvoveMidTempConst) / DegPerADHalfUnitAbvoveMid);
    }
}


