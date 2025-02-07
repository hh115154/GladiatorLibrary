/**************************************************************************************************
  Filename:       cyclingSensor.c
  Revised:        $Date: 2013-08-15 15:28:40 -0700 (Thu, 15 Aug 2013) $
  Revision:       $Revision: 34986 $

  Description:    This file contains the Cycling Speed and Cadence (CSC) sample
                  application for use with the Bluetooth Low Energy Protocol Stack.

  Copyright 2012-2013 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "OSAL.h"
#include "OnBoard.h"
#include "hal_led.h"
#include "hal_key.h"
#include "linkdb.h"
#include "gatt.h"
#include "hci.h"
#include "gapgattserver.h"
#include "gattservapp.h"
#include "gatt_profile_uuid.h"
#include "cyclingservice.h"
#include "devinfoservice.h"
#include "peripheral.h"
#include "gapbondmgr.h"
#include "cyclingSensor.h"

/*********************************************************************
 * MACROS
 */



/*********************************************************************
 * CONSTANTS
 */

// Fast advertising interval in 625us units.  625*32 = 20ms (recommended)
#define DEFAULT_FAST_ADV_INTERVAL                32

// Duration of fast advertising duration in ms
#define DEFAULT_FAST_ADV_DURATION                20000

// Duration of advertising to white list members only after link termination
#define DEFAULT_WHITE_LIST_ADV_DURATION          10000

// Slow advertising interval in 625us units.  625*1704 = 1065ms (recommended)
#define DEFAULT_SLOW_ADV_INTERVAL                1704

// Duration of slow advertising duration in ms (set to 0 for continuous advertising)
#define DEFAULT_SLOW_ADV_DURATION                20000

// How often to perform sensor's periodic event (ms)
#define DEFAULT_CSC_PERIOD                       1000

// Whether to enable automatic parameter update request when a connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST            FALSE

// Minimum connection interval (units of 1.25ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL        200

// Maximum connection interval (units of 1.25ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL        1600

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY            0

// Supervision timeout value (units of 10ms) if automatic parameter update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT             1000

// Sensor sends a slave security request.
#define DEFAULT_PAIRING_PARAMETER                GAPBOND_PAIRING_MODE_INITIATE

// Bonded devices' addresses are stored in white list.
#define USING_WHITE_LIST                         TRUE

// Request bonding.
#define REQ_BONDING                              TRUE

// Time alloted for service discovery before requesting more energy efficient connection parameters
#define SVC_DISC_DELAY                           5000

// After 15 seconds of no user input with notifications off, terminate connection
#define NEGLECT_TIMEOUT_DELAY                    15000

// Setting this to true lets this device disconnect after a period of no use.
#define USING_NEGLECT_TIMEOUT                    TRUE

// delay for reset of device's bonds, connections, alerts
#define CSC_RESET_DELAY                          3000 // in ms, 3 seconds

// For simulated measurements
#define FLAGS_IDX_MAX                            4

// values for simulated measurements. units in revolutions
#define CRANK_REV_INCREMENT                      2
#define WHEEL_REV_INCREMENT                      2

// values for simulated measurements.  units 1/1024 seconds.  512 * (1/1024) = 500 milliseconds
#define WHEEL_EVT_INCREMENT                      512
#define CRANK_EVT_INCREMENT                      512

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 sensor_TaskID;   // Task ID for internal task/event processing

static gaprole_States_t gapProfileState = GAPROLE_INIT;

static uint8 sensorUsingWhiteList = FALSE;

// GAP Profile - Name attribute for SCAN RSP data
static uint8 scanData[] =
{
  0x0B,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'C',
  'S',
  'C',
  ' ',
  'S',
  'e',
  'n',
  's',
  'o',
  'r',

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm, default Tx power level
};

static uint8 advertData[] =
{
  // flags
  0x02,
  GAP_ADTYPE_FLAGS,
  GAP_ADTYPE_FLAGS_GENERAL | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
  // service UUIDs
  0x03,
  GAP_ADTYPE_16BIT_MORE,
  LO_UINT16(CSC_SERV_UUID),
  HI_UINT16(CSC_SERV_UUID),
};

