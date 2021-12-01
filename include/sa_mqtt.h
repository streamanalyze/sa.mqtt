#ifndef SA_MQTT_H
#define SA_MQTT_H
#include "MQTTAsync.h"
#include "sa_core.h"
#include "sa_circular_buff.h"
#include "sa_syscalls.h"
struct pubsub_opts
{
    /* debug app options */
    int publisher; /* publisher app? */
    int quiet;
    int verbose;
    int tracelevel;
    char *delimiter;
    int maxdatalen;
    /* message options */
    char *message;
    char *filename;
    int stdin_lines;
    int stdlin_complete;
    int null_message;
    /* MQTT options */
    int MQTTVersion;
    char *topic;
    char *clientid;
    int qos;
    int retained;
    char *username;
    char *password;
    char *host;
    char *port;
    char *connection;
    int keepalive;
    /* will options */
    char *will_topic;
    char *will_payload;
    int will_qos;
    int will_retain;
    /* TLS options */
    int insecure;
    char *capath;
    char *cert;
    char *cafile;
    char *key;
    char *keypass;
    char *ciphers;
    char *psk_identity;
    char *psk;
    /* MQTT V5 options */
    int message_expiry;
    struct
    {
        char *name;
        char *value;
    } user_property;
    /* websocket HTTP proxies */
    char *http_proxy;
    char *https_proxy;
    void *context;
};
#define Pubsub_opts_initializer {                  \
    1, 0, 0, 0, "\n", 100, /* debug/app options */ \
    NULL,                                          \
    NULL,                                          \
    1,                                             \
    0,                                             \
    0, /* message options */                       \
    MQTTVERSION_DEFAULT,                           \
    NULL,                                          \
    "sa.engine",                                   \
    0,                                             \
    0,                                             \
    NULL,                                          \
    NULL,                                          \
    "localhost",                                   \
    "1883",                                        \
    NULL,                                          \
    10, /* MQTT options */                         \
    NULL,                                          \
    NULL,                                          \
    0,                                             \
    0, /* will options */                          \
    0,                                             \
    NULL,                                          \
    NULL,                                          \
    NULL,                                          \
    NULL,                                          \
    NULL,                                          \
    NULL,                                          \
    NULL,                                          \
    NULL, /* TLS options */                        \
    0,                                             \
    {NULL, NULL}, /* MQTT V5 options */            \
    NULL,                                          \
    NULL,NULL};
#endif


extern void onDisconnect(void *context, MQTTAsync_successData *response);
extern void onSubscribe5(void *context, MQTTAsync_successData5 *response);
extern void onSubscribe(void *context, MQTTAsync_successData *response);
extern void onSubscribeFailure5(void *context, MQTTAsync_failureData5 *response);
extern void onSubscribeFailure(void *context, MQTTAsync_failureData *response);
extern void onUnSubscribe5(void *context, MQTTAsync_successData5 *response);
extern void onUnSubscribe(void *context, MQTTAsync_successData *response);
extern void onUnSubscribeFailure5(void *context, MQTTAsync_failureData5 *response);
extern void onUnSubscribeFailure(void *context, MQTTAsync_failureData *response);
extern void onConnectFailure5(void *context, MQTTAsync_failureData5 *response);
extern void onConnectFailure(void *context, MQTTAsync_failureData *response);
extern void onConnect5(void *context, MQTTAsync_successData5 *response);
extern void onConnect(void *context, MQTTAsync_successData *response);
extern void onPublishFailure5(void *context, MQTTAsync_failureData5 *response);
extern void onPublishFailure(void *context, MQTTAsync_failureData *response);
extern void onPublish5(void *context, MQTTAsync_successData5 *response);
extern void onPublish(void *context, MQTTAsync_successData *response);