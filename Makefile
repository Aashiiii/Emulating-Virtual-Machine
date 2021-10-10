CC=gcc
CFLAGS=-fsanitize=address -Wvla -Wall -Werror -s -std=gnu11 

# fill in all your make rules

vm_x2017: vm_x2017.c
	$(CC) $(CFLAGS) $^ -o $@

temp: temp.c
	$(CC) $(CFLAGS) $^ -o $@ 

objdump_x2017: objdump_x2017.c
	$(CC) $(CFLAGS) $^ -o $@ 

o: o.c
	$(CC) $(CFLAGS) $^ -o $@

tests:
	bash test.sh

run_tests:
	bash test.sh

clean:
	rm vm_x2017
	rm objdump_x2017
	echo "clean"

