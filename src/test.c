#include <stdio.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>

int main() {
    int fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error opening framebuffer device");
        return -1;
    }

    struct fb_var_screeninfo vinfo;
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("Error reading variable information");
        close(fbfd);
        return -1;
    }

    int fb_width = vinfo.xres_virtual;
    int fb_height = vinfo.yres;
    int fb_bpp = vinfo.bits_per_pixel;
    int fb_bytes = fb_bpp / 8;

    long screensize = fb_width * fb_height * fb_bytes;
    uint8_t *fbptr = (uint8_t *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (fbptr == MAP_FAILED) {
        perror("Couldn't map framebuffer memory");
        close(fbfd);
        return -1;
    }

    printf("Drawing a 100x100 magenta square at (50,50)...\n");

    int square_size = 100;
    int start_x = fb_width/2-50, start_y = fb_height/2-50;
    int row_bytes = square_size * fb_bytes;
    uint8_t original[square_size * row_bytes];
    for (int y = 0; y < square_size; y++) {
        int fb_offset = ((start_y + y) * fb_width + start_x) * fb_bytes;
        memcpy(&original[y * row_bytes], &fbptr[fb_offset], row_bytes);
    }
    for (int y = 0; y < square_size; y++) {
        for (int x = 0; x < square_size; x++) {
            int pixel_index = ((start_y + y) * fb_width) + (start_x + x);
            ((uint32_t *)fbptr)[pixel_index] = 0xFFFF00FF;  // Magenta
        }
    }
    sleep(5);

    printf("Restoring original screen content...\n");
/*

    for (int y = 0; y < square_size; y++) {
        int fb_offset = ((start_y + y) * fb_width + start_x) * fb_bytes;
        memcpy(&fbptr[fb_offset], &original[y * row_bytes], row_bytes);
    }*/

    munmap(fbptr, screensize);
    close(fbfd);

    return 0;
}
