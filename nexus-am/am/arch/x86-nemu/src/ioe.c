#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  //return 0;
  return inl(RTC_PORT)-boot_time;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
   /*int i;
   for (i = 0; i < _screen.width * _screen.height; i++) {
     fb[i] = i;
   }*/
  // pixels指定的像素颜色 填充到（x，y）到（x+w，y+h)上
  int i;
  for(i=0;i<h;i++)
      memcpy(fb+(y+i)*_screen.width+x,pixels+i*w,w*4);
}

void _draw_sync() {
}

int _read_key() {
    uint32_t code = _KEY_NONE;
    if(inb(0x64) & 0x1)
        code = inl(0x60);
    //if(inb(I8042_STATUS_PORT) & I8042_STATUS_HASKEY_MASK)
    //    code = inl(I8042_DATA_PORT);
    // return _KEY_NONE;
    return code;
}
