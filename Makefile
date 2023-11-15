all: mmap malloc fine-grained bulk mmap-huge fine-grained-huge bulk-huge
mmap:
	g++ mmap-minor-faults.cc -o mmap-minor-faults-mmap -lglog -ljemalloc -O3 -g -D MMAP -D NDEBUG

malloc:
	g++ mmap-minor-faults.cc -o mmap-minor-faults-malloc -lglog -ljemalloc -O3 -g -D MALLOC -D NDEBUG

fine-grained:
	g++ mmap-minor-faults.cc -o mmap-minor-faults-fine-grained -lglog -ljemalloc -O3 -g -D FINE_GRAINED -D NDEBUG

bulk:
	g++ mmap-minor-faults.cc -o mmap-minor-faults-bulk -lglog -ljemalloc -O3 -g -D BULK -D NDEBUG

mmap-huge:
	g++ mmap-minor-faults.cc -o mmap-minor-faults-mmap-huge -lglog -ljemalloc -O3 -g -D MMAP -D HUGETLB -D NDEBUG

fine-grained-huge:
	g++ mmap-minor-faults.cc -o mmap-minor-faults-fine-grained-huge -lglog -ljemalloc -O3 -g -D FINE_GRAINED -D HUGETLB -D NDEBUG

bulk-huge:
	g++ mmap-minor-faults.cc -o mmap-minor-faults-bulk-huge -lglog -ljemalloc -O3 -g -D BULK -D HUGETLB -D NDEBUG

