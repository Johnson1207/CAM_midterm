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
    this->img.copyTo(this->path);

    this->initial_vertives(vertices_path);

    this->tool = tool_size;
}

Printer::Printer() {}

Printer::~Printer()
{
    imwrite("Origin_Contour.jpg", this->img);
    imwrite("Inner_Contour.jpg", this->inner);
    imwrite("Scan_Line.jpg", this->scan);
    imwrite("Path.jpg", this->path);
    destroyAllWindows();
}

void Printer::draw_polygon_contour()
{
    for (uint i = 0; i < this->input_contour.size(); i++) {
        line(img, this->input_contour[i].a, this->input_contour[i].b, Scalar(0, 255, 0));
        line(inner, this->input_contour[i].a, this->input_contour[i].b, Scalar(255, 0, 0));
        line(scan, this->input_contour[i].a, this->input_contour[i].b, Scalar(255, 0, 0));
        line(path, this->input_contour[i].a, this->input_contour[i].b, Scalar(255, 0, 0));
    }

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
        if (this->input_contour[i].a.x != this->input_contour[i].b.x) {
            float tangentVector[2] = { -1 * (float)(this->input_contour[i].b.y - this->input_contour[i].a.y), 1 * (float)(this->input_contour[i].b.x - input_contour[i].a.x) };
            // cout << "Origin: " << tangentVector[0] << "  " << tangentVector[1] << endl;

            float tempX = tangentVector[0] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
            tangentVector[1] = tangentVector[1] / sqrt(pow(tangentVector[0], 2) + pow(tangentVector[1], 2));
            tangentVector[0] = tempX;

            lineTmp.a = Point(this->input_contour[i].a.x + this->tool * tangentVector[0], this->input_contour[i].a.y + this->tool * tangentVector[1]);
            lineTmp.b = Point(this->input_contour[i].b.x + this->tool * tangentVector[0], this->input_contour[i].b.y + this->tool * tangentVector[1]);

            lineTmp.slope = this->input_contour[i].slope;

            this->inner_contour.push_back(lineTmp);
        } else {
            if (this->input_contour[i].a.y > this->input_contour[i].b.y) {
                lineTmp.a = Point(this->input_contour[i].a.x + this->tool, this->input_contour[i].a.y);
                lineTmp.b = Point(this->input_contour[i].b.x + this->tool, this->input_contour[i].b.y);
            } else {
                lineTmp.a = Point(this->input_contour[i].a.x - this->tool, this->input_contour[i].a.y);
                lineTmp.b = Point(this->input_contour[i].b.x - this->tool, this->input_contour[i].b.y);
            }

            lineTmp.slope = this->input_contour[i].slope;

            this->inner_contour.push_back(lineTmp);
        }
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
            if (this->inner_contour[i].a.y < this->inner_contour[i].b.y) {
                if (this->inner_contour[i].a.y < s && s < this->inner_contour[i].b.y) {
                    tmp = this->find_interaction(this->inner_contour[i], scanning);
                    tmp.y = s;
                    this->scan_vertices.push_back(tmp);
                }
            } else if (this->inner_contour[i].a.y > this->inner_contour[i].b.y) {
                if (this->inner_contour[i].a.y > s && s > this->inner_contour[i].b.y) {
                    tmp = this->find_interaction(this->inner_contour[i], scanning);
                    tmp.y = s;
                    this->scan_vertices.push_back(tmp);
                }
            } else if (this->inner_contour[i].a.y == s) {
                if (i != 0) {
                    // check if it is local highest or local lowest point
                    if ((this->inner_contour[i].a.y > this->inner_contour[i].b.y && this->inner_contour[i].a.y > this->inner_contour[i - 1].a.y)) {
                        break;
                    }
                } else {
                    if ((this->inner_contour[i].a.y < this->inner_contour[i].b.y && this->inner_contour[i].a.y < this->inner_contour.back().a.y)) {
                        break;
                    }
                }
                this->scan_vertices.push_back(this->inner_contour[i].a);
            } else if (this->inner_contour[i].b.y == s) {
                if (i != this->inner_contour.size() - 1) {
                    // check if it is local highest or local lowest point
                    if ((this->inner_contour[i].b.y > this->inner_contour[i].a.y && this->inner_contour[i].b.y > this->inner_contour[i + 1].b.y)) {
                        break;
                    }
                } else {
                    if ((this->inner_contour[i].b.y > this->inner_contour[i].a.y && this->inner_contour[i].b.y > this->inner_contour[0].b.y)) {
                        break;
                    }
                }
                this->scan_vertices.push_back(this->inner_contour[i].b);
            }
        }

        s += this->tool * 2;
    }

    // sort
    Point pointTmp;
    for (uint i = 0; i < this->scan_vertices.size(); ++i) {
        if (i != scan_vertices.size() - 1) {
            if (this->scan_vertices[i].y == this->scan_vertices[i + 1].y) {
                if (this->scan_vertices[i].x > this->scan_vertices[i + 1].x) {
                    pointTmp = this->scan_vertices[i];
                    this->scan_vertices[i] = this->scan_vertices[i + 1];
                    this->scan_vertices[i + 1] = pointTmp;
                    i = -1;
                }
            }
        }
    }

    this->point2line(this->scan_vertices, this->scan_lines, 0);

    for (uint i = 0; i < this->scan_lines.size(); i++) {
        if (this->scan_lines[i].b == this->scan_lines[i + 1].a) {
            this->scan_lines.erase(this->scan_lines.begin() + i + 1);
        }

        line(this->scan, this->scan_lines[i].a, this->scan_lines[i].b, Scalar(0, 255, 0));
        line(this->path, this->scan_lines[i].a, this->scan_lines[i].b, Scalar(255, 0, 0));
    }

    imshow("scanning process", this->scan);
    waitKey(0);
}

