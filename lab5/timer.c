#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8245.h"

// Global Variables
static int hook_id;
int counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  if (freq < 19 || freq > TIMER_FREQ)
    return 1;

  uint8_t st = 0;
  
  if (timer_get_conf(timer, &st))
    return 2;

  uint8_t ctrl_word = st & 0x0f;
  ctrl_word |= TIMER_LSB_MSB;

  if (timer == 0)
    ctrl_word |= TIMER_SEL0;
  else if (timer == 1)
    ctrl_word |= TIMER_SEL1;
  else if (timer == 2)
    ctrl_word |= TIMER_SEL2;
  else
    return 3;

  uint16_t val = TIMER_FREQ / freq;

  uint8_t msb, lsb;

  //if (util_get_LSB(val, &lsb) || util_get_MSB(val, &msb))
  //  return 3;
  util_get_LSB(val, &lsb);
  util_get_MSB(val, &msb);

  //int port = TIMER_0 + timer;

  return (sys_outb(TIMER_CTRL, ctrl_word) || sys_outb(TIMER_0 + timer, lsb) || sys_outb(TIMER_0 + timer, msb));
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  hook_id = *bit_no = TIMER0_IRQ;
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id);
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  uint8_t rb_command = (TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_);

  int port = TIMER_0 + timer;

  if (sys_outb(TIMER_CTRL, rb_command))
    return 1;
  
  if (util_sys_inb(port, st))
    return 2;

  return 0;
}

enum timer_init timer_get_in_mode(uint8_t st) {

  //defines the init mode
  switch (st & TIMER_LSB_MSB) {
    case TIMER_LSB:
      return LSB_only;
    case TIMER_MSB:
      return MSB_only;
    case TIMER_LSB_MSB:
      return MSB_after_LSB;
    default:
      return INVAL_val;
  }

}

uint8_t timer_get_count_mode(uint8_t st) {
  st = (st & (BIT(1) | BIT(2) | BIT(3))) >> 1;

  if (st > 5)
    st &= ~BIT(2);

  return st;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {

  union timer_status_field_val aux;

  switch (field) {
    case tsf_all:
      aux.byte = st;
      break;
    case tsf_initial:
      aux.in_mode = timer_get_in_mode(st);
      break;
    case tsf_mode:
      aux.count_mode = timer_get_count_mode(st); 
      break;
    case tsf_base:
      aux.bcd = st & TIMER_BCD;
      break;
  }
  
  return timer_print_config(timer, field, aux);
}
