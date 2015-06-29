warmup2: warmup2.o my402list.o all_function.o
	gcc -o warmup2 -g warmup2.o my402list.o all_function.o -lm -D_POSIX_PTHREAD_SEMANTICS -pthread
warmup2.o: warmup2.c my402list.h warmup2.h
	gcc -g -c -Wall warmup2.c
my402list.o: my402list.c my402list.h
	gcc -g -c -Wall my402list.c
all_function.o: all_function.c warmup2.h
	gcc -g -c -Wall all_function.c -D_POSIX_PTHREAD_SEMANTICS
clean:
	rm -f *.o warmup2.o my402list.o all_function.o
