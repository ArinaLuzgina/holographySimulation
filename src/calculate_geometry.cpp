#include <vector>
#include <fstream>
#include <iostream>

struct Point{
    double x;
    double y;
    double z;
};

class obj_geometry
{
private:
    double scale;
    std::vector<unsigned int> number_of_points{0, 0}; //начнем просто с задавания четырехуголника
    std::vector<Point> vertexes;
    std::vector<std::vector<Point>> points;

    bool calculate_points(){
        std::vector<Point> sides;
        double eps = 1e-9 * scale;
        for(size_t i = 0; i < 4; i ++){ // задаем векторы боковых граней
            if (i < 2){ 
            sides.push_back({vertexes[(i + 1) % 4].x - vertexes[i % 4].x, vertexes[(i + 1) % 4].y - vertexes[i % 4].y, vertexes[(i + 1) % 4].z - vertexes[i % 4].z});
            }else{
                sides.push_back({vertexes[i % 4].x - vertexes[(i + 1) % 4].x, vertexes[i % 4].y - vertexes[(i + 1) % 4].y, vertexes[i % 4].z - vertexes[(i + 1) % 4].z});
            }
        }
        for(size_t i = 0; i < 4; i++){ // задаем шаг на каждой из сторон
            sides[i].x = sides[i].x * 1.0 / number_of_points[i % 2];
            sides[i].y = sides[i].y * 1.0 / number_of_points[i % 2];
            sides[i].z = sides[i].z * 1.0 / number_of_points[i % 2];

        }
        for(size_t i = 0; i < number_of_points[0]; i ++){ // теперь посчитаем все точки, который у нас есть
            for(size_t j = 0; j < number_of_points[1]; j ++){
                std::vector<Point> sup_vec;
                for(size_t k = 0; k < 4; k ++){
                    if(k % 2 == 0){
                    sup_vec.push_back({sides[k].x * i, sides[k].y * i, sides[k].z * i});
                    }else{
                        sup_vec.push_back({sides[k].x * j, sides[k].y * i, sides[k].z * i});
                    }
                }
                double v = 0.0;
                if(sup_vec[1].x - sup_vec[3].x > eps)
                {
                    v = sup_vec[0].y - sup_vec[2].y + (sup_vec[3].y - sup_vec[1].y) * (sup_vec[0].x - sup_vec[2].x) / (sup_vec[1].x - sup_vec[3].x);
                    if(v < eps){
                        v = sup_vec[0].z - sup_vec[2].z + (sup_vec[3].z - sup_vec[1].z) * (sup_vec[0].x - sup_vec[2].x) / (sup_vec[1].x - sup_vec[3].x);                    
                    }
                } 
                else if (sup_vec[1].y - sup_vec[3].y > eps)
                {
                    v = sup_vec[0].x - sup_vec[2].x + (sup_vec[3].x - sup_vec[1].x) * (sup_vec[0].y - sup_vec[2].y) / (sup_vec[1].y - sup_vec[3].y);
                    if(v < eps){
                        v = sup_vec[0].z - sup_vec[2].z + (sup_vec[3].z - sup_vec[1].z) * (sup_vec[0].y - sup_vec[2].y) / (sup_vec[1].y - sup_vec[3].y);                    
                    }
                }
                else if (sup_vec[1].z - sup_vec[3].z > eps)
                {
                    v = sup_vec[0].x - sup_vec[2].x + (sup_vec[3].x - sup_vec[1].x) * (sup_vec[0].z - sup_vec[2].z) / (sup_vec[1].z - sup_vec[3].z);
                    if(v < eps){
                        v = sup_vec[0].y - sup_vec[2].y + (sup_vec[3].y - sup_vec[1].y) * (sup_vec[0].z - sup_vec[2].z) / (sup_vec[1].z - sup_vec[3].z);                    
                    }
                }
                if(v > eps){
                    //double t = (sup_vec[2].x - sup_vec[3].x + (sup_vec[0].x - sup_vec[2].x) * v) / (sup_vec[1].x - sup_vec[3].x);
                    points[i][j].x = sup_vec[2].x + (sup_vec[0].x - sup_vec[2].x) * v + vertexes[0].x;
                    points[i][j].y = sup_vec[2].y + (sup_vec[0].y - sup_vec[2].y) * v + vertexes[0].y;
                    points[i][j].z = sup_vec[2].z + (sup_vec[0].z - sup_vec[2].z) * v + vertexes[0].z;
                }else{
                    points[i][j] = vertexes[0];
                }
                
            }
        }       
    }

public:
    obj_geometry() : scale(1e-6), number_of_points{100, 100}, vertexes{{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}} {}
    obj_geometry(double scale, const std::vector<unsigned int>& number_of_points, const std::vector<Point>& vertexes) : scale(scale), number_of_points(number_of_points), vertexes(vertexes) {}
    ~obj_geometry() = default;

    bool save_geometry(const std::string& filename) const{
        std::ofstream file(filename);
        if(!file.is_open()){
            std::cerr << "Error: Could not open file " << filename << "\n";
            return false;
        }
        file << "Distance matrix (" << vertexes.size() << "x" << vertexes.size() << "):\n";
        for(const auto& row : points){
            for (Point val : row) {
                file << val.x << " " << val.y << " " << val.z << "\t";
            }
            file << "\n";
        }

        file.close();
        std::cout << "Matrix successfully written to " << filename << "\n";
        return true;
    }
};

obj_geometry::obj_geometry(/* args */)
{
}

obj_geometry::~obj_geometry()
{
}
