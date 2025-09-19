
#include "ProtocolMbRtuSlaveCtrl.h"

#include "App.h"
#include "BSP.h"

uint8_t modbusBufRxTxRtu485[MODBUS_SS_BUF_CNT];

//--------------------  PROTOCOL ---------------------//
//---1
#define MDB_TABLE_BSP_REG_NO (1)
enum mdb_table_bsp
{
  tab_bsp_buf_8b_ADS1251_0 = MDB_TABLE_BSP_REG_NO,
  tab_bsp_buf_8b_ADS1251_1,
  tab_bsp_buf_8b_ADS1251_2,
  tab_bsp_buf_32b_ADS1251_LW,
  tab_bsp_buf_32b_ADS1251_HW,

  tab_bsp_buf_8b_ADS1231_0,
  tab_bsp_buf_8b_ADS1231_1,
  tab_bsp_buf_8b_ADS1231_2,
  tab_bsp_buf_32b_ADS1231_LW,
  tab_bsp_buf_32b_ADS1231_HW,

  tab_bsp_spi_read_state,

  tab_bsp_DI_state,
  tab_bsp_DI_rele_state,
  
  tab_bsp_DO_control,

  ab_bsp_buf_16b_AI_NTC,

  tab_bsp_control
};
#define MDB_BSP_BUF_COUNT (tab_bsp_control - MDB_TABLE_BSP_REG_NO + 1)
uint16_t mdb_bsp_buf[MDB_BSP_BUF_COUNT];

ModbusSS_table_t mdb_table_bsp = {
    .buf = (uint8_t *)mdb_bsp_buf,
    .quantity = MDB_BSP_BUF_COUNT,
    .regNo = MDB_TABLE_BSP_REG_NO,
    .type = ModbusSS_Holding};

//---101
#define MDB_TABLE_ANALOG_REG_NO (101)
enum mdb_table_analog
{
  tab_analog_ADS1251_filter_N = MDB_TABLE_ANALOG_REG_NO,
  tab_analog_ADS1251_order,
  tab_analog_ADS1251_shift,
  tab_analog_ADS1251_kMul,

  tab_analog_ADS1231_filter_N,
  tab_analog_ADS1231_order,
  tab_analog_ADS1231_shift,
  tab_analog_ADS1231_kMul,
};
#define MDB_ANALOG_BUF_COUNT (tab_analog_ADS1231_kMul - MDB_TABLE_ANALOG_REG_NO + 1)
uint16_t mdb_analog_buf[MDB_ANALOG_BUF_COUNT];
ModbusSS_table_t mdb_table_analog = {
    .buf = (uint8_t *)mdb_analog_buf,
    .quantity = MDB_ANALOG_BUF_COUNT,
    .regNo = MDB_TABLE_ANALOG_REG_NO,
    .type = ModbusSS_Holding};

//---201
#define MDB_TABLE_ADC_REG_NO (201)
enum mdb_table_adc
{
  tab_adc_ADS1251_data_i16 = MDB_TABLE_ADC_REG_NO,
  tab_adc_ADS1251_data_i32_LW,
  tab_adc_ADS1251_data_i32_HW,

  tab_adc_ADS1231_data_i16,
  tab_adc_ADS1231_data_i32_LW,
  tab_adc_ADS1231_data_i32_HW,

  tab_adc_NTC_T_i16,

  tab_adc_state
};
#define MDB_ADC_BUF_COUNT (tab_adc_state - MDB_TABLE_ADC_REG_NO + 1)
uint16_t mdb_adc_buf[MDB_ADC_BUF_COUNT];

ModbusSS_table_t mdb_table_adc = {
    .buf = (uint8_t *)mdb_adc_buf,
    .quantity = MDB_ADC_BUF_COUNT,
    .regNo = MDB_TABLE_ADC_REG_NO,
    .type = ModbusSS_Holding};
//--------------------  PROTOCOL END---------------------//

//--------------------  TABLES ARRAY ---------------------//
ModbusSS_table_t *modbusTables[] = {
  &mdb_table_bsp,
  &mdb_table_analog,
  &mdb_table_adc
};
//--------------------  TABLES ARRAY END---------------------//

