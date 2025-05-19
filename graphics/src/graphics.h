#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <vector>


// ╔════════════════════╗ 
// ║ External variables ║
// ╚════════════════════╝

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;



// ╔═══════════╗ 
// ║ Functions ║
// ╚═══════════╝

// Run everything
void runSimulation(std::vector<std::vector<double>>& intensity);


#endif // GRAPHICS_H
