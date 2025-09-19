#include "BSP.h"

BSP_typedef Bsp;

// ------------------------------ INIT ------------------------------
void bsp_init()
{
  MX_TIM7_Init();
  MX_TIM1_Init();
  
  SysTick_Config(SystemCoreClock/1000);

  HAL_Delay(100);
  bsp_tim7_100ms_start();

  HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);

  bsp_tim6_300ms_start();

  HAL_ADCEx_InjectedStart_IT(&hadc2);
  return;
}
// ---------------------------- INIT END ----------------------------

// ------------------------------ RS485 ------------------------------
#define BSP_RS485_1 huart1

#define BSP_RS_485_RX_TIMEOUT (2000)

static uint16_t timer_rs485_timeout[2] = {BSP_RS_485_RX_TIMEOUT, BSP_RS_485_RX_TIMEOUT};

void bsp_rs485_setPortToModbusRtu(uint8_t portNo, uint8_t *bufRxTX, uint16_t bufSizeByte)
{

  UART_HandleTypeDef *port = NULL;
  HAL_Delay(100);
  if (portNo == 1)
    port = &BSP_RS485_1;
  else
    // port = &BSP_RS485_2;
    ;
  port->Instance->RTOR |= 0xfff << USART_RTOR_RTO_Pos;
  port->Instance->CR1 |= USART_CR1_RTOIE;
  port->Instance->CR2 |= USART_CR2_RTOEN;
  //__HAL_UART_ENABLE_IT(&huart3, UART_IT_TC);
  port->NbRxDataToProcess = bufSizeByte;
  // port->RxXferSize = bufSizeByte;

  HAL_UART_Abort(port);
  HAL_DMA_Start(port->hdmarx, (uint32_t)&port->Instance->RDR, (uint32_t)bufRxTX, bufSizeByte);
  port->Instance->CR3 |= USART_CR3_DMAR;
}

void bsp_rs485_sendBlock(uint8_t portNo, uint8_t *buf, uint8_t bufSizeByte)
{
  UART_HandleTypeDef *port = NULL;
  if (portNo == 1)
    port = &BSP_RS485_1;
  else
    // port = &BSP_RS485_2;
    ;
  // HAL_UART_Abort(port);
  port->hdmatx->State = HAL_DMA_STATE_READY;
  __HAL_UNLOCK(port->hdmatx);
  HAL_DMA_Start(port->hdmatx, (uint32_t)buf, (uint32_t)&port->Instance->TDR, bufSizeByte);
  port->Instance->CR3 |= USART_CR3_DMAT;
}

void bsp_rs485_sendTestBlock(uint8_t portNo)
{
  static uint8_t testBuf[5] = {0xAA, 1, 2, 3, 4};
  bsp_rs485_sendBlock(portNo, testBuf, 5);
}

__weak void bsp_rs485_callback_rxBlockReady(uint8_t portNo)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(portNo);
  /*
      Если callback нужен его необходимо переопределить.
      void bsp_rs485_callback_rxBlockReady(UART_HandleTypeDef* port){

      }
      Удалять функцию не надо.
  */
}

__weak void bsp_rs485_callback_rxTimeout(uint8_t portNo)
{
}

