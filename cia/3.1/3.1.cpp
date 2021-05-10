// 3.1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <list>
#include <mutex>
#include <algorithm>
#include <iostream>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value)
{
    std::lock_guard<std::mutex> guard(some_mutex);
    some_list.push_back(new_value);
}
void list_contains(int value_to_find)
{
    std::lock_guard<std::mutex> guard(some_mutex);
    auto res = std::find(some_list.begin(), some_list.end(), value_to_find)
        != some_list.end();
    std::cout << "contains("
        << value_to_find << ")=" << res << std::endl;
    return;
}

int main()
{
    std::thread t1(list_contains, 4);
    add_to_list(42);
    std::thread t2(list_contains, 42);
    add_to_list(1);
    t1.join();
    t2.join();
    std::thread t3(list_contains, 1);
    t3.join();
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
