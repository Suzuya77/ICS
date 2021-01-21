#include "common.h"
#include <amdev.h>
#include <am.h>
#include <nemu.h>

extern int screen_width();
extern int screen_height();

#define KEYDOWN_CODE 0x8000

const char *s1 = "ku";
const char *s2 = "kd";

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (size_t i = 0; i < len; ++i)
    _putc(((char *)buf)[i]);
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
    int key = read_key();
    int is_down = 0;

    if (key & KEYDOWN_CODE)
    {
      key ^= KEYDOWN_CODE;
      is_down = 1;
    }

    if (key != _KEY_NONE)
    {
      len = sprintf(buf, "%s %s\n", (is_down == 1) ? s2 : s1, keyname[key]);
    }else{
      len = sprintf(buf, "t %d\n", uptime());
    }
    // assert(len != strlen(buf));
    return len;
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
    len = sprintf(buf, dispinfo + offset);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int x = (offset / 4) % screen_width();
  int y = (offset / 4) / screen_width();
  draw_rect((void *)buf, x, y, len / 4, 1);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return 0;
}

size_t get_dispinfo_size() {
  return strlen(dispinfo);
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
