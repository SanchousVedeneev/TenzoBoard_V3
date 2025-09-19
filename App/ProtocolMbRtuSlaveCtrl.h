#ifndef __ProtocolMbRtuSlaveCtrl__H__
#define __ProtocolMbRtuSlaveCtrl__H__

//#include "BSP.h"
#include "ModbusSS.h"

void protocolMbRtuSlaveCtrl_init(uint8_t portNo);

void protocolMbRtuSlaveCtrl_update_tables();

void protocolMbRtuSlaveCtrl_callback_H_READ(ModbusSS_table_t* table, uint16_t reg, uint16_t quantity);
void protocolMbRtuSlaveCtrl_callback_H_WRITE(ModbusSS_table_t* table, uint16_t reg, uint16_t quantity);


#endif