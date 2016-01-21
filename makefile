#Makefile
#CXX = g++
#TARGET = jikken
#CXXFLAGS -02 

jikken: main.cpp env.cpp evaluator.cpp indi.cpp operator.cpp search.cpp rand.cpp  sort.cpp
	g++ -o jikken    -O2 main.cpp env.cpp evaluator.cpp indi.cpp operator.cpp search.cpp rand.cpp  sort.cpp -lm


run: jikken.exe
	./jikken.exe 1 DST small1k.tim 1 10 100000

test: jikken.exe
	./jikken.exe 1 DST small1k.tim 1 10 1

dclean:
	rm -f *Result
	rm -f *Sol
