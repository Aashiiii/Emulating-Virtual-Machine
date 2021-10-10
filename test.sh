#!/bin/bash

# USYD CODE CITATION ACKNOWLEDGEMENT
# I declare that i have copied the following lines of code from INFO1110 unit
# assignment resources, with only few changes. The changes i have made include # changing the programming language in which tests files need to be run, and 
# some minor changes in the echo statements. It is not my own work.
#
# URL- https://canvas.sydney.edu.au/courses/20732

echo "TESTING!"

count=0

for test in tests/*.x2017; do
	name=$(basename $test .x2017)
	expected_disassembler=tests/$name.asm
	expected_execution=tests/$name.out
	./objdump_x2017 tests/$name.x2017 | diff - tests/$name.asm || echo "Test disassembler $name failed!\n"
	./vm_x2017 tests/$name.x2017 | diff - tests/$name.out || echo "Test execution $name failed!\n"
	count=$((count+1))
done

echo "Finished running $count tests!"
