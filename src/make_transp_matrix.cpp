#include "hologram_prep.h"

int main(){

    std::string name_from = "geom_book_2";
    std::string name_to = "book_2_intensity.txt";
    double width = 10.0;
    double height = 10.0;
    double scale = 1e-6;
    std::vector<unsigned int> np = {500, 500};
    obj_plate plate(scale, np, width, height);
    plate.readDataFromFile(name_from);
    plate.calculate_intensity_matrix();
    plate.saveIntensityToFile(name_to);
    return 0;
}
