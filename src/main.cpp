#include <chrono>
#include "Cube.h"
int main() {
    Cube cubu("/dev/fb0");
    cubu.setStage(0.5, 0.5, 800, 600);
    //cubu.allowBackground(0xFF0000FF);

    bool running = true;
    auto startTime = std::chrono::steady_clock::now();

    while (running) {
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration<float>(now - startTime).count();

        float angle = elapsed * 0.5f; 

        cubu.allowBackground(0xFF000000); 

        //cubu.rotateX(angle);
        cubu.rotateY(angle);

        cubu.spawnCube(500);

        usleep(8000);  // ~60 FPS
    }

    return 0;
}
