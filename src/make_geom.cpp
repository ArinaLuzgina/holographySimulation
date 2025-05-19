#include "geometry.h"

int main(){

    std::vector<std::vector<Point>> vertices = {{{3.0, 2.5, 7.5}, {7.5, 2.5, 7.5}, {7.5, 5, 5}, {2.5, 5, 5}}, {{2.5, 5.0, 5.0}, {7.5, 5.0, 5.0}, {7.5, 7.5, 7.5}, {3.0, 7.5, 7.5}}};
    std::vector<unsigned int> np = {200, 200};
    obj_geometry geom_book(1e-6, np, vertices);
    geom_book.calculate_points();
    geom_book.calculate_surfaces();
    geom_book.save_geometry("geom_book");
    return 0;
}