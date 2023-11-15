#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <iostream>
#include <sstream>

#include <glog/logging.h>

constexpr uint64_t CACHELINE_SIZE = 64; 

void load_memory(char *data, uint64_t capacity) {
  for (uint64_t i = 0; i < capacity; i += CACHELINE_SIZE) {
    char c = i;
    data[i] = c;
  }
}

void check_memory(char *data, uint64_t capacity) {
  for (uint64_t i = 0; i < capacity; i += CACHELINE_SIZE) {
    char c = i;
    CHECK_EQ(data[i], c);
  }
}

#ifdef HUGETLB
int flags = MAP_NORESERVE | MAP_ANON | MAP_PRIVATE | MAP_HUGETLB;
#else
int flags = MAP_NORESERVE | MAP_ANON | MAP_PRIVATE;
#endif

void *mmap_memory(uint64_t capacity) {
  return mmap(nullptr, capacity, PROT_READ | PROT_WRITE, flags, -1, 0);
}


void *fine_grained_mmap_memory(uint64_t capacity) {
  auto data = reinterpret_cast<char *>(mmap(nullptr, capacity, PROT_NONE, flags, -1, 0));
  auto step = 2 * 1024 * 1024;
  for (uint64_t i = 0; i < capacity; i += step) {
    if (i + step > capacity) {
      auto ret = mprotect(data + i, capacity - i, PROT_READ | PROT_WRITE);
      CHECK_EQ(ret, 0);
    } else {
      auto ret = mprotect(data + i, step, PROT_READ | PROT_WRITE);
      CHECK_EQ(ret, 0);
    }
  }
  return data;
}

void *bulk_mmap_memory(uint64_t capacity) {
  auto data = reinterpret_cast<char *>(mmap(nullptr, capacity, PROT_NONE, flags, -1, 0));
  auto ret = mprotect(data, capacity, PROT_READ | PROT_WRITE);
  CHECK_EQ(ret, 0);
  return data;
}

int main() {
  uint64_t capacity = 16UL *
    1024 * 1024 * 1024;  // 1GiB

#ifdef MMAP
  auto mmap_data = reinterpret_cast<char *>(mmap_memory(capacity));
#endif
#ifdef MALLOC
  auto malloc_data = reinterpret_cast<char *>(malloc(capacity));
#endif
#ifdef FINE_GRAINED
  auto fine_grained_mmap_data = reinterpret_cast<char *>(fine_grained_mmap_memory(capacity));
#endif
#ifdef BULK
  auto bulk_mmap_data = reinterpret_cast<char *>(bulk_mmap_memory(capacity));
#endif

#ifdef MMAP
  load_memory(mmap_data, capacity);
#endif
#ifdef MALLOC
  load_memory(malloc_data, capacity);
#endif
#ifdef FINE_GRAINED
  load_memory(fine_grained_mmap_data, capacity);
#endif
#ifdef BULK
  load_memory(bulk_mmap_data, capacity);
#endif

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
#ifdef MMAP
    DLOG(INFO) << "checking mmap memory...";
    check_memory(mmap_data, capacity);
#endif
#ifdef MALLOC
    DLOG(INFO) << "checking malloc memory...";
    check_memory(malloc_data, capacity);
#endif
#ifdef FINE_GRAINED
    DLOG(INFO) << "checking fine grained mprotect'ed memory...";
    check_memory(fine_grained_mmap_data, capacity);
#endif
#ifdef BULK
    DLOG(INFO) << "checking bulk mprotect'ed memory...";
    check_memory(bulk_mmap_data, capacity);
#endif
  }

  std::cerr << "stop perf..." << std::endl;
  kill(perf_pid, SIGINT);
  waitpid(perf_pid, nullptr, 0);
}
