#include <mutex>

class some_data
{
    int a;
    std::string b;
public:
    void do_something(int _a, std::string _b)
    {
        a = _a;
        b = _b;
    }
};

class data_wrapper
{
private:
    some_data data;
    std::mutex m;
public:
    template<typename Function>
    void process_data(Function func)
    {
        //locks state
        std::lock_guard<std::mutex> l(m);
        //but gives out a reference to a member 
        //variable
        func(data);
    }
};

some_data* unprotected;

void malicious_function(some_data& protected_data)
{
    //gets a reference to protected data
    unprotected = &protected_data;
}

data_wrapper x;

int main()
{
    x.process_data(malicious_function);
    unprotected->do_something(1, "one");
    unprotected->do_something(2, "two");
}

