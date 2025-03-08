#include <iostream>
#include <vector>  
#include <fcntl.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

class FrameBuffer {
private:
    int fbfd;
    struct fb_var_screeninfo vinfo;
    std::uint8_t *fbptr;
    int fb_width, fb_height, fb_bpp, fb_bytes;
    long screensize;
    int stage_x,stage_y,stage_width,stage_height;
    int stage_midX,stage_midY;

public:
    FrameBuffer() {
        fbfd = open("/dev/fb0", O_RDWR);
        if (fbfd == -1) {
            perror("Error opening framebuffer device");
            exit(1);
        }

        if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
            perror("Error reading framebuffer info");
            close(fbfd);
            exit(1);
        }
        fb_width = vinfo.xres_virtual;
        fb_height = vinfo.yres;
        fb_bpp = vinfo.bits_per_pixel;
        fb_bytes = fb_bpp / 8;
        screensize = fb_width * fb_height * fb_bytes;
        fbptr = reinterpret_cast<std::uint8_t *>(mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0));
        if (fbptr == MAP_FAILED) {
            perror("Error mapping framebuffer memory");
            close(fbfd);
            exit(1);
        }
    }

    void copyInitial() {
        size_t bufferSize = static_cast<size_t>(stage_width) * stage_height * fb_bytes;
        if (bufferSize > 100000000) {
            perror("Error: Requested copy size too large!");
            return;
        }
        std::vector<std::uint8_t> original(bufferSize);
        for (int i = 0; i < stage_height; i++) {
            int screenOffset = ((stage_y + i) * fb_width + stage_x) * fb_bytes;
            if (screenOffset < 0 || screenOffset + (stage_width * fb_bytes) > screensize) {
                perror("Error: Attempted to access out-of-bounds memory");
                return;
            }
            int bufferOffset = i * stage_width * fb_bytes;
            std::memcpy(&original[bufferOffset], &fbptr[screenOffset], stage_width * fb_bytes);
        }
    }
    void setStage(float x, float y, int width, int height) {
        if(x == 0.5f){x = (fb_width-width)/2;}
        if(y == 0.5f){y = (fb_height-height)/2;}
        stage_x = static_cast<int>(x);
        stage_y = static_cast<int>(y);
        stage_width = width;
        stage_height = height;
        stage_midX = stage_x+stage_width/2;
        stage_midY = stage_y+stage_height/2;
        copyInitial();
        std::cout << "Set Stage from (" << stage_x << "," << stage_y << ") to (" << stage_x + stage_width << "," << stage_y + stage_height << ")." << std::endl;
        /*
        for(int i = 0;i<stage_height;i++){
            for(int j = 0;j<stage_width;j++){
                int pixelindex = ((stage_y+i)*fb_width)+(stage_x+j);
                reinterpret_cast<std::uint32_t*>(fbptr)[pixelindex] = 0xFFFFFFFF;
            }
        }*/

    }
    void drawPoint(int x, int y, uint32_t color){
        if (x < 0 || x >= fb_width || y < 0 || y >= fb_height) return; 
        int pixelIndex = (y * fb_width) + x;
        reinterpret_cast<std::uint32_t *>(fbptr)[pixelIndex] = color;}
    void drawLine(int x1,int y1,int x2,int y2,uint32_t color){
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
    std::vector<int> projectTo2D(std::vector<int> vertex,float fov){
        float factor = fov/(fov+vertex[2]);
        int projectedX = static_cast<int>(vertex[0]*factor);
        int projectedY = static_cast<int>(vertex[1]*factor);
        return std::vector<int>{projectedX,projectedY};
    }
    void drawCube(){
        std::vector<std::vector<int>> vertices = {
            {-100, -100, -100}, {-100, 100, -100}, {100, 100, -100}, {100, -100, -100},
            {-100, -100, 100}, {-100, 100, 100}, {100, 100, 100}, {100, -100, 100}
        };
        std::vector<std::vector<int>> edges = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},
            {4, 5}, {5, 6}, {6, 7}, {7, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7}
        };
        std::vector<std::vector<int>> normalisedPts;
        for (const auto &vertex : vertices){
            std::vector<int> projectedVertex = projectTo2D(vertex,400);//fov is 400, change later
            normalisedPts.push_back({ stage_midX + projectedVertex[0], stage_midY + projectedVertex[1] });
        }
        for (const auto& edge : edges){
            int x1 = normalisedPts[edge[0]][0];
            int y1 = normalisedPts[edge[0]][1];
            int x2 = normalisedPts[edge[1]][0];
            int y2 = normalisedPts[edge[1]][1];
            drawLine(x1,y1,x2,y2,0xFFFFFF);
            
        }
        //std::cout <<stage_midX << ","<<stage_midY << "\n";

    }
    ~FrameBuffer() {
        munmap(fbptr, screensize);
        close(fbfd);
    }
};

int main() {
    FrameBuffer cubu;
    cubu.setStage(0.5, 0.5, 800,600);
    cubu.drawCube();
}
