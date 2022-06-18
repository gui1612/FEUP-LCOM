#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "kbc.h"


extern uint8_t scancode;
extern uint32_t cnt;
extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  int ipc_status, i = 0;
  message msg;
  uint8_t bit_no;

  if (kbc_subscribe_int(&bit_no))
    return 1;

  int irq_set = BIT(bit_no);

  uint8_t bytes[2];

  while (scancode != ESC) {
    if (driver_receive(ANY, &msg, &ipc_status))
      continue;

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & irq_set) {
          kbc_ih();
          // Checks if the scancode is long or short
          if (scancode == PREFIX_2B_SCANCODE) {
            bytes[i] = scancode;
            i++; continue;
          }

          bytes[i] = scancode;
          kbd_print_scancode(!(scancode & MAKE_BREAK_DIFF), i + 1, bytes);
          i = 0;
        }
        break;
      
      default:
        break;
      }
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }


  return (kbc_unsubscribe_int() | kbd_print_no_sysinb(cnt));
}

int(kbd_test_poll)() {
  int i = 0;
  uint8_t bytes[2];

  while (scancode != ESC) {
    if (kbc_read(&scancode)) {
      tickdelay(micros_to_ticks(DELAY_US));
      continue;
    }

    if (scancode == PREFIX_2B_SCANCODE) {
      bytes[i] = scancode;
      i++; continue;
    }

    bytes[i] = scancode;
    kbd_print_scancode(!(scancode & MAKE_BREAK_DIFF), i + 1, bytes);
    i = 0;
  }
  
  kbc_enable_interrupt();
  return kbd_print_no_sysinb(cnt);
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status;
  message msg;

  uint8_t bit_no_kbc, bit_no_timer;
  uint8_t irq_set_kbc, irq_set_timer;

  if (timer_subscribe_int(&bit_no_timer))
    return 1;

  if (kbc_subscribe_int(&bit_no_kbc))
    return 2;
  
  irq_set_timer = BIT(bit_no_timer);
  irq_set_kbc = BIT(bit_no_kbc);

  uint8_t bytes[2];
  int i = 0;

  while (scancode != ESC && (counter / 60) < n) {
    if (driver_receive(ANY, &msg, &ipc_status))
      continue;

    if is_ipc_notify(ipc_status) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & irq_set_kbc) {
          counter = 0;
          kbc_ih();
          // Checks if the scancode is long or short
          if (scancode == PREFIX_2B_SCANCODE) {
            bytes[i] = scancode;
            i++; continue;
          }

          bytes[i] = scancode;
          kbd_print_scancode(!(scancode & MAKE_BREAK_DIFF), i + 1, bytes);
          i = 0;

        } 
        
        if (msg.m_notify.interrupts & irq_set_timer) {
          timer_int_handler();
        }

        break;
      default:
        break;
      }
    }
  }

  return 1;
}
