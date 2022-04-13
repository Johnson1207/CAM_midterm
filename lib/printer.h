#ifndef __PRINTER__H__
#define __PRINTER__H__

#include <cstring>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <sstream>
#include <string>
#include <vector>


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

    cv::Mat img;

    std::vector<cv::Point> input_vertices;
};

#endif
