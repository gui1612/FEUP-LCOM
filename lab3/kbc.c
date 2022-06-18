#include <minix/sysutil.h>

#include "kbc.h"


// tickdelay(micros_to_ticks(DELAY_US))

static int hook_id = 0;
uint8_t scancode = 0;

int (kbc_subscribe_int)(uint8_t *bit_no) {
  hook_id = *bit_no = KBC_IRQ;
  return sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int (kbc_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

int (kbc_get_status)(uint8_t *st) {
  return util_sys_inb(KBC_ST_REG, st);
}

int (kbc_enable_interrupt)() {
  uint8_t cmd_b = 0;

  kbc_write_cmd(KBC_IN_CMD, R_CMD_B);
  util_sys_inb(KBC_OUT_BUF, &cmd_b);
  cmd_b |= INT;
  kbc_write_cmd(KBC_IN_CMD, W_CMD_B);
  kbc_write_cmd(KBC_IN_ARG, cmd_b);

  return 0;
}

bool kbc_can_read(uint8_t *st) {
  return !(*st & KBC_ERROR_SIG & KBC_AUX);
}


int (kbc_read)(uint8_t *data) {
  uint8_t st;

  if (kbc_get_status(&st))
      return 1;

  if ( st & KBC_OBF ) {

    if (!kbc_can_read(&st))
      return 2;

    if (util_sys_inb(KBC_OUT_BUF, data))
      return 3;

    return 0;
  }

  return 4;
}

int (kbc_write_cmd)(int port, uint8_t cmd) {
  uint8_t st = 0, curr_time = 0;
  const uint8_t TIMEOUT = 10;

  while (curr_time < TIMEOUT) {
    if (kbc_get_status(&st))
      return 1;

    if ( (st & KBC_IBF) == 0)
      return sys_outb(KBC_IN_CMD, cmd);

    tickdelay(micros_to_ticks(DELAY_US));
    curr_time++;
  }

  return 2;
}

void (kbc_ih)() {
  int err;
  if ( (err = kbc_read(&scancode)) != 0)
    printf("There was a problem while reading the scancode. Error code: %d\n", err);
}
