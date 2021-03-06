// name_lookup.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

namespace Foo {
	namespace Bar {
		template <typename T>
		struct Meow {
			void purr() {
				std::cout << "Foo::Bar::Meow<T>::purr()" << std::endl;
			}
		};
	}
	using Bar::Meow;
}

template <> struct Foo::Meow<int> {
	void purr() {
		std::cout << "Foo::Bar::Meow<int>::purr()" << std::endl;
	}
};

namespace Qux {
	namespace Moo {
		struct Kitty{};

		void PURR(Kitty k) {
			using std::cout;
			using std::endl;
			cout << "Qux::Moo::PURR()" << endl;
		}
	}
	using Moo::Kitty; //brings out type kitty to the outer namespace Qux

	void PURR2(Kitty k) {
		using std::cout;
		using std::endl;
		cout << "Qux::Moo::PURR2()" << endl;
	}
}

int main()
{
	/*
	Name Lookup
	Template Argument Deduction
	Overload Resolution
	Access Control 
	Virutual functions
	*/
	std::cout << "Meow\n"; //does normal lookup
	operator<<(std::cout, "Meow\n"); //does Argument dependent lookup

	/*
	operator overlaoding is syntactic sugar for function calls
	*/

	/*
	c++11 range based for 
	std::vector<int> v;
	for(auto& e:v) //looks for vector namespace
	for(auto& something: somethings) //looksfor begin(somethings) and end(somethings) in somethings namespace,
	//if its exists
	*/

	//typedefs and namespace aliases
	/*
	typedef Foo::Bar::Meow::Kitty FBMK; 
	Whenver the class Foo::Bar::Meow::Kitty is used we can use the fake name FBMK instead
	the thing above works only for class names

	namespace FBM = Foo::Bar::Meow;
	is called namespace alias

	**Note the syntax in typdef the typeto be deffed comes first
	typedef a b; means b can be used in place of a, whereas, namespace alias 
	work like varialble names namespace a = b; implies, using a:: is equivalent to doing
	b:: for opening a namespace**
	*/

	/*template specialization*/
	using Foo::Meow;
	Meow<long> l;
	l.purr();

	Meow<int> i;
	i.purr();


	Qux::Kitty k; //as Kitty is visible in Qux because of using Moo::Kitty;
	PURR(k); //does work because PURR lives in namespaces as kitty and Arguement Dependent LookUp kicks in
	//Node PURR is not just visible outside

	//Qux::Kitty k2;
	//PURR2(k2); //does not work because PURR2 and Kitty live in different namespaces
	//However,	
	//Qux::PURR2(k2); //works

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
