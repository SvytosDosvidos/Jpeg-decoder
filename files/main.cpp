#include <iostream>
#include "parser.cpp"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "usage: jpg_decoder <file.jpg>\n";
        return 1;
    }

    cout << "this_open\n";
    parser decoder;
    decoder.decode(argv[1]);
    return 0;
}