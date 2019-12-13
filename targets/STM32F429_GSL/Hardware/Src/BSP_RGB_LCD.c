/**
  ******************************************************************************
  * @file    stm32746g_discovery_lcd.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    25-June-2015
  * @brief   This file includes the driver for Liquid Crystal Display (LCD) module
  *          mounted on STM32746G-Discovery board.
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

/* Includes ------------------------------------------------------------------*/
#include "dma2d.h"
#include "BSP_RGB_LCD.h"

#include "dma2d.h"
#include "ltdc.h"
/** @addtogroup BSP
  * @{
  */
#define XSIZE_PHYS 800
#define YSIZE_PHYS 480

#define HBP  2
#define VBP  2

#define HSW  10
#define VSW  2

#define HFP  2
#define VFP  2

/** @addtogroup STM32746G_DISCOVERY
  * @{
  */
    
/** @addtogroup STM32746G_DISCOVERY_LCD
  * @{
  */ 

/** @defgroup STM32746G_DISCOVERY_LCD_Private_TypesDefinitions STM32746G_DISCOVERY_LCD Private Types Definitions
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup STM32746G_DISCOVERY_LCD_Private_Defines STM32746G_DISCOVERY LCD Private Defines
  * @{
  */
#define POLY_X(Z)              ((int32_t)((Points + Z)->X))
#define POLY_Y(Z)              ((int32_t)((Points + Z)->Y))      
/**
  * @}
  */ 

/** @defgroup STM32746G_DISCOVERY_LCD_Private_Macros STM32746G_DISCOVERY_LCD Private Macros
  * @{
  */
#define ABS(X)  ((X) > 0 ? (X) : -(X))      
/**
  * @}
  */ 
    
/** @defgroup STM32746G_DISCOVERY_LCD_Private_Variables STM32746G_DISCOVERY_LCD Private Variables
  * @{
  */ 
//static LTDC_HandleTypeDef  hLtdcHandler;
//static DMA2D_HandleTypeDef hDma2dHandler;
#define hLtdcHandler hltdc  
#define hDma2dHandler hdma2d

/* Default LCD configuration with LCD Layer 1 */
static uint32_t            ActiveLayer = 0;
/**
  * @}
  */ 

/** @defgroup STM32746G_DISCOVERY_LCD_Private_FunctionPrototypes STM32746G_DISCOVERY_LCD Private Function Prototypes
  * @{
  */ 
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex);
/**
  * @}
  */ 

/** @defgroup STM32746G_DISCOVERY_LCD_Exported_Functions STM32746G_DISCOVERY_LCD Exported Functions
  * @{
  */

/**
  * @brief  Initializes the LCD.
  * @retval LCD state
  */
uint8_t BSP_LCD_Init(void)
{  
  /* Initialize the SDRAM */
  BSP_SDRAM_Init();
	BSP_LCD_SelectLayer(1);
  /* Clear the LCD Foreground layer */
  BSP_LCD_SetLayerVisible(1, DISABLE);
  /* Set Foreground Layer */
  BSP_LCD_SelectLayer(0);

  return LCD_OK;
}

/**
  * @brief  Gets the LCD X size.
  * @retval Used LCD X size
  */
uint32_t BSP_LCD_GetXSize(void)
{
  return hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth;
}

/**
  * @brief  Gets the LCD Y size.
  * @retval Used LCD Y size
  */
uint32_t BSP_LCD_GetYSize(void)
{
  return hLtdcHandler.LayerCfg[ActiveLayer].ImageHeight;
}


/**
  * @brief  Selects the LCD Layer.
  * @param  LayerIndex: Layer foreground or background
  * @retval None
  */
void BSP_LCD_SelectLayer(uint32_t LayerIndex)
{
  ActiveLayer = LayerIndex;
} 

/**
  * @brief  Sets an LCD Layer visible
  * @param  LayerIndex: Visible Layer
  * @param  State: New state of the specified layer
  *          This parameter can be one of the following values:
  *            @arg  ENABLE
  *            @arg  DISABLE 
  * @retval None
  */
void BSP_LCD_SetLayerVisible(uint32_t LayerIndex, FunctionalState State)
{
  if(State == ENABLE)
  {
    __HAL_LTDC_LAYER_ENABLE(&hLtdcHandler, LayerIndex);
  }
  else
  {
    __HAL_LTDC_LAYER_DISABLE(&hLtdcHandler, LayerIndex);
  }
  __HAL_LTDC_RELOAD_CONFIG(&hLtdcHandler);
} 

/**
  * @brief  Configures the transparency.
  * @param  LayerIndex: Layer foreground or background.
  * @param  Transparency: Transparency
  *           This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF 
  * @retval None
  */
void BSP_LCD_SetTransparency(uint32_t LayerIndex, uint8_t Transparency)
{    
  HAL_LTDC_SetAlpha(&hLtdcHandler, Transparency, LayerIndex);
}

/**
  * @brief  Sets an LCD layer frame buffer address.
  * @param  LayerIndex: Layer foreground or background
  * @param  Address: New LCD frame buffer value      
  * @retval None
  */
