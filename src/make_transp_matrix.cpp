#include "hologram_prep.h"

int main(){

    std::string name_from = "geom_2p";
    std::string name_to = "intensity_2p.txt";
    double width = 10.0;
    double height = 10.0;
    double scale = 1;
    std::vector<unsigned int> np = {256, 256};
    obj_plate plate(scale, np, width, height, M_PI / 180);
    plate.readDataFromFile(name_from);
    plate.calculate_intensity_matrix();
    plate.saveIntensityToFile(name_to);
    return 0;
}