void Printer::draw_tool_path()
{
    imshow("scanning process", this->path);
    waitKey(0);

    Line tmpLine;
    Point tmpPoint;

    for (uint i = 0; i < this->scan_lines.size() - 1; i++) {
        if ((this->scan_lines[i].b != this->scan_lines[i + 1].a) && (this->scan_lines[i].b.y == this->scan_lines[i + 1].a.y)) {
            this->scan_lines.push_back(this->scan_lines[i + 1]);
            this->scan_lines.erase(this->scan_lines.begin() + i + 1);
            i = -1;
        }
    }

    this->line2point(this->scan_lines, this->scan_vertices, 0, 0);

    for (uint i = 0; i < this->scan_vertices.size(); i += 2) {
        if (i % 4 == 0) {
            if (this->scan_vertices[i].x > this->scan_vertices[i + 1].x) {
                tmpPoint = this->scan_vertices[i];
                this->scan_vertices[i] = this->scan_vertices[i + 1];
                this->scan_vertices[i + 1] = tmpPoint;
            }
        } else {
            if (this->scan_vertices[i].x < this->scan_vertices[i + 1].x) {
                tmpPoint = this->scan_vertices[i];
                this->scan_vertices[i] = this->scan_vertices[i + 1];
                this->scan_vertices[i + 1] = tmpPoint;
            }
        }
    }

    for (uint i = 0; i < this->scan_vertices.size(); i++) {
        if (i + 1 != this->scan_vertices.size()) {
            tmpLine.a = this->scan_vertices[i];
            tmpLine.b = this->scan_vertices[i + 1];
        }

        this->tool_path.push_back(tmpLine);
    }

    for (uint i = 0; i < this->tool_path.size(); i++) {
        line(this->path, this->tool_path[i].a, this->tool_path[i].b, Scalar(0, 255, 0));
        // imshow("scanning process", this->path);
        // waitKey(0);
    }
    imshow("scanning process", this->path);
    waitKey(0);
}

