// // #include <microhttpd.h>
// // #include <stdio.h>
// // #include <stdlib.h>
// // #include <string.h>
// // #include "../lib/api.h"

// // int ret;  


// // int post_iterator(void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
// //                   const char *filename, const char *content_type,
// //                   const char *transfer_encoding, const char *data, uint64_t off,
// //                   size_t size)
// // {
// //     // Esta função é chamada para cada parte do corpo da solicitação POST

// //     // Aqui, você pode processar os dados da solicitação POST conforme necessário
// //     // Neste exemplo, apenas imprimimos os dados

// //     printf("Dados brutos recebidos (parte %s): %.*s\n", key, (int)size, data);

// //     return MHD_YES; // Continuar processando
// // }

// // enum MHD_Result answer_to_connection(void *cls, struct MHD_Connection *connection,
// //                                      const char *url, const char *method,
// //                                      const char *version, const char *upload_data,
// //                                      size_t *upload_data_size, void **con_cls)
// // {



// //     const char *html_content = "<html><body>Federated Learning Server</body></html>";

// //     if (strcmp(url, "/") == 0) {
// //         struct MHD_Response *http_response = MHD_create_response_from_buffer(strlen(html_content), (void *)html_content, MHD_RESPMEM_PERSISTENT);
// //         MHD_add_response_header(http_response, "Content-Type", "text/html; charset=UTF-8");
// //         ret = MHD_queue_response(connection, MHD_HTTP_OK, http_response);
// //         MHD_destroy_response(http_response);

// //         return ret;
// //     }



// //     if (strcmp(method, "GET") == 0 && strcmp(url, "/api/testget") == 0) {
// //         const char *response = "Hello from the /api/hello endpoint!";
// //         struct MHD_Response *http_response = MHD_create_response_from_buffer(strlen(response), (void *)response, MHD_RESPMEM_PERSISTENT);
// //         ret = MHD_queue_response(connection, MHD_HTTP_OK, http_response);
// //         MHD_destroy_response(http_response);

// //         return ret;
// //     }



// // //  if (strcmp(method, MHD_HTTP_METHOD_POST) == 0 && strcmp(url, "/api/testpost") == 0) {
// // //         printf("post entrou %d\n", *upload_data_size);

// // //         // Verifica se há dados de upload
// // //         if (*upload_data_size != 0) {
// // //             // Se ainda não configurou o MHD_PostProcessor, configure-o
// // //             if (*con_cls == NULL) {
// // //                 struct MHD_PostProcessor *pp = MHD_create_post_processor(connection, 8192, &post_iterator, NULL);
// // //                 *con_cls = pp;
// // //                 return MHD_YES;
// // //             }

// // //             // Processa os dados POST
// // //             MHD_post_process(*con_cls, upload_data, *upload_data_size);
// // //             *upload_data_size = 0;

// // //             return MHD_YES;
// // //         } else {
// // //             // Processa os dados do POST que foram acumulados
// // //             // ...

// // //             // Destroi o MHD_PostProcessor
// // //             MHD_destroy_post_processor(*con_cls);

// // //             // Cria uma resposta de exemplo
// // //             const char *response = "Recebido JSON com sucesso em /api/testpost!";
// // //             struct MHD_Response *http_response = MHD_create_response_from_buffer(strlen(response), (void *)response, MHD_RESPMEM_PERSISTENT);

// // //             // Retorna a resposta ao cliente
// // //             int ret = MHD_queue_response(connection, MHD_HTTP_OK, http_response);
// // //             MHD_destroy_response(http_response);

// // //             return ret;
// // //         }
// // //     }


// // if (strcmp(method, MHD_HTTP_METHOD_POST) == 0 && strcmp(url, "/api/testpost") == 0) {
// //     printf("entrou");
// //     struct MHD_PostProcessor * pp = *con_cls;
// //     static int post_processor_created = 0;

// //     if (!post_processor_created && pp==NULL) {
// //     printf("entrou 1");

// //         // Cria o MHD_PostProcessor
// //         pp = MHD_create_post_processor(connection, 8192, &post_iterator, NULL);
// //         if (pp == NULL) {
// //                 fprintf(stderr, "Erro ao criar o MHD_PostProcessor: %s\n", strerror(errno));
// //                 fprintf(stderr, "Valor de errno: %d\n", errno);
// //         }

// //         *con_cls = pp;

// //         // Marca que o MHD_PostProcessor foi criado para esta conexão
// //         post_processor_created = 1;

