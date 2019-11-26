#ifndef _COMMON_H_INCLUDED_
#define _COMMON_H_INCLUDED_

#include <stdint.h>
#include "rvutil.h"

#define SIMON_64_128_BLOCK_SIZE 8
#define SIMON_ID 0x53494d4f4e313238

#define TEST_DATA_SIZE 1024

extern const uint8_t key[];
extern const uint8_t iv[];
extern uint8_t testData[];
extern int err;

typedef struct test_result_s {
  uint64_t cycles;
  uint64_t instret;
} TestResult;

void test_start(TestResult* data);
void test_end(TestResult* data);

#endif
