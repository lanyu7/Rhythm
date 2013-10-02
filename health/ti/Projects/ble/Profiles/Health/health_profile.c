/**************************************************************************************************
  Filename:       simpleGATTprofile.c
  Revised:        $Date: 2013-05-06 13:33:47 -0700 (Mon, 06 May 2013) $
  Revision:       $Revision: 34153 $

  Description:    This file contains the Simple GATT profile sample GATT service 
                  profile for use with the BLE sample application.

  Copyright 2010 - 2013 Texas Instruments Incorporated. All rights reserved.

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
  PROVIDED �AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
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
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#include "hal_lcd.h"

#include "health_profile.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SERVAPP_NUM_ATTR_SUPPORTED        17

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Simple GATT Profile Service UUID: 0xFFF0
CONST uint8 simpleProfileServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(HEALTH_SERV_UUID), HI_UINT16(HEALTH_SERV_UUID)
};

// Characteristic 1 UUID: 0xFFF1
CONST uint8 healthSyncUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(HEALTH_SYNC_UUID), HI_UINT16(HEALTH_SYNC_UUID)
};

// Characteristic 2 UUID: 0xFFF2
CONST uint8 healthClockUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(HEALTH_CLOCK_UUID), HI_UINT16(HEALTH_CLOCK_UUID)
};

// Characteristic 3 UUID: 0xFFF3
CONST uint8 healthDataHeaderUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(HEALTH_DATA_HEADER_UUID), HI_UINT16(HEALTH_DATA_HEADER_UUID)
};

// Characteristic 4 UUID: 0xFFF4
CONST uint8 healthDataBodyUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(HEALTH_DATA_BODY_UUID), HI_UINT16(HEALTH_DATA_BODY_UUID)
};

// Characteristic 5 UUID: 0xFFF5
CONST uint8 simpleProfilechar5UUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_CHAR5_UUID), HI_UINT16(SIMPLEPROFILE_CHAR5_UUID)
};

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static simpleProfileCBs_t *simpleProfile_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Simple Profile Service attribute
static CONST gattAttrType_t simpleProfileService = { ATT_BT_UUID_SIZE, simpleProfileServUUID };


// Simple Profile Characteristic 1 Properties
static uint8 healthSyncProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 1 Value
static uint8 healthSync = 0;

// Simple Profile Characteristic 1 User Description
static uint8 healthSyncUserDesp[17] = "Characteristic 1\0";


// Simple Profile Characteristic 2 Properties
static uint8 healthClockProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Characteristic 2 Value
static uint8 healthClock[4] = {0,0,0,0};

// Simple Profile Characteristic 2 User Description
static uint8 healthClockUserDesp[17] = "Characteristic 2\0";


// Simple Profile Characteristic 3 Properties
static uint8 healthDataHeaderProps = GATT_PROP_WRITE;

// Characteristic 3 Value
static uint8 healthDataHeader = 0;

// Simple Profile Characteristic 3 User Description
static uint8 healthDataHeaderUserDesp[17] = "Characteristic 3\0";


// Simple Profile Characteristic 4 Properties
static uint8 healthDataBodyProps = GATT_PROP_NOTIFY;

// Characteristic 4 Value
static uint8 healthDataBody = 0;

// Simple Profile Characteristic 4 Configuration Each client has its own
// instantiation of the Client Characteristic Configuration. Reads of the
// Client Characteristic Configuration only shows the configuration for
// that client and writes only affect the configuration of that client.
static gattCharCfg_t healthDataBodyConfig[GATT_MAX_NUM_CONN];
                                        
// Simple Profile Characteristic 4 User Description
static uint8 healthDataBodyUserDesp[17] = "Characteristic 4\0";


// Simple Profile Characteristic 5 Properties
static uint8 simpleProfileChar5Props = GATT_PROP_READ;

// Characteristic 5 Value
static uint8 simpleProfileChar5[SIMPLEPROFILE_CHAR5_LEN] = { 0, 0, 0, 0, 0 };

// Simple Profile Characteristic 5 User Description
static uint8 simpleProfileChar5UserDesp[17] = "Characteristic 5\0";


/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t simpleProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = 
{
  // Simple Profile Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&simpleProfileService            /* pValue */
  },

    // Characteristic 1 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &healthSyncProps 
    },

      // Characteristic Value 1
      { 
        { ATT_BT_UUID_SIZE, healthSyncUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        &healthSync 
      },

      // Characteristic 1 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        healthSyncUserDesp 
      },      

    // Characteristic 2 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &healthClockProps 
    },

      // Characteristic Value 2
      { 
        { ATT_BT_UUID_SIZE, healthClockUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        healthClock 
      },

      // Characteristic 2 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        healthClockUserDesp 
      },           
      
    // Characteristic 3 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &healthDataHeaderProps 
    },

      // Characteristic Value 3
      { 
        { ATT_BT_UUID_SIZE, healthDataHeaderUUID },
        GATT_PERMIT_WRITE, 
        0, 
        &healthDataHeader 
      },

      // Characteristic 3 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        healthDataHeaderUserDesp 
      },

    // Characteristic 4 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &healthDataBodyProps 
    },

      // Characteristic Value 4
      { 
        { ATT_BT_UUID_SIZE, healthDataBodyUUID },
        0, 
        0, 
        &healthDataBody 
      },

      // Characteristic 4 configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)healthDataBodyConfig 
      },
      
      // Characteristic 4 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        healthDataBodyUserDesp 
      },
      
    // Characteristic 5 Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &simpleProfileChar5Props 
    },

      // Characteristic Value 5
      { 
        { ATT_BT_UUID_SIZE, simpleProfilechar5UUID },
        GATT_PERMIT_AUTHEN_READ, 
        0, 
        simpleProfileChar5 
      },

      // Characteristic 5 User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        simpleProfileChar5UserDesp 
      },


};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static uint8 simpleProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen );
static bStatus_t simpleProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset );

