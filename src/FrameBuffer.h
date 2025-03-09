#ifndef FRAME_H
#define FRAME_H
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
protected:
    int fbfd;
    struct fb_var_screeninfo vinfo;
    std::uint8_t *fbptr;
    int fb_width, fb_height, fb_bpp, fb_bytes;
    long screensize;
    int stage_x,stage_y,stage_width,stage_height;
    int stage_midX,stage_midY;
public:
    FrameBuffer(const char* buffer);
    void CopyInitial();
    void allowBackground(int color);
    void setStage(float x, float y, int width, int height);
    ~FrameBuffer();
};
#endif