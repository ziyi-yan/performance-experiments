#include <chrono>
#include <iostream>
#include <cstring>

uint64_t MB = 1024 * 1024;
uint64_t BYTES = 1024 * MB;
uint64_t SIZE = BYTES / sizeof(uint64_t);

void do_memcpy()
{
  auto dst = (uint8_t *) std::malloc(BYTES);
  auto src = (uint8_t *) std::malloc(BYTES);

  auto starttime = std::chrono::system_clock::now();
  for (uint64_t i = 0; i < SIZE; i++) {
    std::memcpy(&dst[i * sizeof(uint64_t)], &src[i * sizeof(uint64_t)], sizeof(uint64_t));
  }
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - starttime);
  std::cout << "memcpy: " << duration.count() << " us" << std::endl;
}

void do_load_store()
{
  auto dst = (uint64_t *) std::malloc(BYTES);
  auto src = (uint64_t *) std::malloc(BYTES);

  auto starttime = std::chrono::system_clock::now();
  for (uint64_t i = 0; i < SIZE; i++) {
    dst[i] = src[i];
  }
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - starttime);
  std::cout << "load/store: " << duration.count() << " us" << std::endl;
}

int main() {
  std::cout << "Copying " << SIZE << " of 8-byte integers" << std::endl;

  do_memcpy();
  do_load_store();
}

// Output:
// Copying 134217728 of 8-byte integers
// memcpy: 493815 us
// load/store: 494753 us
