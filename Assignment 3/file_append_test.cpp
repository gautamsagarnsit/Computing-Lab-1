#include<iostream>
#include<fstream>
using namespace::std;
int main()
{
    std::ofstream outfile;
    outfile.open("test.txt", std::ios_base::app); // append instead of overwrite
    outfile << "Data\n"; 
    return 0;

}

