#ifndef CUBE_H
#define CUBE_H
#include "FrameBuffer.h"
class Cube : public FrameBuffer{
private:
    std::vector<std::vector<int>>vertices;
    std::vector<std::vector<int>> originalvertices;
    std::vector<std::vector<int>> edges;
    std::vector<std::vector<int>> normalisedPts;
    std::vector<int> center = {0,0,0};

public:
    Cube(const char* buffer);
    void drawPoint(int x, int y, uint32_t color);
    void drawLine(int x1,int y1,int x2,int y2,uint32_t color);
    std::vector<int> projectPointTo2D(std::vector<int> vertex,float fov);
    void normaliseVertexes(float fov);
    void spawnCube(float fov);
    void rotateX(float angle);
    void rotateY(float angle);
    ~Cube();
};
#endif