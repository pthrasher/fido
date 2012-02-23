// This program is free software. It comes without any warranty, to the extent
// permitted by applicable law. You can redistribute it and/or modify it under
// the terms of the Do What The Fuck You Want To Public License, Version 2, as
// published by Sam Hocevar. See http://sam.zoy.org/wtfpl/COPYING for more
// details.

#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// The integer type used for each slot. Larger types allow for more bits per
// slot and slightly faster lookup times. This must coincide with the
// @ALL_TRUE_BIT_MASK variable in @bit-array.c.
typedef uintmax_t bit_array_slot_t;

// The integer type used to index individual bits.
typedef uint32_t bit_array_idx_t;

// The integer type used for misc numbers.
typedef uint32_t bit_array_num_t;

typedef struct bit_array_t {
  bit_array_slot_t *slots;

  bit_array_num_t num_bits;
  bit_array_num_t num_slots;
} bit_array_t;

// Initialize a new bit array. This function must be called before any others.
//
//   @param array: a bit array
//
void bit_array_init(bit_array_t *array);

// Destroy and free the memory of a bit array.
//
//   @param array: a bit array
//
void bit_array_destroy(bit_array_t *array);

// Set the storage size of a bit array to @num_bits.
//
//   @param array: a bit array
//   @param num_bits: the number of bits to store in the array
//   @return: true on success and false otherwise
//
bool bit_array_set_num_bits(bit_array_t *array, const bit_array_num_t num_bits);

// Get the number of bits stored in an array (aka, the size of it.)
//
//   @param array: a bit array.
//
static inline bit_array_num_t bit_array_num_bits(const bit_array_t *array) {
  return array->num_bits;
}

// Set the @nth_bit to true.
//
//   @param array: a bit array
//   @param nth_bit: the index of the bits to set true
//
void bit_array_set_true(bit_array_t *array, const bit_array_idx_t nth_bit);

// Set all bits to true.
//
//   @param array: a bit array
//
void bit_array_set_all_true(bit_array_t *array);

// Set the @nth_bit to false.
//
//   @param array: a bit array
//   @param nth_bit: the index of the bit to set false
//
void bit_array_set_false(bit_array_t *array, const bit_array_idx_t nth_bit);

// Set all bits to false.
//
//   @param array: a bit array
//
void bit_array_set_all_false(bit_array_t *array);

// Toggle the @nth_bit from true to false or vice versa.
//
//   @param array: a bit array
//   @param nth_bit: the bit to toggle
//
void bit_array_toggle(bit_array_t *array, const bit_array_idx_t nth_bit);

// Toggle all bits from true to false or vice versa.
//
//   @param array: a bit array
//
void bit_array_toggle_all(bit_array_t *array);

// Check if the @nth_bit is true.
//
//   @param array: a bit array
//   @param nth_bit: the index of the bit to check for truthness
//   @return: true if the @nth_bit is true and false otherwise
//
bool bit_array_test(const bit_array_t *array, const bit_array_idx_t nth_bit);

// Check if all bits are true.
//
//   @param array: a bit array
//
bool bit_array_test_all_true(const bit_array_t *array);

// Check if all bits are false.
//
//   @param array: a bit array
//
bool bit_array_test_all_false(const bit_array_t *array);

// Check if any bits are true.
//
//   @param array: a bit array
//
bool bit_array_test_any_true(const bit_array_t *array);

// Check if any bits are false.
//
//   @param array: a bit array
//
bool bit_array_test_any_false(const bit_array_t *array);

// Create a string representation of a bit array, limited to @max_length. The
// resulting string is always null terminated.
//
//   @param buffer: a buffer to hold the string.
//   @param buffer_size: the number of characters @buffer can hold.
//   @return: the number of bits that were not copied.
//
size_t bit_array_to_string(const bit_array_t *array, char *buffer,
                           const size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif
