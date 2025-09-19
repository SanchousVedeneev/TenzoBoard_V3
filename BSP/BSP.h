#ifndef __BSP__H__
#define __BSP__H__

#include "main.h"
#include "tim.h"
#include "spi.h"
#include "usart.h"
#include "adc.h"

//------------------------------ DEFINE ------------------------------//
#define BSP_LED_1                 DO_LED_1_GPIO_Port, DO_LED_1_Pin
#define BSP_LED_2                 DO_LED_2_GPIO_Port, DO_LED_2_Pin

#define BSP_LED_ON(LED)			  HAL_GPIO_WritePin(LED, GPIO_PIN_SET)
#define BSP_LED_OFF(LED)	      HAL_GPIO_WritePin(LED, GPIO_PIN_RESET)
#define BSP_LED_TOGGLE(LED)		  HAL_GPIO_TogglePin(LED)

#define BSP_ADR_0                 DI_ADR_0_GPIO_Port, DI_ADR_0_Pin
#define BSP_ADR_1                 DI_ADR_1_GPIO_Port, DI_ADR_1_Pin
#define BSP_ADR_2                 DI_ADR_2_GPIO_Port, DI_ADR_2_Pin
#define BSP_ADR_3                 DI_ADR_3_GPIO_Port, DI_ADR_3_Pin
#define BSP_ADR_4                 DI_ADR_4_GPIO_Port, DI_ADR_4_Pin
#define BSP_ADR_5                 DI_ADR_5_GPIO_Port, DI_ADR_5_Pin

#define BSP_IN1                   DI_IN_1_GPIO_Port, DI_IN_1_Pin

#define BSP_OUT1                  DO_OUT_1_GPIO_Port, DO_OUT_1_Pin

#define BSP_SPI1_MISO             GPIOA, GPIO_PIN_6
#define BSP_SPI2_MISO             GPIOB, GPIO_PIN_14

#define BSP_OUT_SET(OUT)	      HAL_GPIO_WritePin(OUT, GPIO_PIN_SET)
#define BSP_OUT_RESET(OUT)	      HAL_GPIO_WritePin(OUT, GPIO_PIN_RESET)
#define BSP_OUT_TOGGLE(OUT)	      HAL_GPIO_TogglePin(OUT)

#define BSP_GET_DI(PORT_PIN)      (HAL_GPIO_ReadPin(PORT_PIN))

#define BSP_SLEEP                 DO_SLEEP_GPIO_Port, DO_SLEEP_Pin

#define BSP_PWR_TENZO_ON	      HAL_GPIO_WritePin(BSP_SLEEP, GPIO_PIN_SET)
#define BSP_PWR_TENZO_OFF         HAL_GPIO_WritePin(BSP_SLEEP, GPIO_PIN_RESET)

#define BSP_GET_BIT(REG, BIT)     (REG & (1 << BIT))
#define BSP_SET_BIT(REG, BIT)     (REG |= (1 << BIT))
#define BSP_RESET_BIT(REG, BIT)   (REG &= ~(1 << BIT))
//---------------------------- DEFINE END ----------------------------//

//------------------------------- ENUM -------------------------------//
typedef enum
{
    LED_1 = 0,
    LED_2,
    LED_3,
    LED_4,
    REL_1,
    REL_2,
    REL_3,
    REL_4
}LED_REL_ENUM;
//----------------------------- ENUM END -----------------------------//

// ----------------------------- RS-485 ----------------------------- //
void bsp_rs485_setPortToModbusRtu(uint8_t portNo, uint8_t *bufRxTX, uint16_t bufSizeByte);

void bsp_rs485_sendBlock(uint8_t portNo, uint8_t *buf, uint8_t bufSizeByte);
void bsp_rs485_sendTestBlock(uint8_t portNo);

void bsp_rs485_callback_rxBlockReady(uint8_t portNo);
void bsp_rs485_callback_rxTimeout(uint8_t portNo);

uint8_t bsp_get_adr_mdb();

#define BSP_RS485_1_IRQ_HANDLER_RTOF 			USART1_IRQHandler
#define BSP_RS485_1_IRQ_HANDLER_DMA_RX 			DMA1_Channel1_IRQHandler
#define BSP_RS485_1_IRQ_HANDLER_DMA_TX 			DMA1_Channel2_IRQHandler
// --------------------------- RS-485 END----------------------------- //

// ----------------------------- TIM ----------------------------- //
void bsp_tim7_100ms_start();
void bsp_tim7_100ms_callback();

void bsp_tim6_300ms_start();
// --------------------------- TIM END --------------------------- //

// ----------------------------- SPI ----------------------------- //
typedef enum
{
    SPI_ADC_OK        = 0x00U,
    SPI_ADC_TIMEOUT   = 0x01U
}SPI_ADC_status_typedef;

SPI_ADC_status_typedef bsp_get_data_spi_ads1251(uint8_t timeout);

SPI_ADC_status_typedef bsp_get_data_spi_ads1231(uint8_t timeout);

#define COUNT_REG_SPI_BUF (4)
typedef struct 
{
    uint8_t spi_buf[COUNT_REG_SPI_BUF];
    uint32_t data_raw;
} SPI_ADC_typedef;

typedef struct 
{
    SPI_ADC_typedef ADC_ADS1251;
    SPI_ADC_typedef ADC_ADS1231;
    uint8_t         SPI_ADC_state;
    uint16_t        NTC_value_raw;
} BSP_typedef;
// --------------------------- SPI END --------------------------- //

// ----------------------------- ADC ----------------------------- //

// --------------------------- ADC END --------------------------- //

void bsp_init();

uint8_t bsp_get_rele_state();

#endif