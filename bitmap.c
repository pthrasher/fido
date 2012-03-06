// This program is free software. It comes without any warranty, to the extent
// permitted by applicable law. You can redistribute it and/or modify it under
// the terms of the Do What The Fuck You Want To Public License, Version 2, as
// published by Sam Hocevar. See http://sam.zoy.org/wtfpl/COPYING for more
// details.

#include "bitmap.h"

void bit_array_init(bitmap_t *array) {
  array->num_bits = 0;
  array->num_slots = 0;
  array->slots = NULL;
}

void bit_array_destroy(bitmap_t *array) {
  free(array->slots);
}

// Find the number of slots needed to store @num_bits.
static inline bitmapnum_t num_slots_for(const bit_array_num_t num_bits) {
  return num_bits / sizeof(bitmapnum_t);
}

// Get the slot where the @nth_bit is stored.
static inline bitmapnum_t slot_for(const bit_array_idx_t nth_bit) {
  return nth_bit / BITS_PER_SLOT;
}

// Return an initial bit (00000001) cast to bitmapnum_t.
static inline bitmapnum_t initial_bit(void) {
  return 1;
}

// Get the bit mask needed to set the @nth_bit to true.
static inline bitmapnum_t bit_mask_for(const bit_array_idx_t nth_bit) {
  return initial_bit() << (nth_bit % BITS_PER_SLOT);
}

// Allocate the storage for the slots in an array.
static inline bitmapnum_t *allocate_slots_for(bitmap_t *array) {
  return (bitmapnum_t *)realloc(array->slots,
                                     array->num_slots * sizeof(bitmapnum_t));
}

bool bit_array_set_num_bits(bitmap_t *array, const bitmapnum_t num_bits)
{
  array->num_bits = num_bits;
  array->num_slots = num_slots_for(num_bits);
  array->slots = allocate_slots_for(array);

  return array->slots != NULL;
}

void bit_array_set_true(bitmap_t *array, const bitmapnum_t nth_bit) {
  array->slots[slot_for(nth_bit)] |= bit_mask_for(nth_bit);
}

void bit_array_set_all_true(bitmap_t *array) {
  memset(array->slots, ALL_TRUE_BIT_MASK, array->num_slots);
}

void bit_array_set_false(bitmap_t *array, const bitmapnum_t nth_bit) {
  array->slots[slot_for(nth_bit)] &= ~bit_mask_for(nth_bit);
}

void bit_array_set_all_false(bitmap_t *array) {
  memset(array->slots, ALL_FALSE_BIT_MASK, array->num_slots);
}

void bitmap_toggle(bitmap_t *array, const bitmapnum_t nth_bit) {
  array->slots[slot_for(nth_bit)] ^= bit_mask_for(nth_bit);
}

void bitmap_toggle_all(bitmap_t *array) {
  bitmapnum_t i;
  for (i = 0; i < array->num_slots; i += 1)
    array->slots[i] ^= ALL_TRUE_BIT_MASK;
}

bool bitmap_test(const bitmap_t *array, const bitmapnum_t nth_bit) {
  return array->slots[slot_for(nth_bit)] & bit_mask_for(nth_bit);
}

// Find the minimum of two numbers.
static inline bitmapnum_t min(const bit_array_num_t a,
                                  const bitmapnum_t b)
{
  return a < b ? a : b;
}

