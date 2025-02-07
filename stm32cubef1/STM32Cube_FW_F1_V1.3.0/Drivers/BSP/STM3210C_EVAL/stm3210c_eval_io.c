/**
  ******************************************************************************
  * @file    stm3210c_eval_io.c
  * @author  MCD Application Team
  * @version V6.0.1
  * @date    18-December-2015
  * @brief   This file provides a set of functions needed to manage the IO pins
  *          on STM3210C-EVAL evaluation board.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 
  
/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. How To use this driver:
--------------------------
   - This driver is used to drive the IO module of the STM3210C-EVAL evaluation 
     board.
   - The STMPE811 IO expander device component driver must be included with this 
     driver in order to run the IO functionalities commanded by the IO expander 
     device mounted on the evaluation board.

2. Driver description:
---------------------
  + Initialization steps:
     o Initialize the IO module using the BSP_IO_Init() function. This 
       function includes the MSP layer hardware resources initialization and the
       communication layer configuration to start the IO functionalities use.    
  
  + IO functionalities use
     o The IO pin mode is configured when calling the function BSP_IO_ConfigPin(), you 
       must specify the desired IO mode by choosing the "IO_ModeTypedef" parameter 
       predefined value.
     o If an IO pin is used in interrupt mode, the function BSP_IO_ITGetStatus() is 
       needed to get the interrupt status. To clear the IT pending bits, you should 
       call the function BSP_IO_ITClear() with specifying the IO pending bit to clear.
     o The IT is handled using the corresponding external interrupt IRQ handler,
       the user IT callback treatment is implemented on the same external interrupt
       callback.
     o To get/set an IO pin combination state you can use the functions 
       BSP_IO_ReadPin()/BSP_IO_WritePin() or the function BSP_IO_TogglePin() to toggle the pin 
       state.
 
------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "stm3210c_eval_io.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM3210C_EVAL
  * @{
  */ 
  
/** @defgroup STM3210C_EVAL_IO STM3210C_EVAL IO Expander
  * @{
  */   
  
/* Private typedef -----------------------------------------------------------*/

/** @defgroup STM3210C_EVAL_IO_Private_Types_Definitions Private Types Definitions
  * @{
  */ 
  
/**
  * @}
  */      

/* Private define ------------------------------------------------------------*/

/** @defgroup STM3210C_EVAL_IO_Private_Defines Private_Defines
  * @{
  */ 
  
/**
  * @}
  */ 

/* Private macro -------------------------------------------------------------*/

/** @defgroup STM3210C_EVAL_IO_Private_Macros Private_Macros
  * @{
  */ 
  
/**
  * @}
  */ 

/* Private variables ---------------------------------------------------------*/

/** @defgroup STM3210C_EVAL_IO_Private_Variables Private_Variables
  * @{
  */ 
static IO_DrvTypeDef *io1_driver;
static IO_DrvTypeDef *io2_driver;
  

/**
  * @}
  */ 

/* Private function prototypes -----------------------------------------------*/

/** @defgroup STM3210C_EVAL_IO_Private_Function_Prototypes Private_Function_Prototypes
  * @{
  */ 

/**
  * @}
  */ 
    
/* Private functions ---------------------------------------------------------*/

/** @defgroup STM3210C_EVAL_IO_Exported_Functions Exported_Functions
  * @{
  */ 

/**
  * @brief  Initializes and configures the IO functionalities and configures all
  *         necessary hardware resources (GPIOs, clocks..).
  * @note   BSP_IO_Init() is using HAL_Delay() function to ensure that stmpe811
  *         IO Expander is correctly reset. HAL_Delay() function provides accurate
  *         delay (in milliseconds) based on variable incremented in SysTick ISR. 
  *         This implies that if BSP_IO_Init() is called from a peripheral ISR process,
  *         then the SysTick interrupt must have higher priority (numerically lower)
  *         than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
  * @retval IO_OK: if all initializations are OK. Other value if error.
  */