//--------------------  MODBUS STRUCT ---------------------//
ModbusSS_t modbusSS_rtu_rs485 = {
    .cbHoldingUpdate = protocolMbRtuSlaveCtrl_callback_H_WRITE,
    .cbHoldingRequest = NULL, // protocolMbRtuSlaveCtrl_callback_H_REQ, //modbusHoldingReq,
    .rtuTcp = MODBUS_SS_RTU,
    .bufRxTx = modbusBufRxTxRtu485,
    .slaveId = 1,
    .tables = modbusTables,
    .tablesCount = 3
};
//--------------------  MODBUS STRUCT END---------------------//

//------------------------ EXTERN ------------------------
extern App_typedef App;
extern BSP_typedef Bsp;
//---------------------- EXTERN END-----------------------

//------------------------ REGULAR FCN ------------------------
void protocolMbRtuSlaveCtrl_init(uint8_t portNo)
{
  HAL_Delay(100);
  bsp_rs485_setPortToModbusRtu(portNo, modbusBufRxTxRtu485, MODBUS_SS_BUF_CNT);
  asm("Nop");
  //HAL_Delay(100);
  modbusSS_rtu_rs485.slaveId = bsp_get_adr_mdb();
}

__INLINE void protocolMbRtuSlaveCtrl_update_tables()
{
  // --mdb_table_bsp
  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_buf_8b_ADS1251_0,        Bsp.ADC_ADS1251.spi_buf[0]);
  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_buf_8b_ADS1251_1,        Bsp.ADC_ADS1251.spi_buf[1]);
  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_buf_8b_ADS1251_2,        Bsp.ADC_ADS1251.spi_buf[2]);
  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_buf_32b_ADS1251_LW,      (uint16_t)(Bsp.ADC_ADS1251.data_raw & 0x0000FFFFUL));
  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_buf_32b_ADS1251_HW,      (uint16_t)((Bsp.ADC_ADS1251.data_raw & 0xFFFF0000UL) >> 16));

  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_buf_8b_ADS1231_0,        Bsp.ADC_ADS1231.spi_buf[0]);
  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_buf_8b_ADS1231_1,        Bsp.ADC_ADS1231.spi_buf[1]);
  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_buf_8b_ADS1231_2,        Bsp.ADC_ADS1231.spi_buf[2]);
  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_buf_32b_ADS1231_LW,      (uint16_t)(Bsp.ADC_ADS1231.data_raw & 0x0000FFFFUL));
  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_buf_32b_ADS1231_HW,      (uint16_t)((Bsp.ADC_ADS1231.data_raw & 0xFFFF0000UL) >> 16));

  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_spi_read_state,          Bsp.SPI_ADC_state);

  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_DI_state,                App.DI_state);
  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_DI_rele_state,           App.DI_rele_state);

  ModbusSS_SetWord(&mdb_table_bsp, tab_bsp_DO_control,              App.DO_control);

  ModbusSS_SetWord(&mdb_table_bsp, ab_bsp_buf_16b_AI_NTC,           Bsp.NTC_value_raw);

  // --mdb_table_analog
  ModbusSS_SetWord(&mdb_table_analog, tab_analog_ADS1251_filter_N,  App.adc_filter[ADC_ADS1251].filter_N);
  ModbusSS_SetWord(&mdb_table_analog, tab_analog_ADS1251_order,     App.adc_filter[ADC_ADS1251].order);
  ModbusSS_SetWord(&mdb_table_analog, tab_analog_ADS1251_shift,     (uint16_t)App.adc_filter[ADC_ADS1251].shift);
  ModbusSS_SetWord(&mdb_table_analog, tab_analog_ADS1251_kMul,      (uint16_t)App.adc_filter[ADC_ADS1251].kMul);

  ModbusSS_SetWord(&mdb_table_analog, tab_analog_ADS1231_filter_N,  App.adc_filter[ADC_ADS1231].filter_N);
  ModbusSS_SetWord(&mdb_table_analog, tab_analog_ADS1231_order,     App.adc_filter[ADC_ADS1231].order);
  ModbusSS_SetWord(&mdb_table_analog, tab_analog_ADS1231_shift,     (uint16_t)App.adc_filter[ADC_ADS1231].shift);
  ModbusSS_SetWord(&mdb_table_analog, tab_analog_ADS1231_kMul,      (uint16_t)App.adc_filter[ADC_ADS1231].kMul);

  // --mdb_table_adc
  ModbusSS_SetWord(&mdb_table_adc, tab_adc_ADS1251_data_i16,        App.ADC_ADS1251.data_i16);
  ModbusSS_SetWord(&mdb_table_adc, tab_adc_ADS1251_data_i32_LW,     (uint16_t)(App.ADC_ADS1251.data_i32 & 0x0000FFFFUL));
  ModbusSS_SetWord(&mdb_table_adc, tab_adc_ADS1251_data_i32_HW,     (uint16_t)((App.ADC_ADS1251.data_i32 & 0xFFFF0000UL) >> 16));

  ModbusSS_SetWord(&mdb_table_adc, tab_adc_ADS1231_data_i16,        App.ADC_ADS1231.data_i16);
  ModbusSS_SetWord(&mdb_table_adc, tab_adc_ADS1231_data_i32_LW,     (uint16_t)(App.ADC_ADS1231.data_i32 & 0x0000FFFFUL));
  ModbusSS_SetWord(&mdb_table_adc, tab_adc_ADS1231_data_i32_HW,     (uint16_t)((App.ADC_ADS1231.data_i32 & 0xFFFF0000UL) >> 16));

  ModbusSS_SetWord(&mdb_table_adc, tab_adc_NTC_T_i16,               (uint16_t)App.ADC_T_data_i16);

  ModbusSS_SetWord(&mdb_table_adc, tab_adc_state,                   Bsp.SPI_ADC_state);

  
}
//------------------------ REGULAR FCN END------------------------

