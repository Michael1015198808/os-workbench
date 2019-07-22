#include <devices.h>

#define NTEXTURE 16384
#define NSPRITE  16384

#define DEFAULT_BG 0x300A24
#define DEFAULT_FG 0xffffff
static sem_t fb_sem;
extern uint8_t TERM_FONT[];

static void texture_fill(struct texture *tx, int top, uint8_t *bits, uint32_t fg, uint32_t bg) {
  uint32_t *px = tx->pixels;
  tx->fg=fg;
  tx->bg=bg;
  for (int y = 0; y < TEXTURE_H; y++)
    for (int x = 0; x < TEXTURE_W; x++) {
      int bitmask = top ? bits[y + TEXTURE_H] : bits[y];
      *px++ = ((bitmask >> (7 - x)) & 1) ? fg : bg;
    }
}

static void font_load(fb_t *fb, uint8_t *font) {
  for (int ch = 0; ch < 256; ch++) {
    texture_fill(&fb->textures[ch * 2 + 1], 0, &font[16 * ch],
            DEFAULT_FG,
            DEFAULT_BG);
    texture_fill(&fb->textures[ch * 2 + 2], 1, &font[16 * ch],
            DEFAULT_FG,
            DEFAULT_BG);
  }
}

int fb_init(device_t *dev) {
  fb_t *fb = dev->ptr;
  fb->info = pmm->alloc(sizeof(struct display_info));
  fb->textures = pmm->alloc(sizeof(struct texture) * NTEXTURE);
  fb->sprites = pmm->alloc(sizeof(struct sprite) * NSPRITE);
  *(fb->info) = (struct display_info) {
    .width = screen_width(),
    .height = screen_height(),
    .num_displays = 8,
    .num_textures = NTEXTURE,
    .num_sprites = NSPRITE,
    .current = 0,
  };
  kmt->sem_init(&fb_sem, dev->name, 1);
  font_load(fb, TERM_FONT);
  return 0;
}

ssize_t fb_read(device_t *dev, off_t offset, void *buf, size_t count) {
  fb_t *fb = dev->ptr;
  if (offset != 0) return 0;
  if (count != sizeof(struct display_info)) return 0;
  memcpy(buf, fb->info, sizeof(struct display_info));
  return 0;
}

void reload_color(uint32_t* dst,const uint32_t* src,uint32_t fg,uint32_t bg){
  for (int y = 0; y < TEXTURE_H; y++)
    for (int x = 0; x < TEXTURE_W; x++) {
        if(*src==DEFAULT_FG)*dst=fg;
        else if(*src==DEFAULT_BG)*dst=bg;
        else Assert(0,"source texture contains unrecognized color!");
        ++dst;
        ++src;
    }
}

ssize_t fb_write(device_t *dev, off_t offset, const void *buf, size_t count) {
  fb_t *fb = dev->ptr;
  kmt->sem_wait(&fb_sem);
  if (offset == 0) {
    const struct display_info *info = buf;
    if (fb->info->current != info->current) {
      fb->info->current = info->current;
    }
  } else if (offset < SPRITE_BRK) {
    memcpy(((uint8_t *)fb->textures) + offset, buf, count);
  } else {
    // TODO: not remove stale sprites, and does not consider z-values
    _DEV_VIDEO_FBCTL_t ctl = {
      .w = TEXTURE_W,
      .h = TEXTURE_H,
    };
    uint32_t pixels[TEXTURE_W * TEXTURE_H];

    for (struct sprite *sp = (struct sprite *)buf; sp < (struct sprite *)(buf + count); sp++) {
      if (sp->texture > 0 && sp->display == fb->info->current) {
        ctl.x = sp->x;
        ctl.y = sp->y;
        if(sp->fg!=fb->DEFAULT_FG ||
           sp->bg!=fb->DEFAULT_BG ){

            ctl.pixels = pixels;
            reload_color(pixels,fb->textures[sp->texture].pixels,sp->fg,sp->bg);
        }else{
            ctl.pixels = fb->textures[sp->texture].pixels;
        }
        _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &ctl, sizeof(ctl));
      }
    }
  }
  kmt->sem_signal(&fb_sem);
  return count;
}

devops_t fb_ops = {
  .init = fb_init,
  .read = fb_read,
  .write = fb_write,
};