void BSP_LCD_SetLayerAddress(uint32_t LayerIndex, uint32_t Address)
{
  HAL_LTDC_SetAddress(&hLtdcHandler, Address, LayerIndex);
}

/**
  * @brief  Sets display window.
  * @param  LayerIndex: Layer index
  * @param  Xpos: LCD X position
  * @param  Ypos: LCD Y position
  * @param  Width: LCD window width
  * @param  Height: LCD window height  
  * @retval None
  */
void BSP_LCD_SetLayerWindow(uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  /* Reconfigure the layer size */
  HAL_LTDC_SetWindowSize(&hLtdcHandler, Width, Height, LayerIndex);
  
  /* Reconfigure the layer position */
  HAL_LTDC_SetWindowPosition(&hLtdcHandler, Xpos, Ypos, LayerIndex); 
}

/**
  * @brief  Configures and sets the color keying.
  * @param  LayerIndex: Layer foreground or background
  * @param  RGBValue: Color reference
  * @retval None
  */
void BSP_LCD_SetColorKeying(uint32_t LayerIndex, uint32_t RGBValue)
{  
  /* Configure and Enable the color Keying for LCD Layer */
  HAL_LTDC_ConfigColorKeying(&hLtdcHandler, RGBValue, LayerIndex);
  HAL_LTDC_EnableColorKeying(&hLtdcHandler, LayerIndex);
}

/**
  * @brief  Disables the color keying.
  * @param  LayerIndex: Layer foreground or background
  * @retval None
  */
void BSP_LCD_ResetColorKeying(uint32_t LayerIndex)
{   
  /* Disable the color Keying for LCD Layer */
  HAL_LTDC_DisableColorKeying(&hLtdcHandler, LayerIndex);
}

/**
  * @brief  Reads an LCD pixel.
  * @param  Xpos: X position 
  * @param  Ypos: Y position 
  * @retval RGB pixel color
  */
uint32_t BSP_LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
  uint32_t ret = 0;
  
  if(hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    /* Read data value from SDRAM memory */
    ret = *(__IO uint32_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*BSP_LCD_GetXSize() + Xpos)));
  }
  else if(hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    /* Read data value from SDRAM memory */
    ret = (*(__IO uint32_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*BSP_LCD_GetXSize() + Xpos))) & 0x00FFFFFF);
  }
  else if((hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) || \
          (hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_AL88))  
  {
    /* Read data value from SDRAM memory */
    ret = *(__IO uint16_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*BSP_LCD_GetXSize() + Xpos)));    
  }
  else
  {
    /* Read data value from SDRAM memory */
    ret = *(__IO uint8_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*BSP_LCD_GetXSize() + Xpos)));    
  }
  
  return ret;
}

/**
  * @brief  Clears the hole LCD.
  * @param  Color: Color of the background
  * @retval None
  */
void BSP_LCD_Clear(uint32_t Color)
{ 
  /* Clear the LCD */ 
  LL_FillBuffer(ActiveLayer, (uint32_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress), BSP_LCD_GetXSize(), BSP_LCD_GetYSize(), 0, Color);
}


/**
  * @brief  Draws a pixel on LCD.
  * @param  Xpos: X position
  * @param  Ypos: Y position
  * @param  RGB_Code: Pixel color in ARGB mode (8-8-8-8)
  * @retval None
  */
void BSP_LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code)
{
  /* Write data value to all SDRAM memory */
  if(hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { /* RGB565 format */
    *(__IO uint16_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*BSP_LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;
  }
  else
  { /* ARGB8888 format */
    *(__IO uint32_t*) (hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (4*(Ypos*BSP_LCD_GetXSize() + Xpos))) = RGB_Code;
  }
}

/**
  * @brief  Fills a buffer.
  * @param  LayerIndex: Layer index
  * @param  pDst: Pointer to destination buffer
  * @param  xSize: Buffer width
  * @param  ySize: Buffer height
  * @param  OffLine: Offset
  * @param  ColorIndex: Color index
  * @retval None
  */
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex) 
{
  /* Register to memory mode with ARGB8888 as color Mode */ 
  hDma2dHandler.Init.Mode         = DMA2D_R2M;
  if(hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { /* RGB565 format */ 
    hDma2dHandler.Init.ColorMode    = DMA2D_RGB565;
		ColorIndex = ((ColorIndex & LCD_COLOR_RED)<<8) | ((ColorIndex & LCD_COLOR_GREEN )<<5) | ((ColorIndex & LCD_COLOR_BLUE) << 3);
  }
  else
  { /* ARGB8888 format */
    hDma2dHandler.Init.ColorMode    = DMA2D_ARGB8888;
  }
  hDma2dHandler.Init.OutputOffset = OffLine;      
  
  hDma2dHandler.Instance = DMA2D;
  
  /* DMA2D Initialization */
  if(HAL_DMA2D_Init(&hDma2dHandler) == HAL_OK) 
  {
    if(HAL_DMA2D_ConfigLayer(&hDma2dHandler, LayerIndex) == HAL_OK) 
    {
      if (HAL_DMA2D_Start(&hDma2dHandler, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK)
      {
        /* Polling For DMA transfer */  
        HAL_DMA2D_PollForTransfer(&hDma2dHandler, 10);
      }
    }
  } 
}    

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
