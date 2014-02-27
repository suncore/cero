

#include "stdincs.h"
#include "cpp2cero.h"
#include "build.h"

using namespace std;
#define LINELEN 2048


void bail()
{
    printf(
        "Usage: cero <command> args\n"
        "commands:\n"
        "\tb mainfile.ce\t\tbuild whole system\n"
        "\tc file.ce\t\tcompile single file\n"
        "\tcpp2cero infile.cpp outfile.ce\tConvert C++ file to cero file\n");

    exit(1);
}


int main(int argc, char** argv)
{
    if (argc < 3) bail();
   
    if (string(argv[1]) == "b")
    {
        if (argc != 3) bail();
        build(string(argv[2]), true);
    }
    else if (string(argv[1]) == "c")
    {
        if (argc != 3) bail();
        build(string(argv[2]), false);
    }
    else if (string(argv[1]) == "cpp2cero")
    {
        if (argc != 4) bail();
        cpp2cero(string(argv[2]), string(argv[3]));
    }
    else
    {
        printf("Unrecognized command '%s'\n", argv[1]);
        bail();
    } 

    return 0;
}        
