all: clean
	clang++ -std=c++14 test.cpp -o test

.PHONY : clean 
clean:
	-rm test  