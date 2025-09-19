#ifndef __APP__H__
#define __APP__H__

#include "main.h"

typedef enum
{
    ADC_ADS1251 = 0,
    ADC_ADS1231 = 1,
} ADC_enum;

typedef struct 
{
    uint8_t ADC_ADS1251_order;
    uint8_t ADC_ADS1251_filterN;
    float ADC_ADS1251_shift;
    float ADC_ADS1251_kMul;

    uint8_t ADC_ADS1231_order;
    uint8_t ADC_ADS1231_filterN;
    float ADC_ADS1231_shift;
    float ADC_ADS1231_kMul;

} setupParam_typedef;

typedef struct 
{
    int16_t  data_i16;
    int32_t  data_i32;
} ADC_typedef;

#define PROGRAM_ADC_MAX_FILTER_ORDER   (5)
typedef struct {
    float value;
    float value_last;
    float valueRaw;
    float buf[PROGRAM_ADC_MAX_FILTER_ORDER];
    uint8_t bufIdx;
    uint8_t filter_N;
    uint8_t order;
    float shift;
    float kMul;
}ADC_filter_typedef;

#define COUNT_ADC (2)
typedef struct 
{
    ADC_filter_typedef adc_filter[COUNT_ADC];
    ADC_typedef ADC_ADS1251;
    ADC_typedef ADC_ADS1231;
    int16_t ADC_T_data_i16;
    setupParam_typedef setupParam;
    uint8_t DI_state;
    uint8_t DI_rele_state;
    uint8_t DO_control;
} App_typedef;

//------------------------------ FUNCTION ------------------------------//
void app_main();
void app_init();
void app_update_reg();
void app_adc_data_filter(uint32_t ADC_Buf_raw, ADC_enum ADC);
void app_adc_filter_init();
void app_setupParam_init();
void app_setupParam_setDefolt();

int16_t app_get_temp_NTC(uint16_t value_raw);
//---------------------------- FUNCTION END ----------------------------//

#endif