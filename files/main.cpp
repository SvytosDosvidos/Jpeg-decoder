#include <iostream>
#include "decode.cpp"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "usage: jpg_decoder <file.jpg>\n";
        return 1;
    }

    Decode(argv[1]);
    return 0;
}
