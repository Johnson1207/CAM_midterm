#ifndef __PRINTER__H__
#define __PRINTER__H__

#include <iostream>
#include <string>
#include <vector>

struct point{
    int x;
    int y;
}

class Printer{
    public:
        Printer(std::string vertices);
        Printer();
        ~Printer();

        void draw_polygon_contour();
        void draw_inner_polygon();
        void draw_scan_line();
        void draw_tool_path();

    private:
        std::vector<point> input_vertices;
}
