#include "../lib/websocketserver.h"

#include <uv.h>
#include <libwebsockets.h>
#include <stdio.h>
#include <arpa/inet.h>

struct ThreadArgs {
    int port;
};



int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t lenght) {

    
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            if (wsi == NULL) {
                // Lida com o caso em que wsi é nulo
                return -1;
            }

            // Obter o endereço IP do cliente
            struct sockaddr_storage addr;
            socklen_t len = sizeof(addr);
            getpeername(lws_get_socket_fd(wsi), (struct sockaddr*)&addr, &len);

            if (addr.ss_family == AF_INET) {
                // IPv4
                char ip[INET_ADDRSTRLEN];
                struct sockaddr_in *s = (struct sockaddr_in *)&addr;
                inet_ntop(AF_INET, &s->sin_addr, ip, sizeof(ip));
                printf("user: %s ",(char *)user);
                printf("Nova conexão WebSocket estabelecida. IP do cliente (IPv4): %s\n", ip);
            } else if (addr.ss_family == AF_INET6) {
                // IPv6
                char ip[INET6_ADDRSTRLEN];
                struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
                inet_ntop(AF_INET6, &s->sin6_addr, ip, sizeof(ip));
                printf("user: %s ",(char *)user);
                printf("Nova conexão WebSocket estabelecida. IP do cliente (IPv6): %s\n", ip);
            } else {
                printf("Tipo de endereço não suportado.\n");
            }


 
             break;

        case LWS_CALLBACK_RECEIVE:
            if (in == NULL) {
                // Lida com o caso em que in é nulo
                return -1;
            }
            // Restante do código...
            // Tratamento básico de uma mensagem recebida

            printf("Mensagem recebida: %.*s Message Size: %d\n", (int)lenght,(char *)in,(int)lenght);

            // Exemplo: Verificar se a mensagem contém uma palavra específica
            char *message = (char *)in;
            if (strstr(message, "start") != NULL) {
                printf("Comando 'start' recebido!\n");
                // Realizar ação correspondente ao comando 'start'
            }

            // Outras lógicas de processamento de mensagens podem ser adicionadas aqui

            break;

        // Outros casos de callback...
        default:
            break;
    }

    return 0;
}



// struct lws_protocols protocols[] = {
//     {
//         "my-protocol",
//         callback,
//         0,
//         4096,  // Tamanho do buffer de recebimento
//     },
//     {NULL, NULL, 0, 0}  // Fim da lista de protocolos
// };


struct lws_protocols protocols[] = {
    {
        "my-protocol",  // Nome do protocolo
        callback,       // Função de retorno de chamada
        0,              // ID do protocolo, 0 é comumente usado para WebSocket
        4096,           // Tamanho do buffer de recebimento
        0,              // Percebemos que todos os outros campos foram inicializados, mas ainda precisamos lidar com 'id'
        NULL,
        0
    }
};


void *start_websocketserver(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;

    struct lws_context_creation_info info = {0};
    info.port = (int)threadArgs->port;  // Porta para o servidor WebSocket
    info.protocols = protocols;

    struct lws_context *context = lws_create_context(&info);


    printf("Servidor WebSocket rodando em  ws://127.0.0.1:%d/\n", (int)threadArgs->port);

    // Loop principal do servidor WebSocket
    while (1) {
        lws_service(context, 50);  // 50 ms timeout
        // Outras lógicas do servidor podem ser adicionadas aqui
    }

    lws_context_destroy(context);

    return 0;
}


