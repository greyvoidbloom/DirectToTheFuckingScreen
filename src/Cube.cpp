#include "Cube.h" 
#include <math.h>
Cube::Cube(const char* buffer):FrameBuffer(buffer){
    originalvertices = {
        {-100, -100, -100}, {-100, 100, -100}, {100, 100, -100}, {100, -100, -100},
        {-100, -100, 100}, {-100, 100, 100}, {100, 100, 100}, {100, -100, 100}
    };
    vertices = originalvertices;
    edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    int sumX = 0, sumY = 0, sumZ = 0;
    for (const auto& v : vertices) {
        sumX += v[0];
        sumY += v[1];
        sumZ += v[2];
    }
    center = { static_cast<int>(sumX / vertices.size()), static_cast<int>(sumY / vertices.size()),static_cast<int>(sumZ / vertices.size())};

}
void Cube::drawPoint(int x, int y, uint32_t color){
    if (x < 0 || x >= fb_width || y < 0 || y >= fb_height) return; 
    int pixelIndex = (y * fb_width) + x;
    reinterpret_cast<std::uint32_t *>(fbptr)[pixelIndex] = color;}
void Cube::drawLine(int x1,int y1,int x2,int y2,uint32_t color){
    int xdiff = abs(x2-x1);
    int ydiff = abs(y2-y1);
    int xstep = (x2>x1)?1:-1;
    int ystep =(y2>y1)?1:-1;
    int decision = xdiff-ydiff;
    while(true){
        drawPoint(x1,y1,color);
        if(x1 == x2 && y1 == y2)break;
        int dec2 = 2* decision;
        if(dec2>-ydiff){
            decision-=ydiff;
            x1+=xstep;
        }
        if(dec2<xdiff){
            decision+=xdiff;
            y1+=ystep;
        }
    }
}
std::vector<int> Cube::projectPointTo2D(std::vector<int> vertex,float fov){
    float factor = fov/(fov+vertex[2]);
    int projectedX = static_cast<int>(vertex[0]*factor);
    int projectedY = static_cast<int>(vertex[1]*factor);
    return std::vector<int>{projectedX,projectedY};
}
void Cube::normaliseVertexes(float fov){
    for (const auto &vertex : vertices){
        std::vector<int> projectedVertex = Cube::projectPointTo2D(vertex,fov);
        normalisedPts.push_back({ stage_midX + projectedVertex[0], stage_midY + projectedVertex[1] });
    }
}
void Cube::spawnCube(float fov){
    normalisedPts.clear();
    Cube::normaliseVertexes(fov);
    for (const auto& edge : edges){
        int x1 = normalisedPts[edge[0]][0];
        int y1 = normalisedPts[edge[0]][1];
        int x2 = normalisedPts[edge[1]][0];
        int y2 = normalisedPts[edge[1]][1];
        Cube::drawLine(x1,y1,x2,y2,0xFF00FF00);   
    }
}
void Cube::rotateX(float angle) {
    float cosA = cos(angle);
    float sinA = sin(angle);
    for (size_t i = 0; i < vertices.size(); i++) {
        int y = originalvertices[i][1] - center[1];
        int z = originalvertices[i][2] - center[2];
        int newY = static_cast<int>(y * cosA - z * sinA);
        int newZ = static_cast<int>(y * sinA + z * cosA);
        vertices[i][1] = newY + center[1];
        vertices[i][2] = newZ + center[2];
    }
}

void Cube::rotateY(float angle) {
    float cosA = cos(angle);
    float sinA = sin(angle);

    for (size_t i = 0; i < vertices.size(); i++) {
        int x = originalvertices[i][0] - center[0];
        int z = originalvertices[i][2] - center[2];

        int newX = static_cast<int>(x * cosA + z * sinA);
        int newZ = static_cast<int>(-x * sinA + z * cosA);

        vertices[i][0] = newX + center[0];
        vertices[i][2] = newZ + center[2];
    }
}

/*void Cube::rotateX(float angle){
    for(auto&vertex : vertices){
        float tempY = vertex[1];
        vertex[1] = vertex[1] * cos(angle) - vertex[2] * sin(angle);
        vertex[2] = tempY * sin(angle) + vertex[2] * cos(angle);
    }
}
void Cube::rotateY(float angle){
    for(auto&vertex : vertices){
        float tempX = vertex[0];
        vertex[0] = vertex[0] * cos(angle) + vertex[2] * sin(angle);
        vertex[2] = -tempX * sin(angle) + vertex[2] * cos(angle);
    }
}*/
Cube::~Cube(){
    munmap(fbptr, screensize);
    close(fbfd);
}