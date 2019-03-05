#ifndef COMMON_H
#define COMMON_H

#include <am.h>

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;
typedef int bool;

#define true  1
#define false 0

// #define log(fmt, ...) printk("%s:%d: " fmt, __func__, __LINE__, ## __VA_ARGS__)
#define log(fmt, ...)

static inline uint32_t pixel(uint8_t r, uint8_t g, uint8_t b) {
  return (r << 16) | (g << 8) | b;
}
static inline uint8_t R(uint32_t p) { return p >> 16; }
static inline uint8_t G(uint32_t p) { return p >> 8; }
static inline uint8_t B(uint32_t p) { return p; }

// Byte Bit Operations
void common_set_bitb(byte *variable, byte position);
void common_unset_bitb(byte *variable, byte position);
void common_toggle_bitb(byte *variable, byte position);
void common_modify_bitb(byte *variable, byte position, bool set);

// Word Bit Operations
void common_set_bitw(word *variable, byte position);
void common_unset_bitw(word *variable, byte position);
void common_toggle_bitw(word *variable, byte position);
void common_modify_bitw(word *variable, byte position, bool set);

// Double Word Bit Operations
void common_set_bitd(dword *variable, byte position);
void common_unset_bitd(dword *variable, byte position);
void common_toggle_bitd(dword *variable, byte position);
void common_modify_bitd(dword *variable, byte position, bool set);

static inline void* memcpy(void *dest, const void *src, size_t n) {
  char *csrc = (char*)src, *cdest = (char*)dest;
  for (int i = 0; i < n; i ++) {
    cdest[i] = csrc[i];
  }
  return dest;
}

static inline int memcmp(const void *dest, const void *src, size_t n) {
  unsigned char *csrc = (unsigned char*)src, *cdest = (unsigned char*)dest;
  for (int i = 0; i < n; i ++) {
    if (csrc[i] < cdest[i]) return -1;
    if (csrc[i] > cdest[i]) return 1;
  }
  return 0;
}

static inline bool common_bit_set(unsigned long value, byte position) { return value & (1L << position); }

#endif