static void simpleProfile_HandleConnStatusCB( uint16 connHandle, uint8 changeType );


/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t simpleProfileCBs =
{
  simpleProfile_ReadAttrCB,  // Read callback function pointer
  simpleProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleProfile_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t SimpleProfile_AddService( uint32 services )
{
  uint8 status = SUCCESS;

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, healthDataBodyConfig );

  // Register with Link DB to receive link status change callback
  VOID linkDB_Register( simpleProfile_HandleConnStatusCB );  
  
  if ( services & SIMPLEPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( simpleProfileAttrTbl, 
                                          GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                          &simpleProfileCBs );
  }

  return ( status );
}


/*********************************************************************
 * @fn      SimpleProfile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t SimpleProfile_RegisterAppCBs( simpleProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    simpleProfile_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}
  

/*********************************************************************
 * @fn      SimpleProfile_SetParameter
 *
 * @brief   Set a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t SimpleProfile_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case HEALTH_SYNC:
      if ( len == sizeof ( uint8 ) ) 
      {
        healthSync = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case HEALTH_CLOCK:
      if ( len == sizeof ( uint32 ) ) 
      {
        //healthClock = *((uint32*)value);
        VOID osal_memcpy( healthClock, value, 4 );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case HEALTH_DATA_HEADER:
      if ( len == sizeof ( uint8 ) ) 
      {
        healthDataHeader = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case HEALTH_DATA_BODY:
      if ( len == sizeof ( uint8 ) ) 
      {
        healthDataBody = *((uint8*)value);
        
        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg( healthDataBodyConfig, &healthDataBody, FALSE,
                                    simpleProfileAttrTbl, GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                    INVALID_TASK_ID );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SIMPLEPROFILE_CHAR5:
      if ( len == SIMPLEPROFILE_CHAR5_LEN ) 
      {
        VOID osal_memcpy( simpleProfileChar5, value, SIMPLEPROFILE_CHAR5_LEN );
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn      SimpleProfile_GetParameter
 *
 * @brief   Get a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t SimpleProfile_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case HEALTH_SYNC:
      *((uint8*)value) = healthSync;
      break;

    case HEALTH_CLOCK:
      //*((uint8*)value) = healthClock;
      VOID osal_memcpy( value, healthClock, 4 );
      break;      

    case HEALTH_DATA_HEADER:
      *((uint8*)value) = healthDataHeader;
      break;  

    case HEALTH_DATA_BODY:
      *((uint8*)value) = healthDataBody;
      break;

    case SIMPLEPROFILE_CHAR5:
      VOID osal_memcpy( value, simpleProfileChar5, SIMPLEPROFILE_CHAR5_LEN );
      break;      
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn          simpleProfile_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 *
 * @return      Success or Failure
 */
