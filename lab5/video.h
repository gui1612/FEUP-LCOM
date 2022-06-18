#ifndef __VIDEO_H
#define __VIDEO_H

#include <lcom/lcf.h>

int (set_mode)(uint16_t mode);

//int (vbe_get_mode_info)(uint16_t mode, vbe_mode_info_t *vm);

int (mem_init)(uint16_t mode);

void* (vg_init)(uint16_t mode);

//int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);

int (vg_draw_matrix)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);

uint8_t (R)(uint32_t color);

uint8_t (G)(uint32_t color);

uint8_t (B)(uint32_t color);

uint8_t (get_indexed_color)(uint row, uint col, uint32_t first, uint8_t no_rectangles, uint8_t step);

uint32_t (get_direct_color)(uint row, uint col, uint32_t first, uint8_t no_rectangles, uint8_t step);

uint32_t (build_RGB)(uint8_t r, uint8_t g, uint8_t b);

#endif /* __VIDEO_H */