// Device name attribute value
static uint8 attDeviceName[GAP_DEVICE_NAME_LEN] = "CSC Sensor";

// GAP connection handle
static uint16 gapConnHandle;

// Cycling measurement value stored in this structure
static attHandleValueNoti_t sensorMeas;

// Flags for simulated measurements
static const uint8 sensorFlags[FLAGS_IDX_MAX] =
{
  CSC_FLAGS_SPEED_CADENCE,
  CSC_FLAGS_AT_REST,
  CSC_FLAGS_SPEED,
  CSC_FLAGS_CADENCE
};

// Flag index
static uint8 sensorFlagsIdx = 0;

// Advertising user-cancelled state
static bool sensorAdvCancelled = FALSE;

// CSC parameters
uint32 cummWheelRevs = 100;
uint16 cummCrankRevs = 50;
uint16 lastWheelEvtTime = 60;
uint16 lastCrankEvtTime = 70;
uint8 sensorLocation = CSC_SENSOR_LOC_OTHER;

// Used to determine if a reset delay is in progress
static uint8 resetInProgress = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void sensor_ProcessOSALMsg( osal_event_hdr_t *pMsg );
static void SensorGapStateCB( gaprole_States_t newState );
static void sensorPeriodicTask( void );
static void sensor_HandleKeys( uint8 shift, uint8 keys );
static void sensorMeasNotify( void );
static void SensorCB( uint8 event, uint32 *pNewCummVal );

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t cyclingPeripheralCB =
{
  SensorGapStateCB,       // Profile State Change Callbacks
  NULL                    // When a valid RSSI is read from controller
};

