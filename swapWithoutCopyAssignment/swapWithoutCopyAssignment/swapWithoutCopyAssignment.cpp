// swapWithoutCopyAssignment.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <utility> 
//refer https://stackoverflow.com/questions/54610928/how-to-swap-two-objects-without-copy-assignment-operator/54611522#54611522
//the idea is to move construct a temporary object and then swap around by moving.
//move constructos have to be defaulted as they are not defined if at least one sepcial function 
//is deleted.
//see implementation of std::swap
class A {
private:
	int a;
public:
	A(int a_) : a(a_) {}
	A(const A& other) = delete;
	A& operator=(const A&) = delete;
	A(A&& other) {
		a = other.a;
	}
	A& operator=(A&& other) {
		a = other.a;
		return *this;
	}
	void showA() { std::cout << a << std::endl; }
};

int main(int argc, char* argv[]) {
	A obj1(10);
	A obj2(20);
	obj1.showA();
	obj2.showA();

	std::swap(obj1, obj2);
	std::cout << "swapped:" << std::endl;
	obj1.showA();
	obj2.showA();
	return 0;
}