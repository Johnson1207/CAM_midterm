#include <iostream>
#include "printer.h"

using namespace std;

int main(int argc, char** argv){
    if (argc<2){
        cerr << "Please enter: ./test.cpp <src_path>";
    }

    Printer printer(argv[1]);
    printer.draw_polygon_contour();

    return 0;
}
