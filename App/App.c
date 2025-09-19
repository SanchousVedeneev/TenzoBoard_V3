#include "App.h"
#include "BSP.h"
#include "ProtocolMbRtuSlaveCtrl.h"

App_typedef App;
extern BSP_typedef Bsp;

// @do
// Добавить сохранение во Flash

void app_main(void)
{
  bsp_init();
  app_init();
  while (1) // основной цикл
  {
    BSP_PWR_TENZO_ON;

    if (bsp_get_data_spi_ads1251(145) == SPI_ADC_OK)
    {
      app_adc_data_filter(Bsp.ADC_ADS1251.data_raw, ADC_ADS1251);
    }
    else
    {
      app_adc_data_filter(0, ADC_ADS1251);
    }

    if (bsp_get_data_spi_ads1231(145) == SPI_ADC_OK)
    {
      app_adc_data_filter(Bsp.ADC_ADS1231.data_raw, ADC_ADS1231);
    }
    else
    {
      app_adc_data_filter(0, ADC_ADS1231);
    }

    BSP_OUT_TOGGLE(BSP_LED_1);
    //BSP_PWR_TENZO_OFF;
    HAL_Delay(20);
  }
}

void app_init()
{
  protocolMbRtuSlaveCtrl_init(1);
  app_setupParam_init();
  app_adc_filter_init();
  return;
}

void app_setupParam_init()
{
  app_setupParam_setDefolt();
  return;
}

void app_setupParam_setDefolt()
{
  // -------------------------- ADC_ADS1251 -------------------------- //
  App.setupParam.ADC_ADS1251_order = 1;
  App.setupParam.ADC_ADS1251_filterN = 1;
  App.setupParam.ADC_ADS1251_shift = 0.0f;
  App.setupParam.ADC_ADS1251_kMul = 1.0f;
  // ------------------------ ADC_ADS1251 END ------------------------ //

  // -------------------------- ADC_ADS1231 -------------------------- //
  App.setupParam.ADC_ADS1231_order = 1;
  App.setupParam.ADC_ADS1231_filterN = 1;
  App.setupParam.ADC_ADS1231_shift = 0.0f;
  App.setupParam.ADC_ADS1231_kMul = 1.0f;
  // ------------------------ ADC_ADS1231 END ------------------------ //
  return;
}

void app_adc_filter_init()
{
  // -------------------------- ADC_ADS1251 -------------------------- //
  App.adc_filter[ADC_ADS1251].value = 0.0f;
  App.adc_filter[ADC_ADS1251].value_last = 0.0f;
  App.adc_filter[ADC_ADS1251].valueRaw = 0.0f;
  for (uint8_t j = 0; j < PROGRAM_ADC_MAX_FILTER_ORDER; j++)
  {
    App.adc_filter[ADC_ADS1251].buf[j] = 0.0f;
  }
  App.adc_filter[ADC_ADS1251].bufIdx = 0;
  App.adc_filter[ADC_ADS1251].filter_N = App.setupParam.ADC_ADS1251_filterN;
  App.adc_filter[ADC_ADS1251].order = App.setupParam.ADC_ADS1251_order;
  App.adc_filter[ADC_ADS1251].shift = 0.0f;
  App.adc_filter[ADC_ADS1251].kMul = 1.0f;
  // ------------------------ ADC_ADS1251 END ------------------------ //

  // -------------------------- ADC_ADS1231 -------------------------- //
  App.adc_filter[ADC_ADS1231].value = 0.0f;
  App.adc_filter[ADC_ADS1231].value_last = 0.0f;
  App.adc_filter[ADC_ADS1231].valueRaw = 0.0f;
  for (uint8_t j = 0; j < PROGRAM_ADC_MAX_FILTER_ORDER; j++)
  {
    App.adc_filter[ADC_ADS1231].buf[j] = 0.0f;
  }
  App.adc_filter[ADC_ADS1231].bufIdx = 0;
  App.adc_filter[ADC_ADS1231].filter_N = App.setupParam.ADC_ADS1231_filterN;
  App.adc_filter[ADC_ADS1231].order = App.setupParam.ADC_ADS1231_order;
  App.adc_filter[ADC_ADS1231].shift = 0.0f;
  App.adc_filter[ADC_ADS1231].kMul = 1.0f;
  // ------------------------ ADC_ADS1231 END ------------------------ //
  return;
}

void bsp_tim7_100ms_callback()
{
  app_update_reg();
  protocolMbRtuSlaveCtrl_update_tables();

  // BSP_PWR_TENZO_ON;
  // BSP_LED_ON(BSP_LED_1);

  // if (bsp_get_data_spi_ads1251(145) == SPI_ADC_OK)
  // {
  //   app_adc_data_filter(Bsp.ADC_ADS1251.data_raw, ADC_ADS1251);
  // }
  // else
  // {
  //   app_adc_data_filter(0, ADC_ADS1251);
  // }

  // if (bsp_get_data_spi_ads1231(145) == SPI_ADC_OK)
  // {
  //   app_adc_data_filter(Bsp.ADC_ADS1231.data_raw, ADC_ADS1231);
  // }
  // else
  // {
  //   app_adc_data_filter(0, ADC_ADS1231);
  // }

  // BSP_LED_OFF(BSP_LED_1);
  // BSP_PWR_TENZO_OFF;
  return;
}

