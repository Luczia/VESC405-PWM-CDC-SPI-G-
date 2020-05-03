/*
 * shellcfg.c
 *
 *  Created on: Apr 4, 2020
 *      Author: Luczia
 */
#include "ch.h"
#include "hal.h"

#include "shell.h"
#include "chprintf.h"
#include "spi_conf.h"
//#include <chstreams.h>

//#include "encoder.h"
#include "usb_otg/usbcfg.h"

#define LED_ORANGE PAL_LINE(GPIOC,0)
#define TEST BUS_SIZE

/*===========================================================================*/
/* Command line/shell related.                                                     */
/*===========================================================================*/


/* Can be measured using dd if=/dev/xxxx of=/dev/null bs=512 count=10000.*/
void cmd_write(BaseSequentialStream *chp, int argc, char *argv[]) {
  static uint8_t buf[] =
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
      "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: write\r\n");
    return;
  }

  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT) {
#if 1
    /* Writing in channel mode.*/
    chnWrite(&SDU1, buf, sizeof buf - 1);
#else
    /* Writing in buffer mode.*/
    (void) obqGetEmptyBufferTimeout(&SDU1.obqueue, TIME_INFINITE);
    memcpy(SDU1.obqueue.ptr, buf, SERIAL_USB_BUFFERS_SIZE);
    obqPostFullBuffer(&SDU1.obqueue, SERIAL_USB_BUFFERS_SIZE);
#endif
  }
  chprintf(chp, "\r\n\nstopped\r\n");
}

void cmd_led(BaseSequentialStream *chp, int argc, char *argv[])
{
  (void)argv;
   if (argc > 0) {
     chprintf(chp, "Usage: led\r\n Will Toggle the external Orange LED \r\n");
     return;
   }
   palToggleLine(LED_ORANGE);
   chprintf((BaseSequentialStream*)&SDU1, "D: LED Switched \r\n");
}


void cmd_spi(BaseSequentialStream *chp, int argc, char *argv[])
{
  (void)argv;
   if (argc > 0) {
     chprintf(chp, "Usage: spi\r\n Will Send a SPI frame with icmu lib \r\n");
     return;
   }
      uint16_t pos;
      uint8_t status = spiReadFrame(&pos);
      chprintf((BaseSequentialStream*)&SDU1, "D: SPIread : %d - %d \r\n", pos, status);

}

void cmd_ispi(BaseSequentialStream *chp, int argc, char *argv[])
{
  (void)argv;
   if (argc > 0) {
     chprintf(chp, "Usage: ispi\r\n Will Send an init frame with icmu lib \r\n");
     return;
   }
   uint8_t init_seq[6] ={10,10,10,10,10,10};

   spiInitFrame(&init_seq);


   chprintf((BaseSequentialStream*)&SDU1, "D: SPIinit : %d - %d - %d - %d - %d - %d  \r\n", init_seq[0],init_seq[1],init_seq[2],init_seq[3],init_seq[4],init_seq[5]);


}

void cmd_bspi(BaseSequentialStream *chp, int argc, char *argv[])
{
  (void)argv;
   if (argc > 0) {
     chprintf(chp, "Usage: bspi\r\n Will Send an init frame \r\n");
     return;
   }
   basicSpiInitFrame();
   //chThdResume(&spi_thread_ref, MSG_OK);

   uint8_t msg[] = "\r\nD: Init :";
   chprintf((BaseSequentialStream*)&SDU1, "D: SPIBaseinit \r\n");


}


const ShellCommand commands[] = {
  {"write", cmd_write},
  {"led", cmd_led},
  {"spi", cmd_spi},
  {"ispi", cmd_ispi},
  {"bspi", cmd_bspi},
  {NULL, NULL}
};

const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SDU1,
  commands
};