//------------------------------- MODBUS CALLBACKS -------------------------------------------//
__weak void protocolMbRtuSlaveCtrl_callback_H_WRITE(ModbusSS_table_t *table, uint16_t reg, uint16_t quantity)
{

  uint16_t value = 0;
  asm("NOP");

  if (table == &mdb_table_bsp) // Диапазон BSP
  {
    value = ModbusSS_GetWord(&mdb_table_bsp, reg);
    switch (reg)
    {
    case tab_bsp_DO_control:
      if (BSP_GET_BIT(value, 0) == 1)
      {
        BSP_OUT_SET(BSP_OUT1);
        BSP_SET_BIT(App.DO_control, 0);
      }
      else
      {
        BSP_OUT_RESET(BSP_OUT1);
        BSP_RESET_BIT(App.DO_control, 0);
      }
      break;
    default:
      break;
    }
  }
  
}

__weak void protocolMbRtuSlaveCtrl_callback_H_READ(ModbusSS_table_t *table, uint16_t reg, uint16_t quantity)
{

  asm("NOP");
  //             if (table == &modbusTableHolding1)
  // {
  //   for (int r = reg; r < reg + quantity; r++)
  //   {
  //     asm("NOP");
  //     switch (r)
  //     {
  //     case MBP_AI1_X:
  //       ModbusSS_SetWord(table, r, bsp_ai_read_cache(BSP_AI1));
  //       asm("NOP");
  //       break;
  //     case MBP_AI2_Y:
  //       ModbusSS_SetWord(table, r, bsp_ai_read_cache(BSP_AI2));
  //       asm("NOP");
  //       break;
  //     case MBP_DI_STATE:
  //       ModbusSS_SetWord(table, r, bsp_di_get_cache_pack16());
  //       asm("NOP");
  //       break;
  //     default:
  //       break;
  //     }
  //   }
  // }
}
//------------------------------- MODBUS CALLBACKS END-------------------------------------------//

//------------------------------- HW CALLBACK -------------------------------------------//
void bsp_rs485_callback_rxBlockReady(uint8_t portNo)
{

  int32_t blockSizeByte = 0;
  if ((blockSizeByte = ModbusSS_ParseRxData(&modbusSS_rtu_rs485)) == 0)
  {
    // bug with reset modbus!!!
    asm("NOP");
  }
  else if (blockSizeByte != -1)
  {
    asm("NOP");
    BSP_LED_TOGGLE(BSP_LED_2);
    bsp_rs485_sendBlock(portNo, modbusSS_rtu_rs485.bufRxTx, blockSizeByte);
    asm("NOP");
  }
  else
  {
    asm("NOP");
  }
}
//------------------------------- HW CALLBACK END-------------------------------------------//