// Bond Manager Callbacks
static const gapBondCBs_t cyclingBondCB =
{
  NULL,                   // Passcode callback
  NULL                    // Pairing state callback
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      CyclingSensor_Init
 *
 * @brief   Initialization function for the Cycling App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void CyclingSensor_Init( uint8 task_id )
{
  sensor_TaskID = task_id;

  // Setup the GAP Peripheral Role Profile
  {
    // For the CC2540DK-MINI keyfob, device doesn't start advertising until button is pressed
    uint8 initial_advertising_enable = FALSE;

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16 gapRole_AdvertOffTime = 0;

    uint8 enable_update_request = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16 desired_min_interval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16 desired_max_interval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16 desired_slave_latency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16 desired_conn_timeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the GAP Role Parameters
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &initial_advertising_enable );
    GAPRole_SetParameter( GAPROLE_ADVERT_OFF_TIME, sizeof( uint16 ), &gapRole_AdvertOffTime );

    GAPRole_SetParameter( GAPROLE_SCAN_RSP_DATA, sizeof ( scanData ), scanData );
    GAPRole_SetParameter( GAPROLE_ADVERT_DATA, sizeof( advertData ), advertData );

    GAPRole_SetParameter( GAPROLE_PARAM_UPDATE_ENABLE, sizeof( uint8 ), &enable_update_request );
    GAPRole_SetParameter( GAPROLE_MIN_CONN_INTERVAL, sizeof( uint16 ), &desired_min_interval );
    GAPRole_SetParameter( GAPROLE_MAX_CONN_INTERVAL, sizeof( uint16 ), &desired_max_interval );
    GAPRole_SetParameter( GAPROLE_SLAVE_LATENCY, sizeof( uint16 ), &desired_slave_latency );
    GAPRole_SetParameter( GAPROLE_TIMEOUT_MULTIPLIER, sizeof( uint16 ), &desired_conn_timeout );
  }

  // Set the GAP Characteristics
  GGS_SetParameter( GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName );

  // Setup the GAP Bond Manager
  {
    uint32 passkey = 0; // passkey "000000"
    uint8 pairMode = DEFAULT_PAIRING_PARAMETER;
    uint8 mitm = FALSE;
    uint8 ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8 bonding = REQ_BONDING;
    uint8 autoSync = USING_WHITE_LIST;

    GAPBondMgr_SetParameter( GAPBOND_DEFAULT_PASSCODE, sizeof ( uint32 ), &passkey );
    GAPBondMgr_SetParameter( GAPBOND_PAIRING_MODE, sizeof ( uint8 ), &pairMode );
    GAPBondMgr_SetParameter( GAPBOND_MITM_PROTECTION, sizeof ( uint8 ), &mitm );
    GAPBondMgr_SetParameter( GAPBOND_IO_CAPABILITIES, sizeof ( uint8 ), &ioCap );
    GAPBondMgr_SetParameter( GAPBOND_BONDING_ENABLED, sizeof ( uint8 ), &bonding );
    GAPBondMgr_SetParameter( GAPBOND_AUTO_SYNC_WL, sizeof ( uint8 ), &autoSync );
  }

  // Initialize GATT attributes
  GGS_AddService( GATT_ALL_SERVICES );         // GAP
  GATTServApp_AddService( GATT_ALL_SERVICES ); // GATT attributes
  Cycling_AddService( GATT_ALL_SERVICES );
  DevInfo_AddService( );

  // Register for cycling service callback
  Cycling_Register( SensorCB );

  // Setup CSC profile data
  {
    uint8 features = CSC_FULL_SUPPORT;
    uint8 sensorLocationCurrent = CSC_SENSOR_LOC_REAR_DROPOUT;
    uint8 sensorLocation1 = CSC_SENSOR_LOC_REAR_DROPOUT;
    uint8 sensorLocation2 = CSC_SENSOR_LOC_LEFT_CRANK;
    uint8 sensorLocation3 = CSC_SENSOR_LOC_REAR_WHEEL;
    uint8 sensorLocation4 = CSC_SENSOR_LOC_HIP;

    // Add available sensor locations
    Cycling_SetParameter(CSC_AVAIL_SENS_LOCS, 1, &sensorLocation1);
    Cycling_SetParameter(CSC_AVAIL_SENS_LOCS, 1, &sensorLocation2);
    Cycling_SetParameter(CSC_AVAIL_SENS_LOCS, 1, &sensorLocation3);
    Cycling_SetParameter(CSC_AVAIL_SENS_LOCS, 1, &sensorLocation4);

    // Set sensor location
    Cycling_SetParameter(CSC_SENS_LOC, 1, &sensorLocationCurrent);

    // Support all features
    Cycling_SetParameter(CSC_FEATURE, 1,  &features);
  }

  // Register for all key events - This app will handle all key events
  RegisterForKeys( sensor_TaskID );

  // makes sure LEDs are off
  HalLedSet( (HAL_LED_1 | HAL_LED_2), HAL_LED_MODE_OFF );

  // For keyfob board set GPIO pins into a power-optimized state
  // Note that there is still some leakage current from the buzzer,
  // accelerometer, LEDs, and buttons on the PCB.

  P0SEL = 0; // Configure Port 0 as GPIO
  P1SEL = 0; // Configure Port 1 as GPIO
  P2SEL = 0; // Configure Port 2 as GPIO

  P0DIR = 0xFC; // Port 0 pins P0.0 and P0.1 as input (buttons),
                // all others (P0.2-P0.7) as output
  P1DIR = 0xFF; // All port 1 pins (P1.0-P1.7) as output
  P2DIR = 0x1F; // All port 1 pins (P2.0-P2.4) as output

  P0 = 0x03; // All pins on port 0 to low except for P0.0 and P0.1 (buttons)
  P1 = 0;   // All pins on port 1 to low
  P2 = 0;   // All pins on port 2 to low

  // Setup a delayed profile startup
  osal_set_event( sensor_TaskID, START_DEVICE_EVT );
}

