#include "printer.h"

using namespace std;
using namespace cv;

Printer::Printer(string vertices_path, int tool_size)
{
    img = Mat(500, 500, CV_8UC3, Scalar(0, 0, 0));
    this->Origin = Point(img.cols / 2, img.rows / 2);
    line(img, Point(0, img.rows / 2), Point(img.cols, img.rows / 2), Scalar(255, 255, 255));
    line(img, Point(img.cols / 2, 0), Point(img.cols / 2, img.rows), Scalar(255, 255, 255));
    this->initial_vertives(vertices_path);

    this->tool = tool_size;
}

Printer::Printer() {}

Printer::~Printer() { destroyAllWindows(); }

void Printer::draw_polygon_contour()
{
    for (uint i = 0; i < this->input_contour.size(); i++) {
        line(img, this->input_contour[i].a, this->input_contour[i].b, Scalar(0, 0, 255));
    }
    Point centroid;
    centroid = this->find_centroid(this->input_vertices);

    circle(img, centroid, 2, Scalar(255, 0, 0));

    imshow("polygon_contour", this->img);
    waitKey(0);
}

void Printer::draw_inner_polygon()
{
    Line lineTmp;
    int right = 0;

    for (uint i = 0; i < this->input_contour.size(); i++) {
        cout << this->input_contour[i].mid << endl;
        float tangentVector[2] = { 1, -1 / this->input_contour[i].slope };
        cout << "Origin: " << tangentVector[0] << "  " << tangentVector[1] << endl;

        if ((this->input_contour[i].mid.x < this->Origin.x && this->input_contour[i].mid.y < this->Origin.y)) {
            if (this->input_contour[i].slope < 0) { // OK
                right = 1;
                cout << "right" << endl;
            } else if (this->input_contour[i].slope > 0) { //!!!!!!
                right = 0;
                cout << "left???" << endl;
            }
        } else if ((this->input_contour[i].mid.x > this->Origin.x && this->input_contour[i].mid.y < this->Origin.y)) {
            if (this->input_contour[i].slope < 0) { //!!!!!
                right = 0;
                cout << "right???" << endl;
            } else if (this->input_contour[i].slope > 0) { // OK
                right = -1;
                cout << "left" << endl;
            }
        } else if ((this->input_contour[i].mid.x > this->Origin.x && this->input_contour[i].mid.y > this->Origin.y)) {
            if (this->input_contour[i].slope < 0) { // OK
                right = -1;
                cout << "left" << endl;
            } else if (this->input_contour[i].slope > 0) { //!!!!!!!
                right = 0;
                cout << "right???" << endl;
            }
        } else if ((this->input_contour[i].mid.x < this->Origin.x && this->input_contour[i].mid.y > this->Origin.y)) {
            if (this->input_contour[i].slope < 0) { // !!!!!
                right = 0;
                cout << "left???" << endl;
            } else if (this->input_contour[i].slope > 0) { // OK
                right = 1;
                cout << "right" << endl;
            }
        }

        if (right == 1) {
            tangentVector[0] = tangentVector[0] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
            tangentVector[1] = tangentVector[1] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
        } else if (right == -1) {
            tangentVector[0] = -1 * tangentVector[0] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
            tangentVector[1] = -1 * tangentVector[1] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
        } else {
            if ((this->input_contour[i].a.x < this->Origin.x) && (this->input_contour[i].b.x < this->Origin.x)) {
                tangentVector[0] = tangentVector[0] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
                tangentVector[1] = tangentVector[1] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
            } else if ((this->input_contour[i].a.x > this->Origin.x) && (this->input_contour[i].b.x > this->Origin.x)) {
                tangentVector[0] = -1 * tangentVector[0] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
                tangentVector[1] = -1 * tangentVector[1] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
            } else if ((this->input_contour[i].a.y < this->Origin.y) && (this->input_contour[i].b.y < this->Origin.y)) {
                if (tangentVector[1] > 0) {
                    tangentVector[0] = tangentVector[0] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
                    tangentVector[1] = tangentVector[1] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
                } else if (tangentVector[1] < 0) {
                    tangentVector[0] = -1 * tangentVector[0] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
                    tangentVector[1] = -1 * tangentVector[1] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
                }
            } else if ((this->input_contour[i].a.y > this->Origin.y) && (this->input_contour[i].b.y > this->Origin.y)) {
                if (tangentVector[1] > 0) {
                    tangentVector[0] = -1 * tangentVector[0] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
                    tangentVector[1] = -1 * tangentVector[1] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
                } else if (tangentVector[1] < 0) {
                    tangentVector[0] = tangentVector[0] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
                    tangentVector[1] = tangentVector[1] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
                }
            }
        }

        lineTmp.a = Point(this->input_contour[i].a.x + this->tool * tangentVector[0], this->input_contour[i].a.y + this->tool * tangentVector[1]);
        lineTmp.b = Point(this->input_contour[i].b.x + this->tool * tangentVector[0], this->input_contour[i].b.y + this->tool * tangentVector[1]);

        line(img, lineTmp.a, lineTmp.b, Scalar(0, 255, 0));
        // line(img, this->inner_contour[i].a, this->inner_contour[i].b, Scalar(0, 255, 0));

        cout << "Unit: " << tangentVector[0] << "  " << tangentVector[1] << endl;
        imshow("polygon_contour", this->img);
        waitKey(0);
    }
    imshow("polygon_contour", this->img);
    waitKey(0);
}

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

    Point centroid = { 0, 0 };

    while (getline(file, line)) {
        std::istringstream istr(line);

        getline(istr, tmp, delims);
        sscanf(tmp.c_str(), "%d", &xTmp);

        getline(istr, tmp, delims);
        sscanf(tmp.c_str(), "%d", &yTmp);

        pointTmp.x = xTmp;
        pointTmp.y = yTmp;

        pointTmp.x = this->img.cols / 2 + xTmp;
        pointTmp.y = this->img.rows / 2 - yTmp;

        this->input_vertices.push_back(pointTmp);
    }

    centroid = this->find_centroid(this->input_vertices);

    for (uint i = 0; i < this->input_vertices.size(); i++) {
        this->input_vertices[i].x -= centroid.x;
        this->input_vertices[i].y -= centroid.y;
    }

    this->point2line(this->input_vertices, this->input_contour);
}