// //         // Retorna MHD_YES para continuar o processamento
// //         return MHD_YES;
// //     }


// //     // Processa os dados POST se houver
// //     if (*upload_data_size != 0) {
// //     printf("entrou 2");

// //         MHD_post_process(pp, upload_data, *upload_data_size);
// //         *upload_data_size = 0;
// //         return MHD_YES;
// //     } else {

// //     printf("entrou 3");

// //             // Processa os dados do POST que foram acumulados
// //             // ...

// //             // Destroi o MHD_PostProcessor
// //             MHD_destroy_post_processor(*con_cls);

// //             // Cria uma resposta de exemplo
// //             const char *response = "Recebido JSON com sucesso em /api/testpost!";
// //             struct MHD_Response *http_response = MHD_create_response_from_buffer(strlen(response), (void *)response, MHD_RESPMEM_PERSISTENT);

// //             // Retorna a resposta ao cliente
// //             int ret = MHD_queue_response(connection, MHD_HTTP_OK, http_response);
// //             MHD_destroy_response(http_response);

// //             return ret;
// //         }
// //     }

// //     const char *default_response = "Endpoint not found";
// //     struct MHD_Response *default_http_response = MHD_create_response_from_buffer(strlen(default_response), (void *)default_response, MHD_RESPMEM_PERSISTENT);
// //     ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, default_http_response);
// //     MHD_destroy_response(default_http_response);

// //     return ret;
// // }


// #include <sys/types.h>
// #include <microhttpd.h>
// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include "../lib/api.h"

// #define PORT            1337
// #define POSTBUFFERSIZE  512
// #define MAXNAMESIZE     20
// #define MAXANSWERSIZE   512

// #define GET             0
// #define POST            1

// struct connection_info_struct
// {
//   int connectiontype;
//   char *answerstring;
//   struct MHD_PostProcessor *postprocessor;
// };

// const char *askpage = "<html><body>\
//                        What's your name, Sir?<br>\
//                        <form action=\"/namepost\" method=\"post\">\
//                        <input name=\"name\" type=\"text\"\
//                        <input type=\"submit\" ></form>\
//                        </body></html>";

// const char *json_response_page = "{\"status\": \"success\", \"message\": \"JSON data received successfully!\"}";

// const char *greetingpage =
//   "<html><body><h1>Welcome, %s!</center></h1></body></html>";

// const char *errorpage =
//   "<html><body>This doesn't seem to be right.</body></html>";


// static int send_page (struct MHD_Connection *connection, const char *page)
// {
//   int ret;
//   struct MHD_Response *response;


//   response = MHD_create_response_from_buffer (strlen (page), (void *) page,
// 				     MHD_RESPMEM_PERSISTENT);
//   if (!response)
//     return MHD_NO;

//   ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
//   MHD_destroy_response (response);

//   return ret;
// }

// // static void request_completed (void *cls, struct MHD_Connection *connection,
// //                    void **con_cls, enum MHD_RequestTerminationCode toe)
// // {
// //   struct connection_info_struct *con_info = *con_cls;

// //   if (NULL == con_info)
// //     return;

// //   if (con_info->connectiontype == POST)
// //     {
// //       MHD_destroy_post_processor (con_info->postprocessor);
// //       if (con_info->answerstring)
// //         free (con_info->answerstring);
// //     }

// //   free (con_info);
// //   *con_cls = NULL;
// // }

// static int iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
//               const char *filename, const char *content_type,
//               const char *transfer_encoding, const char *data, uint64_t off,
//               size_t size)
// {
// //   struct connection_info_struct *con_info = coninfo_cls;

// //   printf("%s %s\n",key,data);

// //   if (0 == strcmp (key, "name"))
// //     {
// //       if ((size > 0) && (size <= MAXNAMESIZE))
// //         {
// //           char *answerstring;
// //           answerstring = malloc (MAXANSWERSIZE);
// //           if (!answerstring)
// //             return MHD_NO;

// //           snprintf (answerstring, MAXANSWERSIZE, json_response_page, data);
// //           con_info->answerstring = answerstring;
// //         }
// //       else
// //         con_info->answerstring = NULL;

// //       return MHD_NO;
// //     }

// //   return MHD_YES;

// struct connection_info_struct *con_info = coninfo_cls;

//     printf("entrou iterate_post\n");
//     if (0 == strcmp(content_type, "application/json"))
//     {
//     printf("reconheceu o contenttype\n");

