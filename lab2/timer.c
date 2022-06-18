#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

static int hook_id = 0;
int counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  if (freq < 19 || freq > TIMER_FREQ)
    return 1;

  uint8_t st = 0;

  if (timer_get_conf(timer, &st))
    return 2;

  uint8_t cmd = (st & TIMER_DONT_CHANGE) | TIMER_LSB_MSB | TIMER_SEL(timer);

  uint16_t div = TIMER_FREQ / freq;

  uint8_t msb, lsb;

  if (util_get_LSB(div, &lsb))
    return 2;

  if (util_get_MSB(div, &msb))
    return 3;

  return (sys_outb(TIMER_CTRL, cmd) || sys_outb(TIMER_0 + timer, lsb) || sys_outb(TIMER_0 + timer, msb));
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  hook_id = *bit_no = TIMER0_IRQ;
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id);
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

void (timer_int_handler)() {
  ++counter;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  uint8_t read_back_cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  
  if (sys_outb(TIMER_CTRL, read_back_cmd))
    return 1;

  util_sys_inb(TIMER_0 + timer, st);

  return 0;
}

enum timer_init get_init_mode(uint8_t st) {
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

uint8_t get_count_mode(uint8_t st) {
  st &= TIMER_OPERATING_MODE >> 1;
  st = (st > 5) ? st & ~BIT(3) : st;

  return st;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {

  union timer_status_field_val val;

  switch (field) {
    case tsf_all:
      val.byte = st;
      break;
    case tsf_initial:
      val.in_mode = get_init_mode(st);
      break;
    case tsf_mode:
      val.count_mode = get_count_mode(st);
      break;
    case tsf_base:
      val.bcd = st & TIMER_BCD;
      break;
    
    default:
      return 1;
  }

  return timer_print_config(timer, field, val);
}
