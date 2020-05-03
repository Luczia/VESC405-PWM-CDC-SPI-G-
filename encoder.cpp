/*
 * encoder.cpp
 *
 *  Created on: Apr 24, 2020
 *      Author: Luczia
 */

#include "ch.h"
#include "hal.h"
#include "encoder.h"
#include "spi_conf.h"


IcHausMu ichausmu_enc(ICHAUSMU_HW_REV);

thread_reference_t spi_thread_ref = NULL;



static THD_WORKING_AREA(waSpiComThread, 128);
static THD_FUNCTION(SpiComThread, arg) {

   while (true)
   {
     //basicSpiComTest();

     msg_t msg = chThdSuspendS(&spi_thread_ref);
   }
 /* //Enable action every 500ms
  systime_t prev = chVTGetSystemTime(); // Current system time.
  while (true) {
    LED_on();
    prev = chThdSleepUntilWindowed(prev, prev + MS2ST(500));

    LED_off();
    prev = chThdSleepUntilWindowed(prev, prev + MS2ST(500));
  }*/
}





void initIcHausMu()
{
  ///Init Attach NSS and trasnfer functions to encoder;
  ichausmu_enc.csFunctionAttach(ChangeCS);
  ichausmu_enc.streamFunctionAttach(TransferIcMU);
  //chThdCreateStatic(waSpiComThread, sizeof(waSpiComThread), NORMALPRIO+10, SpiComThread, NULL);
}

extern void spiInitFrame(uint8_t* init_seq)
{
  init_seq[0] = (uint8_t)ichausmu_enc.init();
  init_seq[1] = (uint8_t)ichausmu_enc.writeIcMuRegister(CIBM, 0x08);       // set bias current, Determined by firstTimeSetup()
  init_seq[2] = (uint8_t)ichausmu_enc.writeIcMuRegister(LIN, 0x00);       // set the target type as rotative
  init_seq[3] = (uint8_t)ichausmu_enc.writeIcMuRegister(MPC, 0x05);       // magnetic target is a 32master/31Nonius period
  init_seq[4] = (uint8_t)ichausmu_enc.changeSdadMode(0x00, 18, 3);        // define the SDAD output
  init_seq[5] = (uint8_t)ichausmu_enc.setAutomaticGain(true);
  //return (uint8_t)init_encoder;
}



extern void basicSpiInitFrame()
{
  uint8_t txbuf[2],rxbuf[2],txbuf2[3],rxbuf2[3];
  txbuf[0] = 0x97;
  txbuf[1] = 0x74;
  txbuf2[0] = 0xad;
  txbuf2[1] = 0x00;
  txbuf2[2] = 0x00;

 // spiAcquireBus(&SPID1);              // Acquire ownership of the bus.
  spiSelect(&SPID1);                  // Slave Select assertion.
  //palClearPad(PORT_SPI1_CS, PIN_SPI1_CS);
  spiExchange(&SPID1, 2, &txbuf, &rxbuf);
  //palSetPad(PORT_SPI1_CS, PIN_SPI1_CS);
  spiUnselect(&SPID1);
  gptPolledDelay(&GPTD3, 2); /* Small delay.*/ //1 doesn't work
  //chThdSleepMicroseconds(2);
  spiSelect(&SPID1);
  //palClearPad(PORT_SPI1_CS, PIN_SPI1_CS);
  spiExchange(&SPID1, 3, &txbuf2, &rxbuf2);
  spiUnselect(&SPID1);                // Slave Select de-assertion.
  //palSetPad(PORT_SPI1_CS, PIN_SPI1_CS);
 // spiReleaseBus(&SPID1);
}


//Just a function to ensure SPI Send and Receive works. Should be used when shunting MISO and MOSI
extern uint8_t test_spi(uint16_t *val)
{
  uint16_u pos;
  uint8_t txbuf3[2],rxbuf3[2];
  txbuf3[0] = '1';
  txbuf3[1] = '2';
  rxbuf3[0] = '8';
  rxbuf3[1] = '8';
  uint8_t status;
  spiSelect(&SPID1);
  spiExchange(&SPID1, 2, &txbuf3, &rxbuf3);
  spiUnselect(&SPID1);
  status = 4;
  *val = (uint16_t)rxbuf3[0];//pos.word;
  return rxbuf3[0]; //(uint8_t)read_encoder;
}

//This function has to translate unint16_u from C++ ichaus library to uint16_t from HAL library as well as status uint0_t
extern uint8_t spiReadFrame(uint16_t *val)
{
  uint16_u pos;
  uint8_t status;
  status = (uint8_t)ichausmu_enc.readPos(&pos);

  *val = (uint16_t)pos.word;
  return status; //(uint8_t)read_encoder;
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

// if using SPI_SELECT_MODE_NONE
/*  if(state)
    palClearPad(PORT_SPI1_CS, PIN_SPI1_CS);
  else
    palSetPad(PORT_SPI1_CS, PIN_SPI1_CS);
*/


 }




extern void TransferIcMU(unsigned char* const txBuffer, unsigned char* rxBuffer, const unsigned int bufferLen)
{
  spiExchange(&SPID1, bufferLen, txBuffer, rxBuffer);
}
