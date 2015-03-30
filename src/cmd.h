#ifndef _CMD_H_
#define _CMD_H_

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;

void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_test(BaseSequentialStream *chp, int argc, char *argv[]);

const ShellCommand commands[] = {
  {"mem", cmd_mem},
  {"threads", cmd_threads},
  {"test", cmd_test},
  {NULL, NULL}
};

const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SDU1,
  commands
};

#endif /* _CMD_H_ */
