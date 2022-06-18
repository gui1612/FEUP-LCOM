#include <lcom/lcf.h>

#include <stdint.h>

#ifdef LAB3
uint32_t cnt = 0;
#endif


int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = (uint8_t)val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = (uint8_t)(val >> 8);
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t aux;

  // This is to count the number os sys_inb calls in lab3
  #ifdef LAB3
  cnt++;
  #endif

  if (sys_inb(port, &aux))
    return 1;

  *value = (uint8_t)aux;

  return 0;
}
