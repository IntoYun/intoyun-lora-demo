#include "hal_interface.h"

static SPI_HandleTypeDef spiHandle;
static interruptCb dio0Irq = NULL;
static interruptCb dio1Irq = NULL;
static interruptCb dio2Irq = NULL;
static interruptCb dio3Irq = NULL;

//spi初始化
static void HalSpiInit( void )
{
    //使用SPI1
    //PA15--NSS
    //PB3--SCK
    //PB4--MISO
    //PB5--MOSI

    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /* SPI_INTERFACE Config -------------------------------------------------------------*/
    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Pin       = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    spiHandle.Instance                = SPI1;
    spiHandle.Init.Mode               = SPI_MODE_MASTER;
    spiHandle.Init.Direction          = SPI_DIRECTION_2LINES;
    spiHandle.Init.DataSize           = SPI_DATASIZE_8BIT;
    spiHandle.Init.CLKPolarity        = SPI_POLARITY_LOW;
    spiHandle.Init.CLKPhase           = SPI_PHASE_1EDGE;
    spiHandle.Init.NSS                = SPI_NSS_SOFT;
    spiHandle.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_16;
    spiHandle.Init.FirstBit           = SPI_FIRSTBIT_MSB;
    spiHandle.Init.TIMode             = SPI_TIMODE_DISABLE;
    spiHandle.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLE;
    spiHandle.Init.CRCPolynomial      = 7;

    HAL_SPI_Init(&spiHandle);
}

//引脚初始化
void HalPinInit(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;

   // Configure SPI_NSS
    GPIO_InitStructure.Pin = NSS_PIN;
    HAL_GPIO_WritePin(NSS_IOPORT,NSS_PIN,GPIO_PIN_SET);
    HAL_GPIO_Init(NSS_IOPORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pull  = GPIO_PULLUP;
    GPIO_InitStructure.Mode  = GPIO_MODE_IT_RISING;
    // Configure DIO0
    GPIO_InitStructure.Pin =  DIO0_PIN;
    HAL_GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure );

    // Configure DIO1
    GPIO_InitStructure.Pin =  DIO1_PIN;
    HAL_GPIO_Init( DIO1_IOPORT, &GPIO_InitStructure );

    // Configure DIO2
    GPIO_InitStructure.Pin =  DIO2_PIN;
    HAL_GPIO_Init( DIO2_IOPORT, &GPIO_InitStructure );

    // REAMARK: DIO3/4/5 configured are connected to IO expander

    // Configure DIO3 as input
    GPIO_InitStructure.Pin =  DIO3_PIN;
    HAL_GPIO_Init( DIO3_IOPORT, &GPIO_InitStructure );

    HAL_NVIC_SetPriority(EXTI0_IRQn, 6, 0);
    HAL_NVIC_SetPriority(EXTI1_IRQn, 6, 0);
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 6, 0);
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    // Configure DIO4 as input
    /* GPIO_InitStructure.Pin =  DIO4_PIN; */
    /* HAL_GPIO_Init( DIO4_IOPORT, &GPIO_InitStructure ); */

    // Configure DIO5 as input
    /* GPIO_InitStructure.Pin =  DIO5_PIN; */
    /* HAL_GPIO_Init( DIO5_IOPORT, &GPIO_InitStructure ); */

    //接收发送脚初始化
    GPIO_InitStructure.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull      = GPIO_NOPULL;
    GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_InitStructure.Pin =  RXTX_PIN;
    HAL_GPIO_Init( RXTX_IOPORT, &GPIO_InitStructure );

    HalSpiInit();
}

//控制spi nss信号
void HalSpiWriteNss(uint8_t level)
{
    if(!level){
        HAL_GPIO_WritePin(NSS_IOPORT,NSS_PIN,GPIO_PIN_RESET);
    }else{
        HAL_GPIO_WritePin(NSS_IOPORT,NSS_PIN,GPIO_PIN_SET);
    }
}

uint8_t HalSpiTransmit( uint8_t outData )
{
    uint8_t txData = outData;
    uint8_t rxData = 0;
    HAL_SPI_TransmitReceive(&spiHandle, &txData, &rxData, 1, 100);
    return rxData;
}

//控制复位信号
void HalWriteReset(uint8_t level)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    if(!level){ //输出低电平
        HAL_GPIO_WritePin(RESET_IOPORT,RESET_PIN,GPIO_PIN_RESET);
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStructure.Pin = RESET_PIN;
        HAL_GPIO_Init( RESET_IOPORT, &GPIO_InitStructure );
    }else{ //设为输入
        HAL_GPIO_WritePin(RESET_IOPORT,RESET_PIN,GPIO_PIN_SET);
        GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStructure.Pin =  RESET_PIN;
        HAL_GPIO_Init( RESET_IOPORT, &GPIO_InitStructure );
    }
}

//控制发射接收引脚 高收低发
void HalWriteRxTx(uint8_t level)
{
    if(!level){  //输出低
        HAL_GPIO_WritePin( RXTX_IOPORT, RXTX_PIN, GPIO_PIN_RESET );
    }else{  //输出高
        HAL_GPIO_WritePin( RXTX_IOPORT, RXTX_PIN, GPIO_PIN_SET );
    }
}

void HalAttachInterrupt(uint16_t pin, interruptCb irq)
{
    switch(pin)
    {
    case DIO0_PIN:
        if(irq != NULL)
        {
            dio0Irq = irq;
        }
        break;
    case DIO1_PIN:
        if(irq != NULL)
        {
            dio1Irq = irq;
        }

        break;
    case DIO2_PIN:
        if(irq != NULL)
        {
            dio2Irq = irq;
        }

        break;
    case DIO3_PIN:
        if(irq != NULL)
        {
            dio3Irq = irq;
        }
        break;
    case DIO4_PIN:
        break;
    case DIO5_PIN:
        break;
    default:
        break;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
    case DIO0_PIN:
        if(dio0Irq != NULL)
        {
            dio0Irq();
        }
        break;
    case DIO1_PIN:
        if(dio1Irq != NULL)
        {
            dio1Irq();
        }
        break;
    case DIO2_PIN:
        if(dio2Irq != NULL)
        {
            dio2Irq();
        }
        break;
    case DIO3_PIN:
        if(dio3Irq != NULL)
        {
            dio3Irq();
        }
        break;
    case DIO4_PIN:
    case DIO5_PIN:
    default:
        break;
    }
}

void delay(uint32_t ms)
{
    HAL_Delay(ms);
}

uint32_t millis(void)
{
    return HAL_GetTick();
}
