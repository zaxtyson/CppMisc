#include <iostream>
#include <thread>

using namespace std;

int main(int argc, char const *argv[])
{
    thread th = thread([] (){
        cout << "hello, id: " << this_thread::get_id() << endl;
    });
    // th.detach();
    th.join();
    return 0;
}