void Printer::generate_g_codes(string dst)
{
    this->line2point(this->inner_contour, this->final_path, 1, 0);
    for (uint i = 0; i < this->inner_contour.size(); i++) {
        cout << inner_contour[i].a << this->inner_contour[i].b << endl;
    }

    vector<Point> tmp;
    this->line2point(this->tool_path, tmp, 1, 0);

    for (uint i = 0; i < final_path.size(); i++) {
        this->final_path[i].x = this->final_path[i].x - this->Origin.x;
        this->final_path[i].y = this->Origin.y - this->final_path[i].y;
    }

    // concat two vector
    this->final_path.insert(this->final_path.end(), tmp.begin(), tmp.end());

    std::ofstream file;
    file.open(dst);

    for (uint i = 0; i < this->final_path.size(); i++) {
        file << "N" << setfill('0') << setw(3) << i << " G01"
             << " X" << this->final_path[i].x << " Y" << this->final_path[i].y << "\n";
    }
    file.close();
}

void Printer::initial_vertives(string src)
{
    // read data
    fstream file(src);
    string line;
    string tmp;
    char delims = ',';

    int xTmp, yTmp;
    Point pointTmp;

    // Point centroid = { 0, 0 };

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

    this->point2line(this->input_vertices, this->input_contour);
}

cv::Point Printer::find_interaction(Line lineA, Line lineB)
{
    Point interaction;
    if (isfinite(lineA.slope) && (isfinite(lineB.slope))) {
        float interceptA = (float)lineA.a.y - lineA.slope * (float)lineA.a.x;
        float interceptB = (float)lineB.a.y - lineB.slope * (float)lineB.a.x;

        interaction.x = (int)round((interceptB - interceptA) / (lineA.slope - lineB.slope));
        interaction.y = (int)round(lineA.slope * (float)interaction.x + (float)interceptA);
    } else {
        if (isinf(lineA.slope)) {
            float interceptB = (float)lineB.a.y - lineB.slope * (float)lineB.a.x;
            interaction.x = lineA.a.x;
            interaction.y = (int)round(lineB.slope * (float)interaction.x + (float)interceptB);
        } else if (isinf(lineB.slope)) {
            float interceptA = (float)lineA.a.y - lineA.slope * (float)lineA.a.x;
            interaction.x = lineB.a.x;
            interaction.y = (int)round(lineA.slope * (float)interaction.x + (float)interceptA);
        }
    }
    // cout << interaction << endl;

    return interaction;
}

void Printer::point2line(vector<cv::Point>& points, vector<Line>& lines, bool loop)
{
    Line lineTmp;
    lines.clear();

    for (uint i = 0; i < points.size(); i++) {
        if (i != points.size() - 1) {
            lineTmp.a = points[i];
            lineTmp.b = points[i + 1];
        } else if (loop) {
            lineTmp.a = points[i];
            lineTmp.b = points[0];
        } else {
            break;
        }

        lineTmp.slope = ((float)lineTmp.a.y - (float)lineTmp.b.y) / ((float)lineTmp.a.x - (float)lineTmp.b.x);
        // cout << lineTmp.slope << endl;
        lineTmp.mid = Point((lineTmp.a.x + lineTmp.b.x) / 2, (lineTmp.a.y + lineTmp.b.y) / 2);

        lines.push_back(lineTmp);
    }
}

void Printer::line2point(std::vector<Line>& lines, std::vector<cv::Point>& points, bool concat, bool loop)
{
    Point pointTmp;
    points.clear();

    if (!concat) {
        for (uint i = 0; i < lines.size(); i++) {
            pointTmp = lines[i].a;
            points.push_back(pointTmp);
            pointTmp = lines[i].b;
            points.push_back(pointTmp);
        }
    } else {
        for (uint i = 0; i < lines.size(); i++) {
            pointTmp = lines[i].a;
            points.push_back(pointTmp);
            if (!loop && i == lines.size() - 1) {
                pointTmp = lines[i].b;
                points.push_back(pointTmp);
            }
        }
    }
}