void BSP_RS485_1_IRQ_HANDLER_RTOF(void)
{
  // BLOCK RX READY
  if (__HAL_UART_GET_FLAG(&BSP_RS485_1, UART_FLAG_RTOF))
  {
    __HAL_UART_CLEAR_FLAG(&BSP_RS485_1, UART_CLEAR_RTOF);

    __HAL_DMA_DISABLE(BSP_RS485_1.hdmarx);
    __HAL_UART_CLEAR_FLAG(&BSP_RS485_1, UART_CLEAR_OREF);

    // MAIN_LED1_TOGGLE();

    if (BSP_RS485_1.NbRxDataToProcess != BSP_RS485_1.hdmarx->Instance->CNDTR)
    {
      bsp_rs485_callback_rxBlockReady(1);
      // bsp_dInOut_toggleDout(bsp_dInOut_led_rs485_1_g);
      // bsp_dInOut_resetDout(bsp_dInOut_led_rs485_1_y);
      timer_rs485_timeout[0] = BSP_RS_485_RX_TIMEOUT;
    }

    BSP_RS485_1.hdmarx->Instance->CNDTR = BSP_RS485_1.NbRxDataToProcess;
    __HAL_DMA_ENABLE(BSP_RS485_1.hdmarx);
    BSP_RS485_1.Instance->CR3 |= USART_CR3_DMAR;
  }
  // TRANSFER COMPLETE
  else if (__HAL_UART_GET_FLAG(&BSP_RS485_1, UART_FLAG_TC))
  {
    __HAL_UART_CLEAR_FLAG(&BSP_RS485_1, UART_FLAG_TC);
    asm("NOP");
    if (BSP_RS485_1.hdmatx->Instance->CNDTR == 0)
    {
      // LL_GPIO_ResetOutputPin(O_D_RS485_DE_GPIO_Port,O_D_RS485_DE_Pin);
    }
  }
  else
  {
    Error_Handler();
    NVIC_SystemReset();
  }
}

#define ADR_MDB_MIN  (uint8_t)(1)
#define ADR_MDB_MAX  (uint8_t)(63)
uint8_t bsp_get_adr_mdb()
{
  uint8_t adr_mdb = 0;

  // adr_mdb |= BSP_GET_DI(BSP_ADR_0) << 0;
  // adr_mdb |= BSP_GET_DI(BSP_ADR_1) << 1;
  // adr_mdb |= BSP_GET_DI(BSP_ADR_2) << 2;
  // adr_mdb |= BSP_GET_DI(BSP_ADR_3) << 3;
  // adr_mdb |= BSP_GET_DI(BSP_ADR_4) << 4;
  // adr_mdb |= BSP_GET_DI(BSP_ADR_5) << 5;

  if (BSP_GET_DI(BSP_ADR_0) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(adr_mdb, 0);
    asm("Nop");
  }
  else
  {
    BSP_RESET_BIT(adr_mdb, 0);
    asm("Nop");
  }

  if (BSP_GET_DI(BSP_ADR_1) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(adr_mdb, 1);
    asm("Nop");
  }
  else
  {
    BSP_RESET_BIT(adr_mdb, 1);
    asm("Nop");
  }

  if (BSP_GET_DI(BSP_ADR_2) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(adr_mdb, 2);
    asm("Nop");
  }
  else
  {
    BSP_RESET_BIT(adr_mdb, 2);
    asm("Nop");
  }

  if (BSP_GET_DI(BSP_ADR_3) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(adr_mdb, 3);
    asm("Nop");
  }
  else
  {
    BSP_RESET_BIT(adr_mdb, 3);
    asm("Nop");
  }

  if (BSP_GET_DI(BSP_ADR_4) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(adr_mdb, 4);
    asm("Nop");
  }
  else
  {
    BSP_RESET_BIT(adr_mdb, 4);
    asm("Nop");
  }

  if (BSP_GET_DI(BSP_ADR_5) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(adr_mdb, 5);
    asm("Nop");
  }
  else
  {
    BSP_RESET_BIT(adr_mdb, 5);
    asm("Nop");
  }

  if (adr_mdb < ADR_MDB_MIN)
  {
    adr_mdb = ADR_MDB_MIN;
  }
  else if (adr_mdb > ADR_MDB_MAX)
  {
    adr_mdb = ADR_MDB_MAX;
  }

  asm("Nop");
  asm("Nop");
  asm("Nop");
  return adr_mdb;
}