uint8_t BSP_IO_Init(void)
{
  uint8_t ret = IO_ERROR;
  
  /* Initialize IO Expander 1*/
  if(stmpe811_io_drv.ReadID(IO1_I2C_ADDRESS) == STMPE811_ID)
  {  
    /* Initialize the IO Expander 1 driver structure */
    io1_driver = &stmpe811_io_drv;

    io1_driver->Init(IO1_I2C_ADDRESS);
    io1_driver->Start(IO1_I2C_ADDRESS, IO1_PIN_ALL >> IO1_PIN_OFFSET);

    ret = IO_OK;
  }
  
  /* Initialize IO Expander 2*/
  if(stmpe811_io_drv.ReadID(IO2_I2C_ADDRESS) == STMPE811_ID)
  {  
    /* Initialize the IO Expander 2 driver structure */
    io2_driver = &stmpe811_io_drv;

    io2_driver->Init(IO2_I2C_ADDRESS);
    io2_driver->Start(IO2_I2C_ADDRESS, IO2_PIN_ALL >> IO2_PIN_OFFSET);

    ret = IO_OK;
  }
  
  return ret;
}

/**
  * @brief  Gets the selected pins IT status.
  * @param  IO_Pin: Selected pins to check the status. 
  *          This parameter can be any combination of the IO pins. 
  * @retval Status of the checked IO pin(s).
  */
uint32_t BSP_IO_ITGetStatus(uint32_t IO_Pin)
{
  uint32_t status = 0;
  uint32_t io1_pin = 0;
  uint32_t io2_pin = 0;
  
  io1_pin = (IO_Pin & IO1_PIN_ALL) >> IO1_PIN_OFFSET;
  io2_pin = (IO_Pin & IO2_PIN_ALL) >> IO2_PIN_OFFSET;
  
  if (io1_pin != 0)
  {
    /* Return the IO Expander 1 Pin IT status */
    status |= (io1_driver->ITStatus(IO1_I2C_ADDRESS, io1_pin)) << IO1_PIN_OFFSET;
  }
  
  if (io2_pin != 0)
  {
    /* Return the IO Expander 2 Pin IT status */
    status |= (io2_driver->ITStatus(IO2_I2C_ADDRESS, io2_pin)) << IO2_PIN_OFFSET;
  }

  return status;
}

/**
  * @brief  Clears the selected IO IT pending bit.
  * @param  IO_Pin: Selected pins to check the status. 
  *          This parameter can be any combination of the IO pins. 
  * @retval None
  */
void BSP_IO_ITClear(uint32_t IO_Pin)
{
  uint32_t io1_pin = 0;
  uint32_t io2_pin = 0;
  
  io1_pin = (IO_Pin & IO1_PIN_ALL) >> IO1_PIN_OFFSET;
  io2_pin = (IO_Pin & IO2_PIN_ALL) >> IO2_PIN_OFFSET;

  if (io1_pin != 0)
  {
    /* Clears the selected IO Expander 1 pin(s) mode */
    io1_driver->ClearIT(IO1_I2C_ADDRESS, io1_pin);
  }
  
  if (io2_pin != 0)
  {
    /* Clears the selected IO Expander 2 pin(s) mode */
    io2_driver->ClearIT(IO2_I2C_ADDRESS, io2_pin);
  }
}

/**
  * @brief  Configures the IO pin(s) according to IO mode structure value.
  * @param  IO_Pin: Output pin to be set or reset. 
  *          This parameter can be any combination of the IO pins. 
  * @param  IO_Mode: IO pin mode to configure
  *          This parameter can be one of the following values:
  *            @arg  IO_MODE_INPUT
  *            @arg  IO_MODE_OUTPUT
  *            @arg  IO_MODE_IT_RISING_EDGE
  *            @arg  IO_MODE_IT_FALLING_EDGE
  *            @arg  IO_MODE_IT_LOW_LEVEL
  *            @arg  IO_MODE_IT_HIGH_LEVEL 
  * @retval IO_OK: if all initializations are OK. Other value if error.  
  */
