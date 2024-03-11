#include "../lib/websocketserver.h"
#include "../lib/websockethandlers.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <libwebsockets.h>


// Estrutura para passar argumentos para a thread
struct ThreadArgs {
    int port;
};

typedef struct client_message {
    const char* message;
    int length;
    struct client_message *previous;
    struct client_message *next;
} client_message;

typedef struct buffer_client_message {
    client_message *first;
    client_message *last;
} buffer_client_message;

buffer_client_message* get_buffer_client_message() {
    static buffer_client_message *buffer = NULL;
    if (buffer == NULL) {
        init_buffer_client_message(&buffer);
    }
    return buffer;
}

int is_buffer_client_message_empty() {
    buffer_client_message *buffer = get_buffer_client_message();
    return buffer->last == NULL;
}


void init_buffer_client_message(buffer_client_message **handle_message) {
    if (*handle_message == NULL) {
        *handle_message = (buffer_client_message *)malloc(sizeof(buffer_client_message));
        (*handle_message)->first = NULL;
        (*handle_message)->last = NULL;
    }
}

void insert_buffer_client_message(const char* message, int length) {

    buffer_client_message *buffer = get_buffer_client_message();
    client_message *new_client_message = (client_message *)malloc(sizeof(client_message));
    if (new_client_message == NULL) {
        printf("Failed to allocate memory for new client message.\n");
        return;
    }

    new_client_message->message = strdup(message);
    if (new_client_message->message == NULL) {
        printf("Failed to allocate memory for message copy.\n");
        free(new_client_message); // Libera a estrutura client_message alocada
        return;
    }

    new_client_message->length = length;

    if (is_buffer_client_message_empty(buffer)) {
        buffer->first = new_client_message;
        buffer->last = new_client_message;
        buffer->first->next = NULL;
        buffer->first->previous = NULL;
    } else {
        buffer->last->next = new_client_message;
        new_client_message->previous = buffer->last;
        new_client_message->next = NULL;
        buffer->last = new_client_message;
    }

}


void remove_buffer_client_message() {

    buffer_client_message *buffer = get_buffer_client_message();
    if (is_buffer_client_message_empty()) {
        printf("Buffer is empty. Cannot remove.\n");
        return;
    }

    client_message *removed_message = buffer->first;

    printf("Removed message: %d %s\n",removed_message->length, removed_message->message);
    handle_clint_model_message(removed_message->message,removed_message->length);


    if (buffer->first == buffer->last) {
        buffer->first = NULL;
        buffer->last = NULL;
    } else {
        buffer->first = buffer->first->next;
        buffer->first->previous = NULL;
    }

    free(removed_message);
}

void handle_message() {
    while (!is_buffer_client_message_empty())
    {
        remove_buffer_client_message();
    }
    
}

static int is_complete_json(const char *json, size_t len) {
    int count_open_braces = 0;
    int count_close_braces = 0;

    for (size_t i = 0; i < len; ++i) {
        if (json[i] == '{') {
            count_open_braces++;
        } else if (json[i] == '}') {
            count_close_braces++;
        }
    }

    return (count_open_braces == count_close_braces && count_open_braces > 0);
}

#define MAX_JSON_SIZE 16384 // Tamanho máximo da string JSON

static char received_json[MAX_JSON_SIZE + 1]; // +1 para o terminador nulo
static size_t received_json_len = 0;

// Callback para tratamento de mensagens recebidas
static int callback_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_SERVER_WRITEABLE:
            // Implementar aqui se necessário
            break;
        case LWS_CALLBACK_RECEIVE:
            if (received_json_len + len > MAX_JSON_SIZE) {
                fprintf(stderr, "Mensagem recebida muito grande\n");
                return -1;
            }

            memcpy(received_json + received_json_len, in, len);
            received_json_len += len;

            if (is_complete_json(received_json, received_json_len)) {
                
                received_json[received_json_len] = '\0';
                printf("Received JSON: %.*s\n",received_json_len ,received_json);
                insert_buffer_client_message(received_json, received_json_len);
                received_json_len = 0;
            }

            break;
        default:
            break;
    }

    return 0;
}

void *start_websocketserver(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int port = threadArgs->port;

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = port;
    info.gid = -1;
    info.uid = -1;
    info.options = 0;
    info.max_http_header_data = 16384;

    struct lws_protocols protocols[] = {
        {"echo-protocol", callback_echo, 0, 0},
        {NULL, NULL, 0, 0} 
    };

    info.protocols = protocols;

    struct lws_context *context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "Erro ao criar o contexto do servidor WebSocket\n");
        return NULL;
    }

    printf("WebSocket Server running in ws://192.168.1.5:%d\n", port);

    while (1) {
        lws_service(context, 50);
        handle_message();
    }

    lws_context_destroy(context);

    return NULL;
}