#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <klib.h>

#define INITIAL_WIDTH 400
#define INITIAL_HEIIGHT 300
size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;

      info->width = INITIAL_WIDTH;
      info->height = INITIAL_HEIIGHT;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;

      int x = ctl->x,
          y = ctl->y,
          w = ctl->w,
          h = ctl->h;

      uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR; 
      uint32_t* pixels = ctl->pixels;
      int copy_size = sizeof(uint32_t) * ( INITIAL_WIDTH - x < w ? INITIAL_WIDTH-x : w);
      for(int i = 0; i < h && y + i < INITIAL_HEIIGHT; ++ i) {
        memcpy(&fb[(i + y) * INITIAL_WIDTH + x], pixels + i * w, copy_size);
      }
      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
  int i;
  int size = screen_width() * screen_height();
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i ++) fb[i] = i;
  draw_sync();
}