uint8_t BSP_IO_ConfigPin(uint32_t IO_Pin, IO_ModeTypedef IO_Mode)
{
  uint32_t io1_pin = 0;
  uint32_t io2_pin = 0;
  
  io1_pin = (IO_Pin & IO1_PIN_ALL) >> IO1_PIN_OFFSET;
  io2_pin = (IO_Pin & IO2_PIN_ALL) >> IO2_PIN_OFFSET;

  if (io1_pin != 0)
  {
    /* Configure the selected IO Expander 1 pin(s) mode */
    io1_driver->Config(IO1_I2C_ADDRESS, io1_pin, IO_Mode);    
  }  

  if (io2_pin != 0)
  {
    /* Configure the selected IO Expander 2 pin(s) mode */
    io2_driver->Config(IO2_I2C_ADDRESS, io2_pin, IO_Mode);    
  }

  return IO_OK;  
}

/**
  * @brief  Sets the selected pins state.
  * @param  IO_Pin: Selected pins to write. 
  *          This parameter can be any combination of the IO pins. 
  * @param  PinState: New pins state to write  
  * @retval None
  */
void BSP_IO_WritePin(uint32_t IO_Pin, uint8_t PinState)
{
  uint32_t io1_pin = 0;
  uint32_t io2_pin = 0;
  
  io1_pin = (IO_Pin & IO1_PIN_ALL) >> IO1_PIN_OFFSET;
  io2_pin = (IO_Pin & IO2_PIN_ALL) >> IO2_PIN_OFFSET;

  if (io1_pin != 0)
  {
    /* Sets the IO Expander 1 selected pins state */
    io1_driver->WritePin(IO1_I2C_ADDRESS, io1_pin, PinState);
  }

  if (io2_pin != 0)
  {
    /* Sets the IO Expander 2 selected pins state */
    io2_driver->WritePin(IO2_I2C_ADDRESS, io2_pin, PinState);
  }
}

/**
  * @brief  Gets the selected pins current state.
  * @param  IO_Pin: Selected pins to read. 
  *          This parameter can be any combination of the IO pins. 
  * @retval The current pins state 
  */
uint32_t BSP_IO_ReadPin(uint32_t IO_Pin)
{
  uint32_t pin_state = 0;
  uint32_t io1_pin = 0;
  uint32_t io2_pin = 0;
  
  io1_pin = (IO_Pin & IO1_PIN_ALL) >> IO1_PIN_OFFSET;
  io2_pin = (IO_Pin & IO2_PIN_ALL) >> IO2_PIN_OFFSET;

  if (io1_pin != 0)
  {
    /* Gets the IO Expander 1 selected pins current state */
    pin_state |= (io1_driver->ReadPin(IO1_I2C_ADDRESS, io1_pin)) << IO1_PIN_OFFSET;
  }

  if (io2_pin != 0)
  {
    /* Gets the IO Expander 2 selected pins current state */
    pin_state |= (io2_driver->ReadPin(IO2_I2C_ADDRESS, io2_pin)) << IO2_PIN_OFFSET;
  }
  
  return pin_state;
}

/**
  * @brief  Toggles the selected pins state
  * @param  IO_Pin: Selected pins to toggle. 
  *          This parameter can be any combination of the IO pins.   
  * @retval None
  */
void BSP_IO_TogglePin(uint32_t IO_Pin)
{
  uint32_t io1_pin = 0;
  uint32_t io2_pin = 0;
  
  io1_pin = (IO_Pin & IO1_PIN_ALL) >> IO1_PIN_OFFSET;
  io2_pin = (IO_Pin & IO2_PIN_ALL) >> IO2_PIN_OFFSET;

  if (io1_pin != 0)
  {
    /* Toggles the IO Expander 1 selected pins state */
    if(io1_driver->ReadPin(IO1_I2C_ADDRESS, io1_pin) == RESET) /* Set */
    {
      BSP_IO_WritePin(io1_pin, GPIO_PIN_SET); /* Reset */
    }
    else
    {
      BSP_IO_WritePin(io1_pin, GPIO_PIN_RESET);  /* Set */    
    } 
  }

  if (io2_pin != 0)
  {
    /* Toggles the IO Expander 2 selected pins state */
    if(io2_driver->ReadPin(IO2_I2C_ADDRESS, io2_pin) == RESET) /* Set */
    {
      BSP_IO_WritePin(io2_pin, GPIO_PIN_SET); /* Reset */
    }
    else
    {
      BSP_IO_WritePin(io2_pin, GPIO_PIN_RESET);  /* Set */    
    } 
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
