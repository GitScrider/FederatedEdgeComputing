#include "../lib/websocketserver.h"
#include <libwebsockets.h>


struct ThreadArgs {
    int port;
};


static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_HTTP:
            // Lida com as solicitações HTTP (se necessário)
            break;
        default:
            break;
    }

    return 0;
}


static struct lws_protocols protocols[] = {
    {"http-only", callback_http, 0, 0},
    {NULL, NULL, 0, 0} /* end of list */
};



void *start_websocketserver(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    struct lws_context_creation_info info;
    struct lws_context *context;
    const char *iface = "192.168.15.100";
    int port = 7681;

    memset(&info, 0, sizeof info);
    info.port = (int)threadArgs->port;
    info.iface = iface;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;

    context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "Erro ao criar o contexto WebSocket\n");
        return -1;
    }

    printf("Servidor WebSocket em execução. Aguarde as conexões...\n");

    while (1) {
        lws_service(context, 50);
        // Pode adicionar outras operações aqui se necessário
    }

    lws_context_destroy(context);

}