#include <stdlib.h>

#include "checksum.h"

#define M 65536

int checksum(char* block, size_t block_size) {
  int lower, higher;
  lower = higher = 0;
  
  for (int i = 0; i < block_size; i++) {
	  lower += block[i];
	  higher += (block_size - i) * block[i];
  }
  lower = lower % M;
  higher = higher % M;

  return lower + (higher * M);
}
