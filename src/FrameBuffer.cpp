#include "FrameBuffer.h"
FrameBuffer::FrameBuffer(const char* buffer){
    fbfd = open(buffer, O_RDWR);
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
void FrameBuffer::CopyInitial() {
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
void FrameBuffer::allowBackground(int color){
    for(int i = 0;i<stage_height;i++){
        for(int j = 0;j<stage_width;j++){
            int pixelindex = ((stage_y+i)*fb_width)+(stage_x+j);
            reinterpret_cast<std::uint32_t*>(fbptr)[pixelindex] = color;
        }
    }
}
void FrameBuffer::setStage(float x, float y, int width, int height) {
    if(x == 0.5f){x = (fb_width-width)/2;}
    if(y == 0.5f){y = (fb_height-height)/2;}
    stage_x = static_cast<int>(x);
    stage_y = static_cast<int>(y);
    stage_width = width;
    stage_height = height;
    stage_midX = stage_x+stage_width/2;
    stage_midY = stage_y+stage_height/2;
    FrameBuffer::CopyInitial();
    std::cout << "Set Stage from (" << stage_x << "," << stage_y << ") to (" << stage_x + stage_width << "," << stage_y + stage_height << ")." << std::endl;
    //FrameBuffer::allowBackground(0xFF0000FF);
}
FrameBuffer::~FrameBuffer() {
    munmap(fbptr, screensize);
    close(fbfd);
}