/*********************************************************************
 * @fn      CyclingSensor_ProcessEvent
 *
 * @brief   Cycling Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16 CyclingSensor_ProcessEvent( uint8 task_id, uint16 events )
{

  VOID task_id; // OSAL required parameter that isn't used in this function

  if ( events & SYS_EVENT_MSG )
  {
    uint8 *pMsg;

    if ( (pMsg = osal_msg_receive( sensor_TaskID )) != NULL )
    {
      sensor_ProcessOSALMsg( (osal_event_hdr_t *)pMsg );

      // Release the OSAL message
      VOID osal_msg_deallocate( pMsg );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & START_DEVICE_EVT )
  {
    // Start the Device
    VOID GAPRole_StartDevice( &cyclingPeripheralCB );

    // Register with bond manager after starting device
    GAPBondMgr_Register( (gapBondCBs_t *) &cyclingBondCB );

    return ( events ^ START_DEVICE_EVT );
  }

  if ( events & CSC_PERIODIC_EVT )
  {
    // Perform sensor's periodic task
    sensorPeriodicTask();

    return (events ^ CSC_PERIODIC_EVT);
  }

  if ( events & CSC_CONN_PARAM_UPDATE_EVT )
  {
    // Send param update.  If it fails, retry until successful.
    GAPRole_SendUpdateParam( DEFAULT_DESIRED_MIN_CONN_INTERVAL, DEFAULT_DESIRED_MAX_CONN_INTERVAL,
                             DEFAULT_DESIRED_SLAVE_LATENCY, DEFAULT_DESIRED_CONN_TIMEOUT,
                             GAPROLE_RESEND_PARAM_UPDATE );

    // Assuming service discovery complete, start neglect timer
    if ( USING_NEGLECT_TIMEOUT )
    {
      osal_start_timerEx( sensor_TaskID, CSC_NEGLECT_TIMEOUT_EVT, NEGLECT_TIMEOUT_DELAY );
    }

    return (events ^ CSC_CONN_PARAM_UPDATE_EVT);
  }

  if ( events & CSC_NEGLECT_TIMEOUT_EVT )
  {
    // No user input, terminate connection.
    GAPRole_TerminateConnection();

    return ( events ^ CSC_NEGLECT_TIMEOUT_EVT );
  }

  if ( events & CSC_RESET_EVT )
  {
    // Soft reset in action
    if ( gapProfileState == GAPROLE_CONNECTED )
    {
      // Exit the connection
      GAPRole_TerminateConnection();

      // There is no callback for manual termination of the link.  change state variable here.
      gapProfileState = GAPROLE_WAITING;

      // Set timer to give the end advertising event time to finish
      osal_start_timerEx( sensor_TaskID, CSC_RESET_EVT, 500 );
    }
    else if ( gapProfileState == GAPROLE_ADVERTISING )
    {
      uint8 value = FALSE;

      // Turn off advertising
      GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &value );

      // Set timer to give the end advertising event time to finish
      osal_start_timerEx( sensor_TaskID, CSC_RESET_EVT, 500 );
    }
    else if ( USING_WHITE_LIST == TRUE )
    {
      //temporary variable
      uint8 value = GAP_FILTER_POLICY_ALL;

      // Turn off white list filter policy
      GAPRole_SetParameter(GAPROLE_ADV_FILTER_POLICY, sizeof( uint8 ), &value);

      sensorUsingWhiteList = FALSE;

      // Clear the white list
      HCI_LE_ClearWhiteListCmd();

      // Set timer to give the end advertising event time to finish
      osal_start_timerEx( sensor_TaskID, CSC_RESET_EVT, 500 );
    }
    else if ( (gapProfileState == GAPROLE_STARTED) ||
              (gapProfileState == GAPROLE_WAITING) ||
              (gapProfileState == GAPROLE_WAITING_AFTER_TIMEOUT) )
    {
      uint8 eraseBonds = TRUE;

      // Erase all bonds
      GAPBondMgr_SetParameter( GAPBOND_ERASE_ALLBONDS, 0, &eraseBonds );

      // Turn on GREEN LED for set time
      HalLedSet( HAL_LED_1, HAL_LED_MODE_BLINK );
    }

    return (events ^ CSC_RESET_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * @fn      sensor_ProcessOSALMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void sensor_ProcessOSALMsg( osal_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
    case KEY_CHANGE:
      sensor_HandleKeys( ((keyChange_t *)pMsg)->state, ((keyChange_t *)pMsg)->keys );
      break;
  }
}

/*********************************************************************
 * @fn      sensor_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void sensor_HandleKeys( uint8 shift, uint8 keys )
{

  if ( keys == ( HAL_KEY_SW_1 | HAL_KEY_SW_2 ) )
  {
    // Reset in progress has started
    resetInProgress = TRUE;

    // Set OSAL timer for reset
    osal_start_timerEx( sensor_TaskID, CSC_RESET_EVT, CSC_RESET_DELAY );
  }
  else if ( keys & HAL_KEY_SW_1 )
  {
    if ( resetInProgress == TRUE )
    {
      // Cancel the reset
      resetInProgress = FALSE;
      osal_stop_timerEx ( sensor_TaskID, CSC_RESET_EVT );
    }

    // set simulated measurement flag index
    if (++sensorFlagsIdx == FLAGS_IDX_MAX)
    {
      sensorFlagsIdx = 0;
    }
  }
  else if ( keys & HAL_KEY_SW_2 )
  {
    if ( resetInProgress == TRUE )
    {
      // Cancel the reset
      resetInProgress = FALSE;
      osal_stop_timerEx ( sensor_TaskID, CSC_RESET_EVT );
    }

    // if not in a connection, toggle advertising on and off
    if ( gapProfileState != GAPROLE_CONNECTED )
    {
      uint8 status;

      // Set fast advertising interval for user-initiated connections
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, DEFAULT_FAST_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, DEFAULT_FAST_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_MIN, DEFAULT_WHITE_LIST_ADV_DURATION );

      // toggle GAP advertisement status
      GAPRole_GetParameter( GAPROLE_ADVERT_ENABLED, &status );
      status = !status;

      // If not already using white list, begin to do so.
      // Only do so if about to begin advertising
      if ( USING_WHITE_LIST && status == TRUE )
      {
        uint8 bondCount = 0;

        GAPBondMgr_GetParameter( GAPBOND_BOND_COUNT, &bondCount );

        if ((sensorUsingWhiteList == FALSE) && (bondCount > 0) )
        {
          uint8 value = GAP_FILTER_POLICY_WHITE;

          GAPRole_SetParameter(GAPROLE_ADV_FILTER_POLICY, sizeof( uint8 ), &value);

          sensorUsingWhiteList = TRUE;
        }
      }

      GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &status );

      // Set state variable
      if (status == FALSE)
      {
        sensorAdvCancelled = TRUE;
      }
    }
  }
  // End of key press
  else if ( keys == 0x00 )
  {
    if ( resetInProgress == TRUE )
    {
      resetInProgress = FALSE;

      osal_stop_timerEx( sensor_TaskID, CSC_RESET_EVT );
    }
  }
}

/*********************************************************************
 * @fn      sensorMeasNotify
 *
 * @brief   Prepare and send a CSC measurement notification
 *
 * @return  none
 */
