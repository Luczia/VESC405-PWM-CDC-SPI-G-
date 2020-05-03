/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "stm32f4xx.h"

#include "ch.hpp"
#include "hal.h"

#include "rt_test_root.h"
#include "oslib_test_root.h"

#define HW_412 TRUE
//#define DISCO_407 TRUE
#ifdef DISCO_407
//#define BLUE_LED LINE_LED6 //=> Manually activated LED
#define RED_ORANGE LINE_LED5   // => Shell active
#define LED_GREEN LINE_LED4 // => HeartBeat
#define LED_RED LINE_LED3 // => OTG_USB_Com active
#elif HW_412
#include "hw_410.h"
#define LED_GREEN PAL_LINE(GPIOC, 4) //LINE_LED4//
#define LED_RED PAL_LINE(GPIOC, 5) //LINE_LED5 //
#define LED_ORANGE PAL_LINE(GPIOC,0)
#endif

#include "shell.h"
#include "chprintf.h"

#include "usb_otg/usbcfg.h"
//#include "comm_usb_serial.h"
//#include "comm_usb.h"

using namespace chibios_rt;

//#include "test.h"
#include "ichausmu/IcHausMu.hpp"
#include "encoder.h"

#include "lib.h"

#include "spi_conf.h"
#include "pwm_conf.h"
#include "shell_conf.h"

#include "STM32F4xx_StdPeriph_Driver/stm32f4_gpio_af.h"
#include "STM32F4xx_StdPeriph_Driver/stm32f4xx_rcc.h"


/*===========================================================================*/
/* Shell Handler thread to spawn a shell                                                             */
/*===========================================================================*/

static THD_WORKING_AREA(waThread2, 2048);
static THD_FUNCTION(Thread2, arg) {
  (void)arg;
  chRegSetThreadName("shell_handler");

  while (true){
    if (SDU1.config->usbp->state == USB_ACTIVE) {
                //palSetLine(LED_RED);
                thread_t *shelltp = chThdCreateFromHeap(NULL, SHELL_WA_SIZE,
                                                        "shell", NORMALPRIO + 1,
                                                        shellThread, (void *)&shell_cfg1);
                chThdWait(shelltp);               // Waiting termination.
               // palClearLine(LED_RED);
    }
    chThdSleepMilliseconds(1000);
  }
}

/* =============================================================================
 * USB com status blinker thread, times are in milliseconds.
 ===============================================================================*/
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {
  (void)arg;
  chRegSetThreadName("com_status");
  while (true) {
    systime_t time = 500;
    time = serusbcfg.usbp->state == USB_ACTIVE ? 250 : 2000;
    palClearLine(LED_RED);
    chThdSleepMilliseconds(time);
    palSetLine(LED_RED);
    chThdSleepMilliseconds(time);
  }
}

/*=============================================================================
 * LED blinker heartbeat thread, times are in milliseconds.
 =============================================================================*/
static THD_WORKING_AREA(waThread3, 128);
static THD_FUNCTION(Thread3, arg) {
  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palSetLine(LED_GREEN);
    chThdSleepMilliseconds(100);
    palClearLine(LED_GREEN);
    chThdSleepMilliseconds(900);
  }
}


void hw_init_gpio(void);



/*
 * Application entry point.
 */