//         printf("%s\n",data);

//         con_info->answerstring = strdup(data);
//     }

//     return MHD_YES;

// }

// enum MHD_Result answer_to_connection (void *cls, struct MHD_Connection *connection,
//                       const char *url, const char *method,
//                       const char *version, const char *upload_data,
//                       size_t *upload_data_size, void **con_cls)
// {

//   if (*con_cls == NULL)
//     {
//       struct connection_info_struct *con_info;

//       con_info = malloc (sizeof (struct connection_info_struct));
      
//       if (NULL == con_info){
//         return MHD_NO;
//       }

//       con_info->answerstring = NULL;


//     if (0 == strcmp (method, "POST"))
//     {
//         printf("print maluco aqui\n");

//         con_info->postprocessor =  MHD_create_post_processor (connection, POSTBUFFERSIZE, iterate_post, (void *) con_info);

//         if (NULL == con_info->postprocessor)
//         {
//             free (con_info);
//             return MHD_NO;
//         }

//         con_info->connectiontype = POST;
//     }
//     else
//     {
//         con_info->connectiontype = GET;
//     }

//       *con_cls = (void *) con_info;

//       return MHD_YES;
//     }





//   if (strcmp(url, "/") == 0)
//     {
//       return send_page (connection, askpage);
//     }

//   if( strcmp (method, "POST") == 0 && strcmp(url, "/api/testpost") == 0)
//   {
//       struct connection_info_struct *con_info = *con_cls;
//       printf("entrou %d %s\n",*upload_data_size,upload_data);

//     if (*upload_data_size != 0)
//     {
//             MHD_post_process(con_info->postprocessor, upload_data,*upload_data_size);
//             *upload_data_size = 0;

//             return MHD_YES;
//     }
//         else if (NULL != con_info->answerstring)
//         {
//             return send_page(connection, con_info->answerstring);
//         }
//   }


//     if (0 == strcmp (method, "POST") )
//     {
//     struct connection_info_struct *con_info = *con_cls;

//     printf("entrou %d %s\n",*upload_data_size,upload_data);
//       if (*upload_data_size != 0)
//         {
//           MHD_post_process (con_info->postprocessor, upload_data,*upload_data_size);
//           *upload_data_size = 0;

//           return MHD_YES;
//         }
//       else if (NULL != con_info->answerstring)
//         return send_page (connection, con_info->answerstring);
//     }



//   return send_page (connection, errorpage);
// }



#include <sys/types.h>

#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define PORT            1337
#define POSTBUFFERSIZE  512
#define MAXNAMESIZE     20
#define MAXANSWERSIZE   512

#define GET             0
#define POST            1

struct connection_info_struct
{
    struct MHD_PostProcessor *postprocessor;
};

static int iterate_json_endpoint(void *coninfo_cls, enum MHD_ValueKind kind,
                                  const char *key, const char *filename,
                                  const char *content_type,
                                  const char *transfer_encoding, const char *data,
                                  uint64_t off, size_t size)
{
    printf("%s\n",data);
    if (data != NULL)
    {

        printf("chegou data %s\n",data);

    }

    return MHD_YES;
}

static void request_completed(void *cls, struct MHD_Connection *connection,
                               void **con_cls, enum MHD_RequestTerminationCode toe)
{
    struct connection_info_struct *con_info = *con_cls;

    if (NULL == con_info)
        return;

    MHD_destroy_post_processor(con_info->postprocessor);
    free(con_info);
    *con_cls = NULL;
}

enum MHD_Result answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls)
{
    if (NULL == *con_cls)
    {
        struct connection_info_struct *con_info;

        con_info = malloc(sizeof(struct connection_info_struct));
        if (NULL == con_info)
            return MHD_NO;

        con_info->postprocessor = MHD_create_post_processor(connection, POSTBUFFERSIZE,
                                  iterate_json_endpoint, (void *)con_info);

        if (NULL == con_info->postprocessor)
        {
            free(con_info);
            return MHD_NO;
        }

        *con_cls = (void *)con_info;

        return MHD_YES;
    }

    if (0 == strcmp(method, "POST") && strcmp(url, "/api/testpost") == 0)
    {
        struct connection_info_struct *con_info = *con_cls;
        printf("teste");
        if (*upload_data_size != 0)
        {
            MHD_post_process(con_info->postprocessor, upload_data,*upload_data_size);
            *upload_data_size = 0;

            return MHD_YES;
        }
    }

    return MHD_YES;
}