static void sensorMeasNotify( void )
{
  uint8 *p = sensorMeas.value;
  uint8 flags = sensorFlags[sensorFlagsIdx];

  // Build CSC measurement structure from simulated values
  // Flags simulate the isPresent bits.
  *p++ = flags;

  // If present, add Speed data into measurement
  if (flags & CSC_FLAGS_SPEED)
  {
    *p++ = BREAK_UINT32(cummWheelRevs, 0);
    *p++ = BREAK_UINT32(cummWheelRevs, 1);
    *p++ = BREAK_UINT32(cummWheelRevs, 2);
    *p++ = BREAK_UINT32(cummWheelRevs, 3);

    *p++ = LO_UINT16(lastWheelEvtTime);
    *p++ = HI_UINT16(lastWheelEvtTime);

    // Update simulated values (simulate in the reverse direction)
    if (cummWheelRevs < WHEEL_REV_INCREMENT) //don't allow revolutions to roll over
    {
      cummWheelRevs = 0;
    }
    else
    {
      cummWheelRevs -= WHEEL_REV_INCREMENT;
    }

    lastWheelEvtTime += WHEEL_EVT_INCREMENT;
  }

  // If present, add Cadence data into measurement
  if (flags & CSC_FLAGS_CADENCE)
  {
    *p++ = LO_UINT16(cummCrankRevs);
    *p++ = HI_UINT16(cummCrankRevs);

    *p++ = LO_UINT16(lastCrankEvtTime);
    *p++ = HI_UINT16(lastCrankEvtTime);

    // Update Simualted Values
    cummCrankRevs += CRANK_REV_INCREMENT;
    lastCrankEvtTime += CRANK_EVT_INCREMENT;
  }

  // Get length
  sensorMeas.len = (uint8) (p - sensorMeas.value);

  // Send to service to send the notification
  Cycling_MeasNotify( gapConnHandle, &sensorMeas );
}

