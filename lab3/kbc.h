#ifndef __KBC_H
#define __KBC_H

#include <lcom/lcf.h>

#include "i8042.h"

#define DELAY_US    20000

int (kbc_subscribe_int)(uint8_t *bit_no);

int (kbc_unsubscribe_int)();

int (kbc_get_status)(uint8_t *st);

int (kbc_enable_interrupt)();

bool kbc_can_read(uint8_t *st);

int (kbc_read)(uint8_t *data);

int (kbc_write_cmd)(int port, uint8_t cmd);

void (kbc_ih)();

#endif /* __KBC_H */
