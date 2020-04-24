/*
 * icmu_utils.h
 *
 *  Created on: Apr 5, 2020
 *      Author: Luczia
 */

#ifndef ICHAUSMU_ENCODER_H_
#define ICHAUSMU_ENCODER_H_

#define BUS_SIZE 16
#include "ichausmu/IcHausMu.hpp"


#ifdef __cplusplus
extern "C" {
#endif



//extern void test153(){return;};

static uint8_t txbuf[BUS_SIZE];
static uint8_t rxbuf[BUS_SIZE];
static uint8_t txbuf2[BUS_SIZE];
static uint8_t rxbuf2[BUS_SIZE];

extern void basicSpiComTest();
extern uint8_t spiInitFrame();
extern void basicSpiInitFrame();
extern uint16_t spiReadFrame();
extern void spiCalibFrame();
extern void ChangeCS(bool state);
extern void TransferIcMU(unsigned char* const txBuffer, unsigned char* rxBuffer, const unsigned int bufferLen);



#ifdef __cplusplus
}
#endif




#endif /* ICHAUSMU_ENCODER_H_ */
