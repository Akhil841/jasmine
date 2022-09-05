#include <iostream>
using namespace std;

//make a string 14 chars long
std::string offset(std::string badstring)
{
    std::string out = badstring;
    for (int i = 0; i < 14 - badstring.length(); i++)
        out += " ";
    return out;
}

int
main()
{
    cout << offset("hello") << "end" << endl;
    return 0;
}