static uint8 simpleProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                            uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen )
{
  bStatus_t status = SUCCESS;

  // If attribute permissions require authorization to read, return error
  if ( gattPermitAuthorRead( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
  
  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }
 
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those reads

      // characteristics 1 and 2 have read permissions
      // characteritisc 3 does not have read permissions; therefore it is not
      //   included here
      // characteristic 4 does not have read permissions, but because it
      //   can be sent as a notification, it is included here
      case HEALTH_SYNC_UUID:
      case HEALTH_CLOCK_UUID:
      case HEALTH_DATA_BODY:
        *pLen = 1;
        pValue[0] = *pAttr->pValue;
        
        #if (defined HAL_LCD) && (HAL_LCD == TRUE)
          HalLcdWriteStringValue( "read:", pValue[0], 10,  HAL_LCD_LINE_3 );
        #endif // (defined HAL_LCD) && (HAL_LCD == TRUE)
        
        break;

      case SIMPLEPROFILE_CHAR5_UUID:
        *pLen = SIMPLEPROFILE_CHAR5_LEN;
        VOID osal_memcpy( pValue, pAttr->pValue, SIMPLEPROFILE_CHAR5_LEN );
        break;
        
      default:
        // Should never get here! (characteristics 3 and 4 do not have read permissions)
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }

  return ( status );
}

/*********************************************************************
 * @fn      simpleProfile_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 *
 * @return  Success or Failure
 */
static bStatus_t simpleProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                 uint8 *pValue, uint8 len, uint16 offset )
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
  
  // If attribute permissions require authorization to write, return error
  if ( gattPermitAuthorWrite( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
  
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      case HEALTH_SYNC_UUID:
      case HEALTH_CLOCK_UUID:
      case HEALTH_DATA_HEADER_UUID:

        //Validate the value
        // Make sure it's not a blob oper
        if ( offset == 0 )
        {
          if ( len != 1 )
          {
            //status = ATT_ERR_INVALID_VALUE_SIZE;
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        //Write the value
        if ( status == SUCCESS )
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;        
          *pCurValue = pValue[0];

          if( pAttr->pValue == &healthSync )
          {
            notifyApp = HEALTH_SYNC;        
          }
          else if( pAttr->pValue == &healthDataHeader )
          {
            notifyApp = HEALTH_DATA_HEADER;           
          }
          else{
            notifyApp = HEALTH_CLOCK;
          }
        }
             
        break;

      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY );
        break;
        
      default:
        // Should never get here! (characteristics 2 and 4 do not have write permissions)
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }

  // If a charactersitic value changed then callback function to notify application of change
  if ( (notifyApp != 0xFF ) && simpleProfile_AppCBs && simpleProfile_AppCBs->pfnSimpleProfileChange )
  {
    simpleProfile_AppCBs->pfnSimpleProfileChange( notifyApp );  
  }
  
  return ( status );
}

/*********************************************************************
 * @fn          simpleProfile_HandleConnStatusCB
 *
 * @brief       Simple Profile link status change handler function.
 *
 * @param       connHandle - connection handle
 * @param       changeType - type of change
 *
 * @return      none
 */
static void simpleProfile_HandleConnStatusCB( uint16 connHandle, uint8 changeType )
{ 
  // Make sure this is not loopback connection
  if ( connHandle != LOOPBACK_CONNHANDLE )
  {
    // Reset Client Char Config if connection has dropped
    if ( ( changeType == LINKDB_STATUS_UPDATE_REMOVED )      ||
         ( ( changeType == LINKDB_STATUS_UPDATE_STATEFLAGS ) && 
           ( !linkDB_Up( connHandle ) ) ) )
    { 
      GATTServApp_InitCharCfg( connHandle, healthDataBodyConfig );
    }
  }
}


/*********************************************************************
*********************************************************************/