#define K_X10 (float)(10.0f)
#define K_X1000 (float)(1000.0f)
#define K_X1000000 (float)(1000000.0f)
void app_update_reg()
{
  // --- ADC_ADS1251
  App.ADC_ADS1251.data_i16 = (int16_t)(App.adc_filter[ADC_ADS1251].value * K_X1000);    // [В*1000]
  App.ADC_ADS1251.data_i32 = (int32_t)(App.adc_filter[ADC_ADS1251].value * K_X1000000); // [В*1000000]

  // --- ADC_ADS1231
  App.ADC_ADS1231.data_i16 = (int16_t)(App.adc_filter[ADC_ADS1231].value * K_X1000);    // [В*1000]
  App.ADC_ADS1231.data_i32 = (int32_t)(App.adc_filter[ADC_ADS1231].value * K_X1000000); // [В*1000000]

  // --- DI_state
  if (BSP_GET_DI(BSP_IN1) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(App.DI_state, 0);
  }
  else
  {
    BSP_RESET_BIT(App.DI_state, 0);
  }

  // --- DI_rele_state
  App.DI_rele_state = bsp_get_rele_state();

  // --- DO_control
  if (HAL_GPIO_ReadPin(BSP_OUT1) == GPIO_PIN_SET)
  {
    BSP_SET_BIT(App.DO_control, 0);
  }
  else
  {
    BSP_RESET_BIT(App.DO_control, 0);
  }

  // -- ADC_T
  // App.ADC_T_data_i16 = (int16_t)(App.adc_filter[ADC_T].value * K_X10);                // [C*10]
}

#define ADC_24BIT_FUL_SCALE (float)(0x16777215UL)

#define ADC_ADS1251_MAX_VAL (float)(8388607.0f)
#define ADC_ADS1251_REF_VOLT (float)(4.096f)

#define ADC_ADS1231_MAX_VAL (float)(4194303.0f)
#define ADC_ADS1231_REF_VOLT (float)(2.5f)

void app_adc_data_filter(uint32_t ADC_Buf_raw, ADC_enum adc)
{
  float value = 0.0f;
  float valueLast = 0.0f;
  float kFilter = 0.0f;
  float data = 0.0f;
  float sum = 0.0f;

  if (adc == ADC_ADS1251)
  {
    // Положительное напряжение на входе АЦП
    if (ADC_Buf_raw <= (uint32_t)ADC_ADS1251_MAX_VAL)
    {
      data = (float)ADC_Buf_raw / ADC_ADS1251_MAX_VAL * ADC_ADS1251_REF_VOLT;
    }
    // Отрицательное напряжение на входе АЦП
    else
    {
      data = ((float)ADC_Buf_raw - ADC_24BIT_FUL_SCALE - 1.0f) / (ADC_ADS1251_MAX_VAL + 1.0f) * ADC_ADS1251_REF_VOLT;
    }
  }
  else if (adc == ADC_ADS1231)
  {
    // Положительное напряжение на входе АЦП
    if ((float)ADC_Buf_raw <= ADC_ADS1231_MAX_VAL)
    {
      data = (float)ADC_Buf_raw / ADC_ADS1231_MAX_VAL * ADC_ADS1231_REF_VOLT;
    }
    // Отрицательное напряжение на входе АЦП
    else
    {
      data = ((float)ADC_Buf_raw - (ADC_ADS1231_MAX_VAL * 2) - 1.0f) / (ADC_ADS1231_MAX_VAL + 1.0f) * ADC_ADS1231_REF_VOLT;
    }
  }

  App.adc_filter[adc].buf[App.adc_filter[adc].bufIdx++] = data;

  if (App.adc_filter[adc].bufIdx == App.adc_filter[adc].order)
  {
    App.adc_filter[adc].bufIdx = 0;
  }

  for (uint8_t idx = 0; idx < App.adc_filter[adc].order; idx++)
  {
    sum += App.adc_filter[adc].buf[idx];
  }

  App.adc_filter[adc].valueRaw = sum / (float)App.adc_filter[adc].order;

  value = (App.adc_filter[adc].valueRaw - App.adc_filter[adc].shift) * App.adc_filter[adc].kMul;

  valueLast = App.adc_filter[adc].value_last;

  kFilter = 2.0f / ((float)App.adc_filter[adc].filter_N + 1.0f);

  value = valueLast + kFilter * (value - valueLast);

  App.adc_filter[adc].value = App.adc_filter[adc].value_last = value;
}

int16_t app_get_temp_NTC(uint16_t value_raw)
{
  //@do - сделать преобразования для получения температуры
  asm("Nop");
  return 1;
}