/**************************************************************************************************
  Filename:       timeapp.h
  Revised:        $Date: 2012-01-20 14:33:21 -0800 (Fri, 20 Jan 2012) $
  Revision:       $Revision: 63 $

  Description:    This file contains the Time App sample application
                  definitions and prototypes.

  Copyright 2011 - 2013 Texas Instruments Incorporated. All rights reserved.

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

#ifndef TIMEAPP_H
#define TIMEAPP_H

#include "gatt.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */


// Time App Task Events
#define START_DEVICE_EVT                              0x0001
#define START_DISCOVERY_EVT                           0x0002
#define CLOCK_UPDATE_EVT                              0x0004

// Time App discovery states
enum
{
  DISC_IDLE = 0x00,                       // Idle state
  
  DISC_CURR_TIME_START = 0x10,            // Current time service
  DISC_CURR_TIME_SVC,                     // Discover service
  DISC_CURR_TIME_CHAR,                    // Discover all characteristics
  DISC_CURR_TIME_CT_TIME_CCCD,            // Discover CT time CCCD
  
  DISC_DST_CHG_START = 0x20,              // DST change service
  DISC_DST_CHG_SVC,                       // Discover service
  DISC_DST_CHG_CHAR,                      // Discover all characteristics

  DISC_REF_TIME_START = 0x30,             // Reference time service
  DISC_REF_TIME_SVC,                      // Discover service
  DISC_REF_TIME_CHAR,                     // Discover all characteristics

  DISC_NWA_START = 0x40,                  // NwA service
  DISC_NWA_SVC,                           // Discover service
  DISC_NWA_CHAR,                          // Discover all characteristics
  DISC_NWA_NWA_CCCD,                      // Discover NwA CCCD
    
  DISC_ALERT_NTF_START = 0x50,            // Alert notification service
  DISC_ALERT_NTF_SVC,                     // Discover service
  DISC_ALERT_NTF_CHAR,                    // Discover all characteristics
  DISC_ALERT_NTF_NEW_CCCD,                // Discover new alert CCCD
  DISC_ALERT_NTF_UNREAD_CCCD,             // Discover unread alert status CCCD

  DISC_BATT_START = 0x60,                 // Battery service
  DISC_BATT_SVC,                          // Discover service
  DISC_BATT_CHAR,                         // Discover all characteristics
  DISC_BATT_LVL_CCCD,                     // Discover battery level CCCD

  DISC_PAS_START = 0x70,                  // Phone alert status service
  DISC_PAS_SVC,                           // Discover service
  DISC_PAS_CHAR,                          // Discover all characteristics
  DISC_PAS_ALERT_CCCD,                    // Discover alert status CCCD
  DISC_PAS_RINGER_CCCD,                   // Discover ringer setting CCCD

  DISC_FAILED = 0xFF                      // Discovery failed
};

// Time App handle cache indexes
enum
{
  HDL_CURR_TIME_CT_TIME_START,            // Current time start handle
  HDL_CURR_TIME_CT_TIME_END,              // Current time end handle
  HDL_CURR_TIME_CT_TIME_CCCD,             // Current time CCCD
  HDL_CURR_TIME_LOC_INFO,                 // Local time info
  HDL_CURR_TIME_REF_INFO,                 // Reference time info
  
  HDL_DST_CHG_TIME_DST,                   // Time with DST
  
  HDL_REF_TIME_UPD_CTRL,                  // Time update control point
  HDL_REF_TIME_UPD_STATE,                 // Time update state
  
  HDL_NWA_NWA_START,                      // NwA start handle
  HDL_NWA_NWA_END,                        // NwA end handle
  HDL_NWA_NWA_CCCD,                       // NwA CCCD
  
  HDL_ALERT_NTF_NEW_START,                // New alert start handle
  HDL_ALERT_NTF_NEW_END,                  // New alert end handle
  HDL_ALERT_NTF_UNREAD_START,             // Unread alert status start handle
  HDL_ALERT_NTF_UNREAD_END,               // Unread alert status end handle
  HDL_ALERT_NTF_CTRL,                     // Alert notification control point
  HDL_ALERT_NTF_NEW_CAT,                  // Supported New Alert Category
  HDL_ALERT_NTF_UNREAD_CAT,               // Supported Unread Alert Category
  HDL_ALERT_NTF_NEW_CCCD,                 // New alert CCCD
  HDL_ALERT_NTF_UNREAD_CCCD,              // Alert unread alert status CCCD

  HDL_BATT_LEVEL_START,                   // Battery level start handle
  HDL_BATT_LEVEL_END,                     // Battery level end handle
  HDL_BATT_LEVEL_CCCD,                    // Battery level CCCD

  HDL_PAS_ALERT_START,                    // Alert status start handle
  HDL_PAS_ALERT_END,                      // Alert status end handle
  HDL_PAS_RINGER_START,                   // Ringer setting start handle
  HDL_PAS_RINGER_END,                     // Ringer setting end handle
  HDL_PAS_CTRL,                           // Ringer control point
  HDL_PAS_ALERT_CCCD,                     // Alert status CCCD
  HDL_PAS_RINGER_CCCD,                    // Ringer setting CCCD

  HDL_CACHE_LEN
};

// Configuration states
#define TIMEAPP_CONFIG_START              0x00
#define TIMEAPP_CONFIG_CMPL               0xFF

// Alert Notification Service values for alert notification control point
#define ALERT_NOTIF_ENABLE_NEW            0   // Enable New Incoming Alert Notification
#define ALERT_NOTIF_ENABLE_UNREAD         1   // Enable Unread Category Status Notification
#define ALERT_NOTIF_DISABLE_NEW           2   // Disable New Incoming Alert Notification
#define ALERT_NOTIF_DISABLE_UNREAD        3   // Disable Unread Category Status Notification
#define ALERT_NOTIF_NOTIFY_NEW            4   // Notify New Incoming Alert immediately
#define ALERT_NOTIF_NOTIFY_UNREAD         5   // Notify Unread Category Status immediately

// Alert Notification Service category IDs
#define ALERT_NOTIF_CAT_ALL               0xFF

// Index into timeAppConfigList to start of connection setup characteristics
#define TIMEAPP_CONFIG_CONN_START         15

/*********************************************************************
 * MACROS
 */

// LCD macros
#if HAL_LCD == TRUE
#define LCD_WRITE_STRING(str, option)                       HalLcdWriteString( (str), (option))
#define LCD_WRITE_SCREEN(line1, line2)                      HalLcdWriteScreen( (line1), (line2) )
#define LCD_WRITE_STRING_VALUE(title, value, format, line)  HalLcdWriteStringValue( (title), (value), (format), (line) )
#else
#define LCD_WRITE_STRING(str, option)                     
#define LCD_WRITE_SCREEN(line1, line2)                    
#define LCD_WRITE_STRING_VALUE(title, value, format, line)
#endif

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Task ID
extern uint8 timeAppTaskId;

// Connection handle
extern uint16 timeAppConnHandle;

// Handle cache
extern uint16 timeAppHdlCache[HDL_CACHE_LEN];

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the BLE Application
 */
extern void TimeApp_Init( uint8 task_id );

/*
 * Task Event Processor for the BLE Application
 */
extern uint16 TimeApp_ProcessEvent( uint8 task_id, uint16 events );

/*
 * Time App clock functions
 */
extern void timeAppClockInit( void );
extern void timeAppClockDisplay( void );
extern void timeAppClockSet( uint8 *pData );

/* 
 * Time App service discovery functions
 */
extern uint8 timeAppDiscStart( void );
extern uint8 timeAppDiscGattMsg( uint8 state, gattMsgEvent_t *pMsg );

/* 
 * Time App characteristic configuration functions
 */
extern uint8 timeAppConfigNext( uint8 state );
extern uint8 timeAppConfigGattMsg( uint8 state, gattMsgEvent_t *pMsg );

/* 
 * Time App indication and notification handling functions
 */
extern void timeAppIndGattMsg( gattMsgEvent_t *pMsg );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* TIMEAPP_H */
