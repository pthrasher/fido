/*
    Copyright 2012 Philip Thrasher
    Distributed under the MIT License (see accompanying file LICENSE
    or a copy at http://www.opensource.org/licenses/MIT
*/

#ifndef _bitmap_h
#define _bitmap_h

#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// The integer type used for each slot. Larger types allow for more bits per
// slot and slightly faster lookup times. This must coincide with the
// @ALL_TRUE_BIT_MASK variable in @bit-array.c.

// The integer type used for misc numbers.
typedef uintmax_t bitmapnum_t;

typedef struct bitmap_t {
  bitmapnum_t *slots;
  bitmapnum_t num_bits;
  bitmapnum_t num_slots;
} bitmap_t;

// Each slot holds this amount of bits.
#define BITS_PER_SLOT CHAR_BIT * sizeof(bitmapnum_t)

// The bit mask used to set all bits in a slot to true.
#define ALL_TRUE_BIT_MASK UINTMAX_MAX

// The bit mask used to set all bits in a slot to false.
#define ALL_FALSE_BIT_MASK 0

// Initialize a new bit array. This function must be called before any others.
//
//   @param array: a bit array
//
void bitArrayInit(bitmap_t *array);

// Destroy and free the memory of a bit array.
//
//   @param array: a bit array
//
void bitArrayDestroy(bitmap_t *array);

// Set the storage size of a bit array to @num_bits.
//
//   @param array: a bit array
//   @param num_bits: the number of bits to store in the array
//   @return: true on success and false otherwise
//
bool bitArraySetNumBits(bitmap_t *array, const bitmapnum_t num_bits);

// Get the number of bits stored in an array (aka, the size of it.)
//
//   @param array: a bit array.
//
static inline bitmapnum_t bitArrayNumBits(const bitmap_t *array) {
  return array->num_bits;
}

// Set the @nth_bit to true.
//
//   @param array: a bit array
//   @param nth_bit: the index of the bits to set true
//
void bitArraySetTrue(bitmap_t *array, const bitmapnum_t nth_bit);

// Set all bits to true.
//
//   @param array: a bit array
//
void bitArraySetAllTrue(bitmap_t *array);

// Set the @nth_bit to false.
//
//   @param array: a bit array
//   @param nth_bit: the index of the bit to set false
//
void bitArraySetFalse(bitmap_t *array, const bitmapnum_t nth_bit);

// Set all bits to false.
//
//   @param array: a bit array
//
void bitArraySetAllFalse(bitmap_t *array);

// Toggle the @nth_bit from true to false or vice versa.
//
//   @param array: a bit array
//   @param nth_bit: the bit to toggle
//
void bitmapToggle(bitmap_t *array, const bitmapnum_t nth_bit);

// Toggle all bits from true to false or vice versa.
//
//   @param array: a bit array
//
void bitmapToggleAll(bitmap_t *array);

// Check if the @nth_bit is true.
//
//   @param array: a bit array
//   @param nth_bit: the index of the bit to check for truthness
//   @return: true if the @nth_bit is true and false otherwise
//
bool bitmapTest(const bitmap_t *array, const bitmapnum_t nth_bit);

// Check if all bits are true.
//
//   @param array: a bit array
//
bool bitmapTestAllTrue(const bitmap_t *array);

// Check if all bits are false.
//
//   @param array: a bit array
//
bool bitmapTestAllFalse(const bitmap_t *array);

// Check if any bits are true.
//
//   @param array: a bit array
//
bool bitmapTestAnyTrue(const bitmap_t *array);

// Check if any bits are false.
//
//   @param array: a bit array
//
bool bitmapTestAnyFalse(const bitmap_t *array);

#endif
