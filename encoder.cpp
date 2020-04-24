/*
 * encoder.cpp
 *
 *  Created on: Apr 24, 2020
 *      Author: Luczia
 */

#include "ch.h"
#include "hal.h"
#include "encoder.h"

IcHausMu ichausmu_enc(ICHAUSMU_HW_REV);

extern void basicSpiComTest()
{
  uint16_u pos;
  //encoder.readPos(&pos);

  for (int i=0 ; i<BUS_SIZE; i++)
      {
        txbuf2[i] = (uint8_t)i%255;
        rxbuf2[i] = (uint8_t)i%255;
      }

  spiAcquireBus(&SPID1);              // Acquire ownership of the bus.
  spiSelect(&SPID1);                  // Slave Select assertion.
  spiExchange(&SPID1, BUS_SIZE, txbuf2, rxbuf2);
      //spiSend(SPID1,*/
  spiUnselect(&SPID1);                // Slave Select de-assertion.
  spiReleaseBus(&SPID1);
}

extern uint8_t spiInitFrame()
{
  fctStatus init_encoder = ichausmu_enc.init();
  ichausmu_enc.setAutomaticGain(true);

  return (uint8_t)init_encoder;
}



extern void basicSpiInitFrame()
{
  txbuf2[0] = (uint8_t)0x97;
  txbuf2[1] = (uint8_t)0x74;
  spiAcquireBus(&SPID1);              // Acquire ownership of the bus.
  spiSelect(&SPID1);                  // Slave Select assertion.
  spiExchange(&SPID1, 2, txbuf2, rxbuf2);
  spiUnselect(&SPID1);                // Slave Select de-assertion.
  txbuf2[0] = (uint8_t)0xad;
  txbuf2[1] = (uint8_t)0x17;
  txbuf2[2] = (uint8_t)0x28;
  spiSelect(&SPID1);                  // Slave Select assertion.
  spiExchange(&SPID1, 3, txbuf2, rxbuf2);
  spiUnselect(&SPID1);                // Slave Select de-assertion.
  spiReleaseBus(&SPID1);
}

extern uint16_t spiReadFrame()
{
  uint16_u pos;
  ichausmu_enc.readPos(&pos);

  uint16_t val = pos.word;
  return val;
}

extern void spiCalibFrame()
{
//  encoder.changeSdadMode(0,18,3);
//  encoder.writeIcMuRegister(CIBM, 0x08);    // Calibration courrant
//  encoder.writeIcMuRegister(LIN, 0x00);  //Encodeur rotatif mono tour
//  encoder.writeIcMuRegister(MPC, 0x05);  // Cible 32 pôles
}

extern void ChangeCS(bool state)
{
  if(state)
    spiSelect(&SPID1);
  else
    spiUnselect(&SPID1);
 }

extern void TransferIcMU(unsigned char* const txBuffer, unsigned char* rxBuffer, const unsigned int bufferLen)
{
  spiExchange(&SPID1, bufferLen, txBuffer, rxBuffer);
}
