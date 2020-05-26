#include "../include/net_stats_client.h"
#include <stdio.h>

void my_callback(int action, NTStatStream* stream) {
    printf("action:%d\n", action);
}

int main(int argc, char *argv[]) {
    run_ntstats(my_callback, 60, 1, 0);
    return 0;
}