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

    void copyInitial(int x, int y, int width, int height) {
        size_t bufferSize = static_cast<size_t>(width) * height * fb_bytes;
        if (bufferSize > 100000000) {
            perror("Error: Requested copy size too large!");
            return;
        }
        std::vector<std::uint8_t> original(bufferSize);
        for (int i = 0; i < height; i++) {
            int screenOffset = ((y + i) * fb_width + x) * fb_bytes;
            if (screenOffset < 0 || screenOffset + (width * fb_bytes) > screensize) {
                perror("Error: Attempted to access out-of-bounds memory");
                return;
            }
            int bufferOffset = i * width * fb_bytes;
            std::memcpy(&original[bufferOffset], &fbptr[screenOffset], width * fb_bytes);
        }
    }
    void setStage(float x, float y, int width, int height) {
        if(x == 0.5f){x = (fb_width-width)/2;}
        if(y == 0.5){y = (fb_height-height)/2;}
        int x_int = static_cast<int>(x);
        int y_int = static_cast<int>(y);
        copyInitial(x_int, y_int, width, height);
        std::cout << "Set Stage from (" << x_int << "," << y_int << ") to (" << x_int + width << "," << y_int + height << ")." << std::endl;
        for(int i = 0;i<height;i++){
            for(int j = 0;j<width;j++){
                int pixelindex = ((y+i)*fb_width)+(x+j);
                reinterpret_cast<std::uint32_t*>(fbptr)[pixelindex] = 0xFFFFFFFF;
            }
        }

    }
    ~FrameBuffer() {
        munmap(fbptr, screensize);
        close(fbfd);
    }
};

int main() {
    FrameBuffer cubu;
    cubu.setStage(0.5, 0.5, 800,600);
}
