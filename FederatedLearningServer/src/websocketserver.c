#include "../lib/websocketserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <libwebsockets.h>

// Estrutura para passar argumentos para a thread
struct ThreadArgs {
    int port;
};

// Callback para tratamento de mensagens recebidas
static int callback_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_SERVER_WRITEABLE:
            // Implementar aqui se necessário
            break;
        case LWS_CALLBACK_RECEIVE:
            // Imprimir a mensagem recebida
            printf("Mensagem recebida: %.*s\n", (int)len, (char *)in);
            break;
        default:
            break;
    }

    return 0;
}

// Função que será executada pela thread para iniciar o servidor WebSocket
void *start_websocketserver(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int port = threadArgs->port;

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = port;
    info.gid = -1;
    info.uid = -1;
    info.options = 0;

    struct lws_protocols protocols[] = {
        {"echo-protocol", callback_echo, 0, 0},
        {NULL, NULL, 0, 0} // Fim da lista de protocolos
    };

    info.protocols = protocols;

    struct lws_context *context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "Erro ao criar o contexto do servidor WebSocket\n");
        return NULL;
    }

    printf("Servidor WebSocket iniciado em 192.168.15.100:%d\n", port);

    // Aguarde até que o contexto seja encerrado
    while (1) {
        lws_service(context, 50);
    }

    lws_context_destroy(context);

    return NULL;
}