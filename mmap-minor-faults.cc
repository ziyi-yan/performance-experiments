#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <iostream>
#include <sstream>

#include <glog/logging.h>

void load_memory(char *data, uint64_t capacity) {
  for (uint64_t i = 0; i < capacity; i++) {
    char c = i;
    data[i] = c;
  }
}

void check_memory(char *data, uint64_t capacity) {
  for (uint64_t i = 0; i < capacity; i++) {
    char c = i;
    CHECK_EQ(data[i], c);
  }
}

void *mmap_memory(uint64_t capacity) {
  int flags = MAP_NORESERVE | MAP_ANON | MAP_PRIVATE;
  return mmap(nullptr, capacity, PROT_READ | PROT_WRITE, flags, -1, 0);
}

int main() {
  // uint64_t capacity = 1024 * 1024 * 1024;  // 1GiB
  uint64_t capacity = 1000 *
                      1024 * 1024;  // 1MiB

  auto mmap_data = reinterpret_cast<char *>(mmap_memory(capacity));

  auto malloc_data = reinterpret_cast<char *>(malloc(capacity));

  load_memory(mmap_data, capacity);
  load_memory(malloc_data, capacity);

  std::stringstream parent_pid;
  parent_pid << getpid();

  pid_t perf_pid;
  pid_t pid = fork();
  if (pid == 0) {
    exit(execl(
          "/usr/bin/perf", "perf", "stat", "-B", "-e",
          "cache-references,cache-misses,cycles,instructions,branches,faults,major-faults,minor-faults",
          "-p", parent_pid.str().c_str(), nullptr));
  } else {
    perf_pid = pid;
  }

  int repeat = 5;
  while (repeat-->0) {
    check_memory(mmap_data, capacity);
    // check_memory(malloc_data, capacity);
  }

  std::cerr << "stop perf..." << std::endl;
  kill(perf_pid, SIGINT);
  waitpid(perf_pid, nullptr, 0);
}