/*********************************************************************
 * @fn      SensorGapStateCB
 *
 * @brief   Notification from the profile of a state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void SensorGapStateCB( gaprole_States_t newState )
{
  // If connected
  if (newState == GAPROLE_CONNECTED)
  {
    // Get connection handle
    GAPRole_GetParameter(GAPROLE_CONNHANDLE, &gapConnHandle);

    // Set timer to update connection parameters
    // 5 seconds should allow enough time for Service Discovery by the collector to finish
    osal_start_timerEx( sensor_TaskID, CSC_CONN_PARAM_UPDATE_EVT, SVC_DISC_DELAY);
  }
  // If disconnected
  else if (gapProfileState == GAPROLE_CONNECTED &&
           newState != GAPROLE_CONNECTED)
  {
    uint8 advState = TRUE;
    uint8 bondCount = 0;

    // Stop periodic measurement
    osal_stop_timerEx( sensor_TaskID, CSC_PERIODIC_EVT );

    // Reset client characteristic configuration descriptors
    Cycling_HandleConnStatusCB( gapConnHandle, LINKDB_STATUS_UPDATE_REMOVED );

    // If not already using white list, begin to do so.
    GAPBondMgr_GetParameter( GAPBOND_BOND_COUNT, &bondCount );

    if( USING_WHITE_LIST && sensorUsingWhiteList == FALSE && bondCount > 0 )
    {
      uint8 value = GAP_FILTER_POLICY_WHITE;

      GAPRole_SetParameter(GAPROLE_ADV_FILTER_POLICY, sizeof( uint8 ), &value);

      sensorUsingWhiteList = TRUE;
    }

    if ( newState == GAPROLE_WAITING_AFTER_TIMEOUT )
    {
      // link loss timeout-- use fast advertising
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, DEFAULT_FAST_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, DEFAULT_FAST_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_MIN, DEFAULT_WHITE_LIST_ADV_DURATION );
    }
    else
    {
      // Else use slow advertising
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, DEFAULT_SLOW_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, DEFAULT_SLOW_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_MIN, DEFAULT_WHITE_LIST_ADV_DURATION );
    }

    // Enable advertising
    GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &advState );
  }
  // if advertising stopped
  else if ( gapProfileState == GAPROLE_ADVERTISING &&
            newState == GAPROLE_WAITING )
  {
    uint8 whiteListUsed = FALSE;

    // if white list is in use, disable to allow general access
    if( sensorUsingWhiteList == TRUE )
    {
      uint8 value = GAP_FILTER_POLICY_ALL;

      GAPRole_SetParameter(GAPROLE_ADV_FILTER_POLICY, sizeof( uint8), &value);

      whiteListUsed = TRUE;

      sensorUsingWhiteList = FALSE;
    }

    // if advertising stopped by user
    if ( sensorAdvCancelled )
    {
      sensorAdvCancelled = FALSE;
    }
    // if fast advertising interrupted to cancel white list
    else if ( ( (!USING_WHITE_LIST) || whiteListUsed) &&
              (GAP_GetParamValue( TGAP_GEN_DISC_ADV_INT_MIN ) == DEFAULT_FAST_ADV_INTERVAL ) )
    {
      uint8 advState = TRUE;

      GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, DEFAULT_FAST_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, DEFAULT_FAST_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_MIN, DEFAULT_FAST_ADV_DURATION );
      GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &advState );
    }
    // if fast advertising switch to slow or if was already slow but using white list.
    else if ( ((!USING_WHITE_LIST) || whiteListUsed) ||
             (GAP_GetParamValue( TGAP_GEN_DISC_ADV_INT_MIN ) == DEFAULT_FAST_ADV_INTERVAL) )
    {
      uint8 advState = TRUE;

      GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MIN, DEFAULT_SLOW_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_INT_MAX, DEFAULT_SLOW_ADV_INTERVAL );
      GAP_SetParamValue( TGAP_GEN_DISC_ADV_MIN, DEFAULT_SLOW_ADV_DURATION );
      GAPRole_SetParameter( GAPROLE_ADVERT_ENABLED, sizeof( uint8 ), &advState );
    }
  }
  // if started
  else if (newState == GAPROLE_STARTED)
  {
    // Set the system ID from the bd addr
    uint8 systemId[DEVINFO_SYSTEM_ID_LEN];
    GAPRole_GetParameter(GAPROLE_BD_ADDR, systemId);

    // shift three bytes up
    systemId[7] = systemId[5];
    systemId[6] = systemId[4];
    systemId[5] = systemId[3];

    // set middle bytes to zero
    systemId[4] = 0;
    systemId[3] = 0;

    DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);
  }

  gapProfileState = newState;
}

/*********************************************************************
 * @fn      SensorCB
 *
 * @brief   Callback function for CSC service.
 *
 * @param   event - service event
 * @param   pNewCummVal - pointer to new wheel revolution data
 *                        if specified by event.  NULL otherwise.
 * @return  none
 */
