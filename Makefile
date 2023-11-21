all: mmap malloc fine-grained bulk mmap-huge fine-grained-huge bulk-huge

CC = g++ -std=c++20 -O3 -g

mmap:
	$(CC) mmap-minor-faults.cc -o mmap-minor-faults-mmap -lglog -ljemalloc -D MMAP -D NDEBUG

malloc:
	$(CC) mmap-minor-faults.cc -o mmap-minor-faults-malloc -lglog -D MALLOC -D NDEBUG

jemalloc:
	$(CC) mmap-minor-faults.cc -o mmap-minor-faults-malloc -lglog -ljemalloc -D MALLOC -D NDEBUG

fine-grained:
	$(CC) mmap-minor-faults.cc -o mmap-minor-faults-fine-grained -lglog -ljemalloc -D FINE_GRAINED -D NDEBUG

bulk:
	$(CC) mmap-minor-faults.cc -o mmap-minor-faults-bulk -lglog -ljemalloc -D BULK -D NDEBUG

mmap-huge:
	$(CC) mmap-minor-faults.cc -o mmap-minor-faults-mmap-huge -lglog -ljemalloc -D MMAP -D HUGETLB -D NDEBUG

fine-grained-huge:
	$(CC) mmap-minor-faults.cc -o mmap-minor-faults-fine-grained-huge -lglog -ljemalloc -D FINE_GRAINED -D HUGETLB -D NDEBUG

bulk-huge:
	$(CC) mmap-minor-faults.cc -o mmap-minor-faults-bulk-huge -lglog -ljemalloc -D BULK -D HUGETLB -D NDEBUG

