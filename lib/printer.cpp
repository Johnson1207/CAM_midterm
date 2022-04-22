#include "printer.h"

using namespace std;
using namespace cv;

Printer::Printer(string vertices_path, int tool_size)
{
    img = Mat(500, 500, CV_8UC3, Scalar(0, 0, 0));
    this->Origin = Point(img.cols / 2, img.rows / 2);
    line(img, Point(0, img.rows / 2), Point(img.cols, img.rows / 2), Scalar(255, 255, 255));
    line(img, Point(img.cols / 2, 0), Point(img.cols / 2, img.rows), Scalar(255, 255, 255));

    this->img.copyTo(this->inner);
    this->img.copyTo(this->scan);
    this->img.copyTo(this->scan);

    this->initial_vertives(vertices_path);

    this->tool = tool_size;
}

Printer::Printer() {}

Printer::~Printer() { destroyAllWindows(); }

void Printer::draw_polygon_contour()
{
    for (uint i = 0; i < this->input_contour.size(); i++) {
        line(img, this->input_contour[i].a, this->input_contour[i].b, Scalar(0, 255, 0));
        line(inner, this->input_contour[i].a, this->input_contour[i].b, Scalar(255, 0, 0));
        line(scan, this->input_contour[i].a, this->input_contour[i].b, Scalar(255, 0, 0));
        line(path, this->input_contour[i].a, this->input_contour[i].b, Scalar(255, 0, 0));
    }
    Point centroid;
    centroid = this->find_centroid(this->input_vertices);

    circle(img, centroid, 2, Scalar(255, 0, 0));

    imshow("scanning process", this->img);
    waitKey(0);
}

void Printer::draw_inner_polygon()
{
    imshow("scanning process", this->inner);
    waitKey(0);

    Line lineTmp;

    for (uint i = 0; i < this->input_contour.size(); i++) {
        // cout << this->input_contour[i].mid << endl;
        float tangentVector[2] = { -1 * (float)(this->input_contour[i].b.y - this->input_contour[i].a.y), 1 * (float)(this->input_contour[i].b.x - input_contour[i].a.x) };
        // cout << "Origin: " << tangentVector[0] << "  " << tangentVector[1] << endl;

        cout << "O:" << tangentVector[0] << " " << tangentVector[1] << endl;
        float tempX = tangentVector[0] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
        tangentVector[1] = tangentVector[1] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
        tangentVector[0] = tempX;

        cout << "U:" << tangentVector[0] << " " << tangentVector[1] << endl;

        lineTmp.a = Point(this->input_contour[i].a.x + this->tool * tangentVector[0], this->input_contour[i].a.y + this->tool * tangentVector[1]);
        lineTmp.b = Point(this->input_contour[i].b.x + this->tool * tangentVector[0], this->input_contour[i].b.y + this->tool * tangentVector[1]);

        lineTmp.slope = this->input_contour[i].slope;

        this->inner_contour.push_back(lineTmp);
        // cout << this->inner_contour[i].a << " " << this->inner_contour[i].b << endl;

        // line(inner, lineTmp.a, lineTmp.b, Scalar(0, 255, 0));
        // line(inner, this->inner_contour[i].a, this->inner_contour[i].b, Scalar(0, 255, 0));

        // cout << "Unit: " << tangentVector[0] << "  " << tangentVector[1] << endl;
    }
    // imshow("scanning process", this->inner);
    // waitKey(0);

    Point interaction;

    for (uint i = 0; i < this->inner_contour.size(); i++) {
        if (i + 1 != this->inner_contour.size()) {
            interaction = this->find_interaction(this->inner_contour[i], this->inner_contour[i + 1]);
            this->inner_vertices.push_back(interaction);
        } else {
            interaction = this->find_interaction(this->inner_contour[i], this->inner_contour[0]);
            this->inner_vertices.push_back(interaction);
        }
        // cout << this->inner_vertices[i] << endl;
    }

    this->point2line(this->inner_vertices, this->inner_contour);

    for (uint i = 0; i < this->inner_contour.size(); i++) {
        // cout << this->inner_contour[i].a << "  " << this->inner_contour[i].b << endl;
        line(this->inner, this->inner_contour[i].a, this->inner_contour[i].b, Scalar(0, 255, 0));
        line(this->scan, this->inner_contour[i].a, this->inner_contour[i].b, Scalar(255, 0, 0));
        line(this->path, this->inner_contour[i].a, this->inner_contour[i].b, Scalar(255, 0, 0));

        // imshow("scanning process", this->inner);
        // waitKey(0);
    }
    imshow("scanning process", this->inner);
    waitKey(0);
}

void Printer::draw_scan_line()
{
    imshow("scanning process", this->scan);
    waitKey(0);
    // Find highest point
    cv::Point highest;
    for (uint i = 0; i < this->inner_vertices.size(); i++) {
        if (i == 0)
            highest = this->inner_vertices[i];
        else if (highest.y > this->inner_vertices[i].y)
            highest = this->inner_vertices[i];
    }

    // Find lowest point
    cv::Point lowest;
    for (uint i = 0; i < this->inner_vertices.size(); i++) {
        if (i == 0)
            lowest = this->inner_vertices[i];
        else if (lowest.y < this->inner_vertices[i].y)
            lowest = this->inner_vertices[i];
    }

    // Find scan line
    int s = highest.y + this->tool;
    while (s < lowest.y) {
        Line scanning;
        scanning.a = Point(0, s);
        scanning.b = Point(img.cols, s);
        scanning.slope = 0;

        Point tmp;

        for (uint i = 0; i < this->inner_contour.size(); i++) {
            if (this->inner_contour[i].a.y<s<this->inner_contour[i].b.y || this->inner_contour[i].a.y> s> this->inner_contour[i].b.y) {
                tmp = this->find_interaction(this->inner_contour[i], scanning);
                this->scan_vertices.push_back(tmp);
            } else if (this->inner_contour[i].a.y == s) {
                this->scan_vertices.push_back(this->inner_contour[i].a);
            } else if (this->inner_contour[i].b.y == s) {
                this->scan_vertices.push_back(this->inner_contour[i].b);
            }
        }

        imshow("scanning process", this->scan);
        waitKey(0);

        s += this->tool * 2;
    }

    for (uint i = 0; i < this->scan_vertices.size(); i += 2) {
        line(this->scan, this->scan_vertices[i], this->scan_vertices[i + 1], Scalar(0, 255, 0));
        // line(this->path, this->scan_vertices.at(-1), this->scan_vertices.at(-2), Scalar(255, 0, 0));
        cout << this->scan_vertices[i] << " " << this->scan_vertices[i + 1] << endl;
    }
}

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

cv::Point Printer::find_interaction(Line lineA, Line lineB)
{
    Point interaction;
    float interceptA = (float)lineA.a.y - lineA.slope * (float)lineA.a.x;
    float interceptB = (float)lineB.a.y - lineB.slope * (float)lineB.a.x;

    // cout << "A: " << lineA.a.y << " - " << lineA.slope << " * " << lineA.a.x << " = " << interceptA << endl;
    // cout << "B: " << lineB.a.y << " - " << lineB.slope << " * " << lineB.a.x << " = " << interceptB << endl;
    // cout << interceptA << "  " << interceptB << endl;

    interaction.x = (int)round((interceptB - interceptA) / (lineA.slope - lineB.slope));
    interaction.y = (int)round(lineA.slope * (float)interaction.x + (float)interceptA);

    // cout << interaction << endl;

    return interaction;
}

void Printer::point2line(vector<cv::Point>& points, vector<Line>& lines)
{
    Line lineTmp;
    lines.clear();

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
