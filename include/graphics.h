#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <vector>
#include "visible_field.h"

// ╔════════════════════╗ 
// ║ External variables ║
// ╚════════════════════╝

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;



// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Run everything
void runSimulation(std::vector<std::vector<double>>& intensity, obj_visible_plate v_plate);


#endif // GRAPHICS_H
