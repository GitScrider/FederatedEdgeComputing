#include <stdio.h>
#include <microhttpd.h>
#include <conio.h>
#include "../lib/api.h"

#define PORT 1337

int kbhit(void) {
    return _kbhit();
}



int main() {

    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);

    if (NULL == daemon) {
        fprintf(stderr, "Falha ao iniciar o servidor.\n");
        return 1;
    }

    while (1) {

    }

    MHD_stop_daemon(daemon);
    printf("Servidor encerrado.\n");

    return 0;
}