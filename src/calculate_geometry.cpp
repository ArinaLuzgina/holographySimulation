#include <vector>


struct Point{
    double x;
    double y;
    double z;
};

class obj_geometry
{
private:
    double scale;
    std::vector<unsigned int> number_of_points;
    std::vector<Point> vertexes;

public:
    obj_geometry() : scale(1e-6), number_of_points{100, 100, 100}, vertexes{{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}} {}
    obj_geometry(double scale, const std::vector<unsigned int>& number_of_points, const std::vector<Point>& vertexes) : scale(scale), number_of_points(number_of_points), vertexes(vertexes) {}
    ~obj_geometry();
};

obj_geometry::obj_geometry(/* args */)
{
}

obj_geometry::~obj_geometry()
{
}
