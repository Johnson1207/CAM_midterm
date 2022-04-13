#include "printer.h"

using namespace std;
using namespace cv;

Printer::Printer(string vertices_path) { this->initial_vertives(vertices_path); }

Printer::Printer() {}

Printer::~Printer() {}

void Printer::draw_polygon_contour()
{
    for (uint i = 0; i < this->input_vertices.size(); i++) {
        cout << this->input_vertices[i].x << this->input_vertices[i].y << endl;
    }
}

void Printer::draw_inner_polygon() {}

void Printer::draw_scan_line() {}

void Printer::draw_tool_path() {}

void Printer::initial_vertives(string src)
{
    // read data
    fstream file(src);
    string line;
    string tmp;
    char delims = ',';

    int xTmp, yTmp;
    point pointTmp;

    while (getline(file, line)) {
        std::istringstream istr(line);

        getline(istr, tmp, delims);
        sscanf(tmp.c_str(), "%d", &xTmp);

        getline(istr, tmp, delims);
        sscanf(tmp.c_str(), "%d", &yTmp);

        pointTmp.x = xTmp;
        pointTmp.y = yTmp;

        this->input_vertices.push_back(pointTmp);
    }
}