cv::Point Printer::find_centroid(vector<Point> vertices)
{
    Point centroid = { 0, 0 };
    double signedArea = 0.0;
    double x0 = 0.0; // Current vertex X
    double y0 = 0.0; // Current vertex Y
    double x1 = 0.0; // Next vertex X
    double y1 = 0.0; // Next vertex Y
    double a = 0.0;  // Partial signed area

    // For all vertices
    uint i = 0;
    for (i = 0; i < vertices.size(); ++i) {
        x0 = vertices[i].x;
        y0 = vertices[i].y;
        x1 = vertices[(i + 1) % (int)vertices.size()].x;
        y1 = vertices[(i + 1) % (int)vertices.size()].y;
        a = x0 * y1 - x1 * y0;
        signedArea += a;
        centroid.x += (x0 + x1) * a;
        centroid.y += (y0 + y1) * a;
    }

    signedArea *= 0.5;
    centroid.x /= (6.0 * signedArea);
    centroid.y /= (6.0 * signedArea);

    centroid.x = centroid.x - this->Origin.x;
    centroid.y = centroid.y - this->Origin.y;

    return centroid;
}

void Printer::find_interaction(Line line_A, Line line_B) {}

void Printer::point2line(vector<cv::Point>& points, vector<Line>& lines)
{
    Line lineTmp;

    for (uint i = 0; i < points.size(); i++) {
        if (i + 1 != points.size()) {
            lineTmp.a = points[i];
            lineTmp.b = points[i + 1];
        } else {
            lineTmp.a = points[i];
            lineTmp.b = points[0];
        }

        lineTmp.slope = ((float)lineTmp.a.y - (float)lineTmp.b.y) / ((float)lineTmp.a.x - (float)lineTmp.b.x);
        // cout << lineTmp.slope << endl;
        lineTmp.mid = Point((lineTmp.a.x + lineTmp.b.x) / 2, (lineTmp.a.y + lineTmp.b.y) / 2);

        lines.push_back(lineTmp);
    }
}
