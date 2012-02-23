// This program is free software. It comes without any warranty, to the extent
// permitted by applicable law. You can redistribute it and/or modify it under
// the terms of the Do What The Fuck You Want To Public License, Version 2, as
// published by Sam Hocevar. See http://sam.zoy.org/wtfpl/COPYING for more
// details.

#define __STDC_LIMIT_MACROS

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bit-array.h"

// Each slot holds this amount of bits.
enum { BITS_PER_SLOT = CHAR_BIT * sizeof(bit_array_slot_t) };

// The bit mask used to set all bits in a slot to true.
enum { ALL_TRUE_BIT_MASK = UINTMAX_MAX };

// The bit mask used to set all bits in a slot to false.
enum { ALL_FALSE_BIT_MASK = 0 };

void bit_array_init(bit_array_t *array) {
  array->num_bits = 0;
  array->num_slots = 0;
  array->slots = NULL;
}

void bit_array_destroy(bit_array_t *array) {
  free(array->slots);
}

// Find the number of slots needed to store @num_bits.
static inline bit_array_num_t num_slots_for(const bit_array_num_t num_bits) {
  return num_bits / sizeof(bit_array_slot_t);
}

// Get the slot where the @nth_bit is stored.
static inline bit_array_idx_t slot_for(const bit_array_idx_t nth_bit) {
  return nth_bit / BITS_PER_SLOT;
}

// Return an initial bit (00000001) cast to bit_array_slot_t.
static inline bit_array_slot_t initial_bit(void) {
  return 1;
}

// Get the bit mask needed to set the @nth_bit to true.
static inline bit_array_slot_t bit_mask_for(const bit_array_idx_t nth_bit) {
  return initial_bit() << (nth_bit % BITS_PER_SLOT);
}

// Allocate the storage for the slots in an array.
static inline bit_array_slot_t *allocate_slots_for(bit_array_t *array) {
  return (bit_array_slot_t *)realloc(array->slots,
                                     array->num_slots * sizeof(bit_array_slot_t));
}

bool bit_array_set_num_bits(bit_array_t *array, const bit_array_num_t num_bits)
{
  array->num_bits = num_bits;
  array->num_slots = num_slots_for(num_bits);
  array->slots = allocate_slots_for(array);

  return array->slots != NULL;
}

void bit_array_set_true(bit_array_t *array, const bit_array_idx_t nth_bit) {
  array->slots[slot_for(nth_bit)] |= bit_mask_for(nth_bit);
}

void bit_array_set_all_true(bit_array_t *array) {
  memset(array->slots, ALL_TRUE_BIT_MASK, array->num_slots);
}

void bit_array_set_false(bit_array_t *array, const bit_array_idx_t nth_bit) {
  array->slots[slot_for(nth_bit)] &= ~bit_mask_for(nth_bit);
}

void bit_array_set_all_false(bit_array_t *array) {
  memset(array->slots, ALL_FALSE_BIT_MASK, array->num_slots);
}

void bit_array_toggle(bit_array_t *array, const bit_array_idx_t nth_bit) {
  array->slots[slot_for(nth_bit)] ^= bit_mask_for(nth_bit);
}

void bit_array_toggle_all(bit_array_t *array) {
  bit_array_idx_t i;
  for (i = 0; i < array->num_slots; i += 1)
    array->slots[i] ^= ALL_TRUE_BIT_MASK;
}

bool bit_array_test(const bit_array_t *array, const bit_array_idx_t nth_bit) {
  return array->slots[slot_for(nth_bit)] & bit_mask_for(nth_bit);
}

bool bit_array_test_all_true(const bit_array_t *array) {
  bit_array_idx_t i;
  for (i = 0; i < array->num_slots; i += 1)
    if (array->slots[i] != ALL_TRUE_BIT_MASK)
      return false;

  return true;
}

bool bit_array_test_all_false(const bit_array_t *array) {
  bit_array_idx_t i;
  for (i = 0; i < array->num_slots; i += 1)
    if (array->slots[i] != ALL_FALSE_BIT_MASK)
      return false;

  return true;
}

bool bit_array_test_any_true(const bit_array_t *array) {
  return !bit_array_test_all_false(array);
}

bool bit_array_test_any_false(const bit_array_t *array) {
  return !bit_array_test_all_true(array);
}

// Find the minimum of two numbers.
static inline bit_array_num_t min(const bit_array_num_t a,
                                  const bit_array_num_t b)
{
  return a < b ? a : b;
}

size_t bit_array_to_string(const bit_array_t *array, char *buffer,
                           const size_t buffer_size)
{
  static const char TRUE_BIT = '1';
  static const char FALSE_BIT = '0';

  const bit_array_idx_t end_bit = min(buffer_size - 1, array->num_bits);
  bit_array_idx_t bit = 0;

  while (bit < end_bit) {
    if (bit_array_test(array, bit))
      buffer[bit] = TRUE_BIT;
    else
      buffer[bit] = FALSE_BIT;

    bit += 1;
  }

  buffer[bit] = '\0';

  // Return the number of bits not copied.
  return array->num_bits - end_bit;
}
