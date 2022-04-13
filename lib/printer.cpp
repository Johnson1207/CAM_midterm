#include "printer.h"

using namespace std;
using namespace cv;

Printer::Printer(string vertices_path) {
    this->initial_vertives(vertices_path);
    img = Mat(500, 500, CV_8UC3, Scalar(0,0, 0));
}

Printer::Printer() {}

Printer::~Printer() {
    destroyAllWindows();
}

void Printer::draw_polygon_contour()
{
    for (uint i = 0; i < this->input_vertices.size(); i++) {
        cout << this->input_vertices[i].x << this->input_vertices[i].y << endl;

        if(i != this->input_vertices.size()){
            line(img, this->input_vertices[i], this->input_vertices[i+1], Scalar(100, 0, 0));
        }else{
            line(img, this->input_vertices[i], this->input_vertices[0], Scalar(100, 0, 0));
        }
    }

    imshow("polygon_contour", this->img);
    waitKey(0);
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
    Point pointTmp;

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
