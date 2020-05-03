/*
 * shellcfg.h
 *
 *  Created on: Apr 4, 2020
 *      Author: Luczia
 */

#ifndef SOURCE_SHELL_CONF_H_
#define SOURCE_SHELL_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif


#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)
#define CT 24

extern const ShellCommand commands[];
extern const ShellConfig shell_cfg1;

void cmd_write(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_led(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_spi(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_ispi(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_bspi(BaseSequentialStream *chp, int argc, char *argv[]);


#ifdef __cplusplus
}
#endif



#endif /* SOURCE_SHELL_CONF_H_ */
