#ifndef __PRINTER__H__
#define __PRINTER__H__

#include <cstring>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <string>
#include <vector>

struct point {
    int x;
    int y;
};

class Printer {
public:
    Printer(std::string vertices_path);
    Printer();
    ~Printer();

    void draw_polygon_contour();
    void draw_inner_polygon();
    void draw_scan_line();
    void draw_tool_path();

private:
    void initial_vertives(std::string src);

    std::vector<point> input_vertices;
};

#endif
