#include "hologram_prep.h"

int main(){

    std::string name_from = "geom_book_sq";
    std::string name_to = "book_intensity_sq.txt";
    double width = 10.0;
    double height = 10.0;
    double scale = 1e-6;
    std::vector<unsigned int> np = {1024, 1024};
    obj_plate plate(scale, np, width, height);
    plate.readDataFromFile(name_from);
    plate.calculate_intensity_matrix();
    plate.saveIntensityToFile(name_to);
    return 0;
}