int main(void) {



  halInit();

  System::init();  //chSysInit();



#ifdef HW_412
  hw_init_gpio();
#endif


  //Initialize pads and AF(Alternate functions) for PWM
  //palSetPadMode(GPIOA, 8, PAL_MODE_ALTERNATE(1)); //PWM on PA8(dico) or D7(nucleo/arduino header)
  //Initialize pads and AF for Serial UART 2 */
  //palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
 // palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
  /*Initialize pads and AF for SPI */
  palSetPadMode(PORT_SPI1_SCK, PIN_SPI1_SCK,PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);    /* New SCK */
  palSetPadMode(PORT_SPI1_MISO, PIN_SPI1_MISO,PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);    /* New MISO*/
  palSetPadMode(PORT_SPI1_MOSI, PIN_SPI1_MOSI, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);    /* New MOSI*/
  palSetPadMode(PORT_SPI1_CS, PIN_SPI1_CS, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);//PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST); /* New CS*/

  //Initialize pads for VPC
  palSetPadMode(GPIOA, 11, PAL_MODE_ALTERNATE(GPIO_AF_OTG_FS) | PAL_STM32_OTYPE_PUSHPULL | PAL_STM32_OSPEED_MID1);
  palSetPadMode(GPIOA, 12, PAL_MODE_ALTERNATE(GPIO_AF_OTG_FS) | PAL_STM32_OTYPE_PUSHPULL | PAL_STM32_OSPEED_MID1);

  init_usb_cdc();

  //Start SPI
  spiStart(&SPID1, &cs_spicfg1); //Power Up the clock signal and start the driver
  //Initialize the encoder
  initIcHausMu();
  //Intialize the GPT
  gptStart(&GPTD3, &gpt3cfg);

  uint8_t txbuf3[2],rxbuf3[2];

  //Start PWM
  //pwmChangePeriod(&PWMD1, 500);
  /*always call a pwmEnableChanel after a PeriodChange because according to doc: "If a period is specified that is shorter than the pulse width
    *          programmed in one of the channels then the behavior is not;
    *          guaranteed." */
  //pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 5000));

  // Creates the usb status blinker thread.
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  // Creates the shell spawning thread.
  chThdCreateStatic(waThread2, sizeof(waThread2), LOWPRIO, Thread2, NULL);
  //Create the heartbeat thread
  chThdCreateStatic(waThread3, sizeof(waThread3), NORMALPRIO, Thread3, NULL);



  while (true) {
    /*if (palReadPad(GPIOA, GPIOA_BUTTON)) {
        fctStatus init_encoder = encoder.init();
        encoder.setAutomaticGain(true);

        sdWriteTimeout(&SD2, (uint8_t*)"Enc status:\r\n", 13, TIME_MS2I(50)); //Example to write a frame on UART 2
        sdPutTimeout(&SD2, (int8_t)init_encoder,TIME_MS2I(50)); //Example to write a single byte on UART 2
        sdPutTimeout(&SD2, (int8_t)'\n',TIME_MS2I(50));

        basicSpiComTest(); //example to call a function of a C++ library
        sdPutTimeout(&SD2, (int8_t)'t',TIME_MS2I(50));
        uint8_t msg[] = "\r\nD: SPI frame sent";
        chnWrite(&SDU1, msg, sizeof msg); //Example to send Debug Message on the VCP (Virtual Com Port)
        }*/
    ///palToggleLine(LED_ORANGE);
//    uint8_t msg[] = "\r\nD: M";
//    chnWrite(&SDU1, msg, sizeof msg); //Example to send Debug Message on the VCP (Virtual Com Port

    BaseThread::sleep(TIME_MS2I(10000)); // chThdSleepMilliseconds(200); // in C;
  }

  return 0;
}


#ifdef HW_412
void hw_init_gpio(void) {
    // GPIO clock enable
    /*RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);*/

    // LEDs
    palSetPadMode(GPIOC, 4,PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(GPIOC, 5, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
    palSetPadMode(GPIOC, 0, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

    palClearPad(GPIOC,0);


    // GPIOC (ENABLE_GATE)
    palSetPadMode(GPIOC, 10, PAL_MODE_OUTPUT_PUSHPULL |  PAL_STM32_OSPEED_HIGHEST);
    palClearPad(GPIOC, 10); //DISABLE_GATE();

    // GPIOB (DCCAL)
    palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

    // GPIOA Configuration: Channel 1 to 3 as alternate function push-pull
    palSetPadMode(GPIOA, 8, PAL_MODE_ALTERNATE(GPIO_AF_TIM1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOA, 9, PAL_MODE_ALTERNATE(GPIO_AF_TIM1) | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOA, 10, PAL_MODE_ALTERNATE(GPIO_AF_TIM1) |  PAL_STM32_OSPEED_HIGHEST |   PAL_STM32_PUPDR_FLOATING);

    palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(GPIO_AF_TIM1) | PAL_STM32_OSPEED_HIGHEST |  PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(GPIO_AF_TIM1) | PAL_STM32_OSPEED_HIGHEST |  PAL_STM32_PUPDR_FLOATING);
    palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(GPIO_AF_TIM1) | PAL_STM32_OSPEED_HIGHEST |  PAL_STM32_PUPDR_FLOATING);

    // Hall sensors
    palSetPadMode(HW_HALL_ENC_GPIO1, HW_HALL_ENC_PIN1, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(HW_HALL_ENC_GPIO2, HW_HALL_ENC_PIN2, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(HW_HALL_ENC_GPIO3, HW_HALL_ENC_PIN3, PAL_MODE_INPUT_PULLUP);

    // Fault pin
    palSetPadMode(GPIOC, 12, PAL_MODE_INPUT_PULLUP);

    // ADC Pins
    palSetPadMode(GPIOA, 0, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOA, 1, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOA, 2, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOA, 3, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOA, 4, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOA, 5, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOA, 6, PAL_MODE_INPUT_ANALOG);

    palSetPadMode(GPIOB, 0, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOB, 1, PAL_MODE_INPUT_ANALOG);

    //palSetPadMode(GPIOC, 0, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOC, 1, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOC, 2, PAL_MODE_INPUT_ANALOG);
    palSetPadMode(GPIOC, 3, PAL_MODE_INPUT_ANALOG);
}
#endif
