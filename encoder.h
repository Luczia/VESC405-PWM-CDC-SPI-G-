/*
 * icmu_utils.h
 *
 *  Created on: Apr 5, 2020
 *      Author: Luczia
 */

#ifndef ICHAUSMU_ENCODER_H_
#define ICHAUSMU_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif




#include "ichausmu/IcHausMu.hpp"






//extern void test153(){return;};



void initIcHausMu();
extern uint8_t test_spi(uint16_t *val);
extern void spiInitFrame(uint8_t* init_seq);
extern void basicSpiInitFrame();
extern uint8_t spiReadFrame(uint16_t *val);
extern void spiCalibFrame();
extern void ChangeCS(bool state);
extern void TransferIcMU(unsigned char* const txBuffer, unsigned char* rxBuffer, const unsigned int bufferLen);



#ifdef __cplusplus
}
#endif




#endif /* ICHAUSMU_ENCODER_H_ */