uint8_t bsp_get_rele_state()
{
  uint8_t adr_mdb = 0;

  if (BSP_GET_DI(BSP_ADR_0) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(adr_mdb, 0);
    asm("Nop");
  }
  else
  {
    BSP_RESET_BIT(adr_mdb, 0);
    asm("Nop");
  }

  if (BSP_GET_DI(BSP_ADR_1) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(adr_mdb, 1);
    asm("Nop");
  }
  else
  {
    BSP_RESET_BIT(adr_mdb, 1);
    asm("Nop");
  }

  if (BSP_GET_DI(BSP_ADR_2) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(adr_mdb, 2);
    asm("Nop");
  }
  else
  {
    BSP_RESET_BIT(adr_mdb, 2);
    asm("Nop");
  }

  if (BSP_GET_DI(BSP_ADR_3) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(adr_mdb, 3);
    asm("Nop");
  }
  else
  {
    BSP_RESET_BIT(adr_mdb, 3);
    asm("Nop");
  }

  if (BSP_GET_DI(BSP_ADR_4) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(adr_mdb, 4);
    asm("Nop");
  }
  else
  {
    BSP_RESET_BIT(adr_mdb, 4);
    asm("Nop");
  }

  if (BSP_GET_DI(BSP_ADR_5) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(adr_mdb, 5);
    asm("Nop");
  }
  else
  {
    BSP_RESET_BIT(adr_mdb, 5);
    asm("Nop");
  }

  return adr_mdb;
}
// ------------------------------ RS485 END ------------------------------

// ------------------------------ TIM ------------------------------------
void bsp_tim7_100ms_start()
{
  HAL_TIM_Base_Start_IT(&htim7);
}

void TIM7_DAC_IRQHandler(void)
{
  if (__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE) != RESET)
  {
    if (__HAL_TIM_GET_IT_SOURCE(&htim7, TIM_IT_UPDATE) != RESET)
    {
      __HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
      bsp_tim7_100ms_callback();
    }
  }
  return;
}

__weak void bsp_tim7_100ms_callback()
{
  asm("Nop");
}

void bsp_tim6_300ms_start()
{
  HAL_TIM_Base_Start(&htim6);
}
// ---------------------------- TIM END ----------------------------------

// ------------------------------ ADC ------------------------------------
//АЦП синхронизировано с TIM6
void ADC1_2_IRQHandler(void)
{
  if (__HAL_ADC_GET_FLAG(&hadc2, ADC_FLAG_JEOS) != 0)
  {
    __HAL_ADC_CLEAR_FLAG(&hadc2, ADC_FLAG_JEOS);
    Bsp.NTC_value_raw = HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1)/16;
  }
}

__weak void bsp_ADC_data_ready()
{
  asm("Nop");
}
// ---------------------------- ADC END ----------------------------------

// ------------------------------ SPI ------------------------------------
SPI_ADC_status_typedef bsp_get_data_spi_ads1251(uint8_t timeout)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick();

  // Выход TIM1 тактирует АЦП ADS1251
  // Частота тактирования fCLK = 30720 Гц
  // Период преобразвания АЦП T = 0.0125 с, f = 80 Гц

  // Включаем тактирование АЦП
  HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);

  // Производим 5 преобразвоаний АЦП для большей точности, берем значение 5-го преобразования
  for (uint8_t i = 0; i < 5; i++)
  {
    while ((BSP_GET_DI(BSP_SPI2_MISO) == GPIO_PIN_RESET) && ((HAL_GetTick() - tickstart) <  timeout))  { asm("Nop"); }
    while ((BSP_GET_DI(BSP_SPI2_MISO) == GPIO_PIN_SET)   && ((HAL_GetTick() - tickstart) <  timeout))  { asm("Nop"); }
    while ((BSP_GET_DI(BSP_SPI2_MISO) == GPIO_PIN_RESET) && ((HAL_GetTick() - tickstart) <  timeout))  { asm("Nop"); }
    while ((BSP_GET_DI(BSP_SPI2_MISO) == GPIO_PIN_SET)   && ((HAL_GetTick() - tickstart) <  timeout))  { asm("Nop"); }

    for (uint8_t i = 0; i < 200; i++) { asm("Nop"); }

    HAL_SPI_Receive(&hspi2, &Bsp.ADC_ADS1251.spi_buf[0], 4, 1);
  }

  // Выключаем тактирование АЦП
  HAL_TIM_OC_Stop_IT(&htim1, TIM_CHANNEL_1);

  Bsp.ADC_ADS1251.data_raw |= ((uint32_t)Bsp.ADC_ADS1251.spi_buf[0] << 16);
  Bsp.ADC_ADS1251.data_raw |= ((uint32_t)Bsp.ADC_ADS1251.spi_buf[1] << 8);
  Bsp.ADC_ADS1251.data_raw |= ((uint32_t)Bsp.ADC_ADS1251.spi_buf[2] << 0);

  if ((HAL_GetTick() - tickstart) >=  timeout)
  {
    BSP_SET_BIT(Bsp.SPI_ADC_state, 0);
    return SPI_ADC_TIMEOUT;
  }
  else
  {
    BSP_RESET_BIT(Bsp.SPI_ADC_state, 0);
    return SPI_ADC_OK;
  }
}

