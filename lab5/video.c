#include <minix/sysutil.h>
#include "video.h"

static vbe_mode_info_t vbe_info;
static char* video_mem;
static unsigned h_res;
static unsigned v_res;
static unsigned bytes_per_pixel;


void* (vg_init)(uint16_t mode) {
  if (mem_init(mode))
    return NULL;
  
  set_mode(mode);

  return video_mem;
}

int (set_mode)(uint16_t mode) {
  reg86_t r86;
  memset(&r86, 0, sizeof(r86));

  r86.intno = 0x10;
  r86.ah = 0x4f;
  r86.al = 0x02;
  r86.bx = mode | BIT(14);

  if (sys_int86(&r86) != OK) {
    printf("\tvg_init(): sys_int86() failed \n");
    return -1;
  }

  return (r86.ah != 0);
}

int (mem_init)(uint16_t mode) {
  struct minix_mem_range mr;
  uint vram_base;
  uint vram_size;

  int r;

  if (vbe_get_mode_info(mode, &vbe_info))
    return 1;

  vram_base = vbe_info.PhysBasePtr;
  h_res = vbe_info.XResolution;
  v_res = vbe_info.YResolution;
  bytes_per_pixel = (vbe_info.BitsPerPixel + 7) / 8;
  vram_size = h_res * v_res * bytes_per_pixel;

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if ( (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)) != OK)
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  video_mem = vm_map_phys(SELF, (void*)mr.mr_base, vram_size);

  if (video_mem == MAP_FAILED)
    panic("couldn't map video memory");

  return 0;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
  if (x > h_res || x < 0 || y > v_res || y < 0)
    return 1;
  
  char *mem = video_mem;
  unsigned long starting_idx = (y * h_res + x) * bytes_per_pixel;
  memcpy(mem + starting_idx, &color, bytes_per_pixel);

  return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (int i = 0; i < len; ++i)
    if (vg_draw_pixel(x + i, y, color))
      return 1;
  
  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (uint16_t i = 0; i < height; ++i)
      vg_draw_hline(x, y + i, width, color);

    return 0;
}


int (vg_draw_matrix)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  uint width = h_res / no_rectangles, height = v_res / no_rectangles;
  uint width_rest = h_res % no_rectangles, height_rest = v_res % no_rectangles;
  uint x, y;
  bool is_indexed = (mode == 0x105);

  for (y = 0; y < no_rectangles; ++y) {
    for (x = 0; x < no_rectangles; ++x) {
      uint32_t color = is_indexed ? get_indexed_color(y, x,first,no_rectangles,step) : get_direct_color(y, x,first,no_rectangles,step);
      vg_draw_rectangle(x * width, y * height, width, height, color);
    }
    vg_draw_rectangle(x * width, y * height, width_rest, height_rest, 0x00);
  }
  vg_draw_rectangle(0, y * height, vbe_info.XResolution, height_rest, 0x00);

  return 0;
}

uint8_t (R)(uint32_t color) {
  return (color << 8) >> 24;
}

uint8_t (G)(uint32_t color) {
  return (color << 16) >> 24;
}

uint8_t (B)(uint32_t color) {
  return (color << 24) >> 24;
}

uint32_t (build_RGB)(uint8_t r, uint8_t g, uint8_t b) {
  return ((r << 16) | (g << 8) | b);
}

uint8_t (get_indexed_color)(uint row, uint col, uint32_t first, uint8_t no_rectangles, uint8_t step) {
  return (first + (row * no_rectangles + col) * step) % (1 << vbe_info.BitsPerPixel);
}

uint32_t (get_direct_color)(uint row, uint col, uint32_t first, uint8_t no_rectangles, uint8_t step) {
  uint8_t r, g, b;
  r = (R(first) + col * step) % (1 << vbe_info.RedMaskSize);
  g = (G(first) + row * step) % (1 << vbe_info.GreenMaskSize);
  b = (B(first) + (col + row) * step) % (1 << vbe_info.BlueMaskSize);

  return build_RGB(r, g, b);
}
