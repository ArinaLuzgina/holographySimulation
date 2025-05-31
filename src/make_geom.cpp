#include "geometry.h"

int main(){

  std::vector<std::vector<Point>> vertices = {{{5.0, 2.0, 2.0}, {5.01, 2.0, 2.0}, {5.01, 2.01, 2.0}, {5.0, 2.01, 2.0}}, {{5.0, 7.0, 2.0}, {5.01, 7.0, 2.0}, {5.01, 7.01, 2.0}, {5.0, 7.01, 2.0}}};
std::vector<unsigned int> np = {1, 1};
    obj_geometry geom_book(1, np, vertices);
    geom_book.calculate_points();
    geom_book.calculate_surfaces();
    geom_book.save_geometry("geom_2p");
    return 0;
}

//for point
// std::vector<std::vector<Point>> vertices = {{{5.0, 5.0, 5.0}, {5.01, 5.0, 5.0}, {5.01, 5.01, 5.0}, {5.0, 5.01, 5.0}}};
// std::vector<std::vector<Point>> vertices = {{{3, 2.5, 7.5}, {7.5, 2.5, 7.5}, {7.5, 7.5, 7.5}, {2.5, 5, 5}}, {{2.5, 5, 5}, {7.5, 5, 5}, {7.5, 7.5, 7.5}, {3, 7.5, 7.5}}};
// std::vector<std::vector<Point>> vertices = {{{3, 3, 3}, {7, 3, 3}, {7, 7, 3}, {3, 7, 3}}};
//    std::vector<std::vector<Point>> vertices = {{{5.0, 2.0, 2.0}, {5.01, 2.0, 2.0}, {5.01, 2.01, 2.0}, {5.0, 2.01, 2.0}}, {{5.0, 7.0, 2.0}, {5.01, 7.0, 2.0}, {5.01, 7.01, 2.0}, {5.0, 7.01, 2.0}}};