SPI_ADC_status_typedef bsp_get_data_spi_ads1231(uint8_t timeout)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick();
  // Период преобразвания АЦП T = 0.0125 с, f = 80 Гц
  
  //#define TEST_ADS1231
  #ifdef TEST_ADS1231
    HAL_SPI_Receive(&hspi1, &Bsp.ADC_ADS1231.spi_buf[0], 4, 1);
    Bsp.ADC_ADS1231.data_raw |= ((uint32_t)Bsp.ADC_ADS1231.spi_buf[0] << 16);
    Bsp.ADC_ADS1231.data_raw |= ((uint32_t)Bsp.ADC_ADS1231.spi_buf[1] << 8);
    Bsp.ADC_ADS1231.data_raw |= ((uint32_t)Bsp.ADC_ADS1231.spi_buf[2] << 0);
    return Bsp.ADC_ADS1231.data_raw;
  #endif

  while ((BSP_GET_DI(BSP_SPI1_MISO) == GPIO_PIN_SET)   && ((HAL_GetTick() - tickstart) <  timeout))  { asm("Nop"); }
  while ((BSP_GET_DI(BSP_SPI1_MISO) == GPIO_PIN_RESET) && ((HAL_GetTick() - tickstart) <  timeout))  { asm("Nop"); }

  // Производим 5 преобразвоаний АЦП для большей точности, берем значение 5-го преобразования
  for (uint8_t i = 0; i < 5; i++)
  {
    while ((BSP_GET_DI(BSP_SPI1_MISO) == GPIO_PIN_SET) && ((HAL_GetTick() - tickstart) <  timeout))  { asm("Nop"); }

    for (uint8_t i = 0; i < 200; i++) { asm("Nop"); }

    HAL_SPI_Receive(&hspi1, &Bsp.ADC_ADS1231.spi_buf[0], 4, 1);
  }
  
  Bsp.ADC_ADS1231.data_raw |= ((uint32_t)Bsp.ADC_ADS1231.spi_buf[0] << 16);
  Bsp.ADC_ADS1231.data_raw |= ((uint32_t)Bsp.ADC_ADS1231.spi_buf[1] << 8);
  Bsp.ADC_ADS1231.data_raw |= ((uint32_t)Bsp.ADC_ADS1231.spi_buf[2] << 0);

  if ((HAL_GetTick() - tickstart) >=  timeout)
  {
    BSP_SET_BIT(Bsp.SPI_ADC_state, 1);
    return SPI_ADC_TIMEOUT;
  }
  else
  {
    BSP_RESET_BIT(Bsp.SPI_ADC_state, 1);
    return SPI_ADC_OK;
  }

}
// ---------------------------- SPI END ----------------------------------