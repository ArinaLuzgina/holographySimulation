#include "geometry.h"

int main(){

    std::vector<std::vector<Point>> vertices = {{{5.0, 5.0, 5.0}, {5.01, 5.0, 5.0}, {5.01, 5.01, 5.0}, {5.0, 5.01, 5.0}}};//{{{3.0, 2.5, 7.5}, {7.5, 2.5, 7.5}, {7.5, 5, 5}, {2.5, 5, 5}}, {{2.5, 5, 5}, {7.5, 5, 5}, {7.5, 7.5, 7.5}, {3, 7.5, 7.5}}};//, {{2.5, 5.0, 2.5}, {7.5, 5.0, 2.5}, {7.5, 7.5, 5}, {3.0, 7.5, 5}}}//{{{5, 5, 1}, {5, 5, 1}, {5, 5, 1}, {5, 5, 1}}};//{{{3.0, 2.5, 5.0}, {7.5, 2.5, 5.0}, {7.5, 5, 2.5}, {2.5, 5, 2.5}}};//, {{2.5, 5.0, 2.5}, {7.5, 5.0, 2.5}, {7.5, 7.5, 5}, {3.0, 7.5, 5}}};
    std::vector<unsigned int> np = {1, 1};
    obj_geometry geom_book(1e-6, np, vertices);
    geom_book.calculate_points();
    geom_book.calculate_surfaces();
    geom_book.save_geometry("geom_book_2");
    return 0;
}

//for point
// std::vector<std::vector<Point>> vertices = {{{5.0, 5.0, 5.0}, {5.01, 5.0, 5.0}, {5.01, 5.01, 5.0}, {5.0, 5.01, 5.0}}};
// std::vector<std::vector<Point>> vertices = {}
