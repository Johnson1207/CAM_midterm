#ifndef __PRINTER__H__
#define __PRINTER__H__

#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <string>
#include <vector>

struct Line {
    cv::Point a;
    cv::Point b;

    float slope;
    cv::Point mid;
};

class Printer {
public:
    Printer(std::string vertices_path, int tool_size = 10);
    Printer();
    ~Printer();

    void draw_polygon_contour();
    void draw_inner_polygon();
    void draw_scan_line();
    void draw_tool_path();

    void generate_g_codes(std::string dst);

private:
    void initial_vertives(std::string src);

    cv::Point find_interaction(Line lineA, Line lineB);
    void point2line(std::vector<cv::Point>& points, std::vector<Line>& lines, bool loop = 1);
    void line2point(std::vector<Line>& lines, std::vector<cv::Point>& points, bool concat = 1, bool loop = 1);

    cv::Mat img;
    cv::Mat inner;
    cv::Mat scan;
    cv::Mat path;
    cv::Point Origin;

    std::vector<cv::Point> input_vertices;
    std::vector<cv::Point> inner_vertices;
    std::vector<cv::Point> scan_vertices;
    std::vector<cv::Point> final_path;

    std::vector<Line> input_contour;
    std::vector<Line> inner_contour;
    std::vector<Line> scan_lines;
    std::vector<Line> tool_path;

    int tool;
};

#endif