static void SensorCB( uint8 event, uint32 *pNewCummVal )
{
  switch ( event )
  {
    case CSC_CMD_SET_CUMM_VAL:
      // Cancel neglect timer
      if ( USING_NEGLECT_TIMEOUT )
      {
        osal_stop_timerEx( sensor_TaskID, CSC_NEGLECT_TIMEOUT_EVT );
      }

      cummWheelRevs = *pNewCummVal;

      // Start neglect timer
      if ( USING_NEGLECT_TIMEOUT )
      {
        osal_start_timerEx( sensor_TaskID, CSC_NEGLECT_TIMEOUT_EVT, NEGLECT_TIMEOUT_DELAY );
      }
      break;

    case CSC_CMD_UPDATE_SENS_LOC:
      // Cancel neglect timer
      if ( USING_NEGLECT_TIMEOUT )
      {
        osal_stop_timerEx( sensor_TaskID, CSC_NEGLECT_TIMEOUT_EVT );
      }

      // Get updated sensor location
      Cycling_GetParameter( CSC_SENS_LOC, &sensorLocation );

      // Start neglect timer
      if ( USING_NEGLECT_TIMEOUT )
      {
        osal_start_timerEx( sensor_TaskID, CSC_NEGLECT_TIMEOUT_EVT, NEGLECT_TIMEOUT_DELAY );
      }
      break;

    case CSC_MEAS_NOTI_ENABLED:
      // Cancel neglect timer
      if ( USING_NEGLECT_TIMEOUT )
      {
        osal_stop_timerEx( sensor_TaskID, CSC_NEGLECT_TIMEOUT_EVT );
      }

      // If connected start periodic measurement
      if (gapProfileState == GAPROLE_CONNECTED)
      {
        osal_start_timerEx( sensor_TaskID, CSC_PERIODIC_EVT, DEFAULT_CSC_PERIOD );
      }
      break;

    case CSC_MEAS_NOTI_DISABLED:
      // Stop periodic measurement
      osal_stop_timerEx( sensor_TaskID, CSC_PERIODIC_EVT );

      // Start neglect timer
      if ( USING_NEGLECT_TIMEOUT )
      {
        osal_start_timerEx( sensor_TaskID, CSC_NEGLECT_TIMEOUT_EVT, NEGLECT_TIMEOUT_DELAY );
      }
      break;

    case CSC_READ_ATTR:
    case CSC_WRITE_ATTR:
      if ( USING_NEGLECT_TIMEOUT )
      {
        // Cancel neglect timer
        osal_stop_timerEx( sensor_TaskID, CSC_NEGLECT_TIMEOUT_EVT );

        // Start neglect timer
        osal_start_timerEx( sensor_TaskID, CSC_NEGLECT_TIMEOUT_EVT, NEGLECT_TIMEOUT_DELAY );
      }
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      sensorPeriodicTask
 *
 * @brief   Perform a periodic cycling application task.
 *
 * @param   none
 *
 * @return  none
 */
static void sensorPeriodicTask( void )
{
  if (gapProfileState == GAPROLE_CONNECTED)
  {
    // Send speed and cadence measurement notification
    sensorMeasNotify();

    // Restart timer
    osal_start_timerEx( sensor_TaskID, CSC_PERIODIC_EVT, DEFAULT_CSC_PERIOD );
  }
}

/*********************************************************************
*********************************************************************/
