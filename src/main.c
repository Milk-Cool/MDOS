#include <mem.h>
#include <graphics.h>
#include <stdlib.h>
#include <stddef.h>

// The following will be our kernel's entry point.
// If renaming _start() to something else, make sure to change the
// linker script accordingly.
int main(void) {
    if(!ensureFramebuffer())
        abort();

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = getFramebuffer();

    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    uint32_t *fb_ptr = framebuffer->address;
    for (size_t i = 0; i < 256; i++) {
        fb_ptr[i * framebuffer->width + i] = /*i * 0x10000 + */i * 0x100 + i;
    }

    // We're done, just hang...
    abort();
}