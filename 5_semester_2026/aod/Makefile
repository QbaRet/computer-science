CXX = clang++
CXXFLAGS = -std=c++20 -O3 -Wall -Wextra

TARGETS = lab1/zadanie1/zad1 lab1/zadanie2/zad2 lab1/zadanie3/zad3 lab1/zadanie4/zad4

all: $(TARGETS)

lab1/zadanie1/zad1: lab1/zadanie1/zad1.cpp
	$(CXX) $(CXXFLAGS) lab1/zadanie1/zad1.cpp -o lab1/zadanie1/zad1

lab1/zadanie2/zad2: lab1/zadanie2/zad2.cpp
	$(CXX) $(CXXFLAGS) lab1/zadanie2/zad2.cpp -o lab1/zadanie2/zad2

lab1/zadanie3/zad3: lab1/zadanie3/zad3.cpp
	$(CXX) $(CXXFLAGS) lab1/zadanie3/zad3.cpp -o lab1/zadanie3/zad3

lab1/zadanie4/zad4: lab1/zadanie4/zad4.cpp
	$(CXX) $(CXXFLAGS) lab1/zadanie4/zad4.cpp -o lab1/zadanie4/zad4

clean:
	rm -f $(TARGETS)

.PHONY: all clean
