#ifndef __HAL_INTERFACE_H
#define __HAL_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include <stdint.h>


#define RESET_IOPORT                                GPIOB
#define RESET_PIN                                   GPIO_PIN_9

/*!
 * SX1278 SPI NSS I/O definitions
 */
#define NSS_IOPORT                                  GPIOA
#define NSS_PIN                                     GPIO_PIN_4

/*!
 * SX1278 DIO pins  I/O definitions
 */
#define DIO0_IOPORT                                 GPIOB
#define DIO0_PIN                                    GPIO_PIN_10

#define DIO1_IOPORT                                 GPIOB
#define DIO1_PIN                                    GPIO_PIN_2

#define DIO2_IOPORT                                 GPIOB
#define DIO2_PIN                                    GPIO_PIN_1

#define DIO3_IOPORT                                 GPIOB
#define DIO3_PIN                                    GPIO_PIN_0

#define DIO4_IOPORT                                 GPIOB
#define DIO4_PIN                                    GPIO_PIN_7

#define DIO5_IOPORT                                 GPIOB
#define DIO5_PIN                                    GPIO_PIN_8

//接收发送使能控制 高收低发
#define RXTX_IOPORT                                   GPIOB
#define RXTX_PIN                                      GPIO_PIN_5

typedef void (*interruptCb)(void);

void HalPinInit(void);
void HalSpiWriteNss(uint8_t level);
uint8_t HalSpiTransmit( uint8_t outData );
void HalWriteReset(uint8_t level);
void HalWriteRxTx(uint8_t level);
void HalAttachInterrupt(uint16_t pin, interruptCb irq);
void delay(uint32_t ms);
uint32_t millis(void);

#ifdef __cplusplus
}
#endif

#endif
