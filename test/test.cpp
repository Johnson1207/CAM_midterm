#include "printer.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 2) {
        cerr << "Please enter: ./test.cpp <src_path>";
    }

    Printer printer(argv[1]);
    printer.draw_polygon_contour();
    printer.draw_inner_polygon();

    return 0;
}
