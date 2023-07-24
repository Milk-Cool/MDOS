// https://github.com/limine-bootloader/limine-barebones/blob/trunk/kernel/kernel.c
// TODO: split into graphics.h and graphics.c

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.

#include <stddef.h>
#include <stdbool.h>
#include <limine.h>

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

bool ensureFramebuffer() {
    if (framebuffer_request.response == NULL
        || framebuffer_request.response->framebuffer_count < 1)
        return false;
    return true;
}

struct limine_framebuffer* getFramebuffer() {
    return framebuffer_request.response->framebuffers[0];
}