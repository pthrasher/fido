#include "fido.h"

bitmap_t* newBitmap(bitmapnum_t size) {
  bitmap_t* bits;
  bitmapInit(bits);
  bitmapSetNumBits(bits, size);
  bitmapSetAllFalse(bits);
  return bits;
}
void bitmapInit(bitmap_t *array) {
  array->numBits = 0;
  array->numSlots = 0;
  array->slots = NULL;
}

void bitmapDestroy(bitmap_t *array) {
  free(array->slots);
}

// Find the number of slots needed to store @num_bits.
static inline bitmapnum_t numSlotsFor(const bitmapnum_t numBits) {
  return numBits / sizeof(bitmapnum_t);
}

// Get the slot where the @nth_bit is stored.
static inline bitmapnum_t slotFor(const bitmapnum_t nthBit) {
  return nthBit / BITS_PER_SLOT;
}

// Return an initial bit (00000001) cast to bitmapnum_t.
static inline bitmapnum_t initialBit(void) {
  return 1;
}

// Get the bit mask needed to set the @nth_bit to true.
static inline bitmapnum_t bitMaskFor(const bitmapnum_t nthBit) {
  return initialBit() << (nthBit % BITS_PER_SLOT);
}

// Allocate the storage for the slots in an array.
static inline bitmapnum_t *allocateSlotsFor(bitmap_t *array) {
  return (bitmapnum_t *)realloc(array->slots,
                                     array->numSlots * sizeof(bitmapnum_t));
}

bool bitmapSetNumBits(bitmap_t *array, const bitmapnum_t numBits) {
  array->numBits = numBits;
  array->numSlots = numSlotsFor(numBits);
  array->slots = allocateSlotsFor(array);

  return array->slots != NULL;
}

void bitmapSetTrue(bitmap_t *array, const bitmapnum_t nthBit) {
  array->slots[slotFor(nthBit)] |= bitMaskFor(nthBit);
}

void bitmapSetAllTrue(bitmap_t *array) {
  memset64(array->slots, ALL_TRUE_BIT_MASK, array->numSlots);
}

void bitmapSetFalse(bitmap_t *array, const bitmapnum_t nthBit) {
  array->slots[slotFor(nthBit)] &= ~bitMaskFor(nthBit);
}

void bitmapSetAllFalse(bitmap_t *array) {
  memset(array->slots, ALL_FALSE_BIT_MASK, array->numSlots);
}

void bitmapToggle(bitmap_t *array, const bitmapnum_t nthBit) {
  array->slots[slotFor(nthBit)] ^= bitMaskFor(nthBit);
}

void bitmapToggleAll(bitmap_t *array) {
  bitmapnum_t i;
  for (i = 0; i < array->numSlots; i += 1)
    array->slots[i] ^= ALL_TRUE_BIT_MASK;
}

bool bitmapTest(const bitmap_t *array, const bitmapnum_t nthBit) {
  return array->slots[slotFor(nthBit)] & bitMaskFor(nthBit);
}

// Find the minimum of two numbers.
static inline bitmapnum_t min(const bitmapnum_t a, const bitmapnum_t b) {
  return a < b ? a : b;
}

void memset64(void * dest, uint64_t value, uintptr_t size) {
  uintptr_t i;
  for(i = 0; i < (size & (~7)); i+=8)
  {
    memcpy(((char*)dest) + i, &value, 8);
  }
  for( ; i < size; i++ )
  {
    ((char*)dest)[i] = ((char*)&value)[i&7];
  }
}
