#include <errno.h>
#include "kernel.h"
#include <netinet/in.h>
#include <sys/time.h>

void kernel_init() {
    log_init();
    config_init();
}

int main(int argc, char** argv) {
    kernel_init();

    io_init();

    planificadores_init();

    server_init();

    while (1)
        ;  // TODO: definir exit strategy

    log_destroy(log_kernel);
}