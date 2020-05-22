#include <unistd.h>
#include <stdio.h>
#include "wrapper.h"

void hi_cb() {
    printf("hi from c, this is callback\n");
}

void bye_cb() {
    printf("bye from c, this is callback\n");
}

int main(int argc, char **argv) {
    SayHello();
    RegisterCallbacks((void*)hi_cb, (void*)bye_cb, NULL);
    sleep(3);
    printf("[c] we're done here\n");
    return 0;
}
