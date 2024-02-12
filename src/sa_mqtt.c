#include "sa_mqtt.h"
#include "MQTTAsync.h"
#include <signal.h>

#include "sa_datapump.h"
#include "sa_handle.h"
#include "sa_threads.h"
#include <dlfcn.h>

typedef struct sa_mqtt_instance_s {
  MQTTAsync *client;
  int qos;
  sa_datapump *pump;
  char name[20];
} sa_mqtt_instance;

typedef struct sa_mqtt_buff_item_s {
  char *topicName;
  int topicLen;
  MQTTAsync_message *message;
} sa_mqtt_buff_item;

#define maybe_set_int(env, var, record, field)                                 \
  {                                                                            \
    ohandle temp = record_get(record, field);                                  \
    if (temp != nil) {                                                         \
      IntoInteger(temp, var, env);                                             \
    }                                                                          \
  }

#define maybe_set_string(env, var, record, field)                              \
  {                                                                            \
    ohandle temp = record_get(record, field);                                  \
    if (temp != nil) {                                                         \
      var = strdup(getstring(temp));                                           \
    }                                                                          \
  }

int getopts(bindtype env, ohandle rec, struct pubsub_opts *opts) {
  opts->MQTTVersion = MQTTVERSION_3_1_1;
  opts->verbose = (record_get(rec, "verbose") != nil);
  opts->quiet = (record_get(rec, "quiet") != nil);
  maybe_set_int(env, opts->qos, rec, "qos");
  maybe_set_string(env, opts->connection, rec, "connection");
  maybe_set_string(env, opts->clientid, rec, "clientid");
  maybe_set_string(env, opts->username, rec, "username");
  maybe_set_string(env, opts->password, rec, "password");
  maybe_set_int(env, opts->maxdatalen, rec, "maxdatalen");
  maybe_set_int(env, opts->keepalive, rec, "keepalive");
  maybe_set_string(env, opts->topic, rec, "topic");
  maybe_set_string(env, opts->will_topic, rec, "will-topic");
  maybe_set_string(env, opts->will_payload, rec, "will-payload");
  maybe_set_int(env, opts->will_qos, rec, "will-qos");
  opts->will_retain = (record_get(rec, "will-retain") != nil);
  opts->insecure = (record_get(rec, "insecure") != nil);
  maybe_set_string(env, opts->capath, rec, "capath");
  maybe_set_string(env, opts->cafile, rec, "cafile");
  maybe_set_string(env, opts->cert, rec, "cert");
  maybe_set_string(env, opts->key, rec, "key");
  maybe_set_string(env, opts->keypass, rec, "keypass");
  maybe_set_string(env, opts->ciphers, rec, "ciphers");
  maybe_set_string(env, opts->psk, rec, "psk");
  maybe_set_string(env, opts->psk_identity, rec, "psk-identity");
  maybe_set_string(env, opts->psk_identity, rec, "psk-identity");
  maybe_set_int(env, opts->MQTTVersion, rec, "version");
  if (opts->publisher == 0) {
    opts->retained = (record_get(rec, "no-retained") == nil);
  } else if (opts->publisher == 1) {
    opts->retained = (record_get(rec, "retained") != nil);
    maybe_set_string(env, opts->user_property.name, rec, "user-property-name");
    maybe_set_string(env, opts->user_property.value, rec,
                     "user-property-value");
    maybe_set_int(env, opts->message_expiry, rec, "message-expiry");
  }
  return 0;
}

int messageArrived(void *context, char *topicName, int topicLen,
                   MQTTAsync_message *message) {

  sa_mqtt_buff_item item;
  sa_mqtt_instance *sa_cxt = (sa_mqtt_instance *)context;
  item.topicLen = topicLen;
  item.topicName = topicName;
  item.message = message;
  sa_datapump_push(sa_cxt->pump, &item);
  return 1;
}

static int onSSLError(const char *str, size_t len, void *context) {
  return fprintf(stderr, "SSL error: %s\n", str);
}

static unsigned int onPSKAuth(const char *hint, char *identity,
                              unsigned int max_identity_len, unsigned char *psk,
                              unsigned int max_psk_len, void *context) {
  int psk_len;
  int k, n;

  int rc = 0;
  struct pubsub_opts *opts = context;

  /* printf("Trying TLS-PSK auth with hint: %s\n", hint);*/

  if (opts->psk == NULL || opts->psk_identity == NULL) {
    /* printf("No PSK entered\n"); */
    goto exit;
  }

  /* psk should be array of bytes. This is a quick and dirty way to
   * convert hex to bytes without input validation */
  psk_len = (int)strlen(opts->psk) / 2;
  if (psk_len > max_psk_len) {
    fprintf(stderr, "PSK too long\n");
    goto exit;
  }
  for (k = 0, n = 0; k < psk_len; k++, n += 2) {
    sscanf(&opts->psk[n], "%2hhx", &psk[k]);
  }

  /* identity should be NULL terminated string */
  strncpy(identity, opts->psk_identity, max_identity_len);
  if (identity[max_identity_len - 1] != '\0') {
    fprintf(stderr, "Identity too long\n");
    goto exit;
  }

  /* Function should return length of psk on success. */
  rc = psk_len;

exit:
  return rc;
}

int mqtt_connect(MQTTAsync *client, struct pubsub_opts *opts) {
  MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
  MQTTAsync_SSLOptions ssl_opts = MQTTAsync_SSLOptions_initializer;
  MQTTAsync_willOptions will_opts = MQTTAsync_willOptions_initializer;
  int rc = 0;

  if (opts->MQTTVersion == MQTTVERSION_5) {
    MQTTAsync_connectOptions conn_opts5 = MQTTAsync_connectOptions_initializer5;
    conn_opts = conn_opts5;
    conn_opts.onSuccess5 = onPublish5;
    conn_opts.onFailure5 = onPublishFailure5;
    conn_opts.onSuccess5 = onConnect5;
    conn_opts.onFailure5 = onConnectFailure5;
    conn_opts.cleanstart = 1;
  } else {
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.cleansession = 1;
  }

  conn_opts.keepAliveInterval = opts->keepalive;
  conn_opts.username = opts->username;
  conn_opts.password = opts->password;
  conn_opts.MQTTVersion = opts->MQTTVersion;

  conn_opts.context = opts->context;
  conn_opts.automaticReconnect = 1;
  conn_opts.httpProxy = opts->http_proxy;
  conn_opts.httpsProxy = opts->https_proxy;

  if (opts->will_topic) /* will options */
  {
    will_opts.message = opts->will_payload;
    will_opts.topicName = opts->will_topic;
    will_opts.qos = opts->will_qos;
    will_opts.retained = opts->will_retain;
    conn_opts.will = &will_opts;
  }

  if (opts->connection && (strncmp(opts->connection, "ssl://", 6) == 0 ||
                           strncmp(opts->connection, "wss://", 6) == 0)) {
    if (opts->insecure)
      ssl_opts.verify = 0;
    else
      ssl_opts.verify = 1;
    ssl_opts.CApath = opts->capath;
    ssl_opts.keyStore = opts->cert;
    ssl_opts.trustStore = opts->cafile;
    ssl_opts.privateKey = opts->key;
    ssl_opts.privateKeyPassword = opts->keypass;
    ssl_opts.enabledCipherSuites = opts->ciphers;
    ssl_opts.ssl_error_cb = onSSLError;
    ssl_opts.ssl_error_context = opts->context;
    ssl_opts.ssl_psk_cb = onPSKAuth;
    ssl_opts.ssl_psk_context = &opts;
    conn_opts.ssl = &ssl_opts;
  }
  rc = MQTTAsync_connect(*client, &conn_opts);
  return rc;
}

void onDisconnectFailure(void *context, MQTTAsync_failureData *response) {}
void onSendFailure(void *context, MQTTAsync_failureData *response) {
  MQTTAsync client = (MQTTAsync)context;
  MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
  int rc;

  printf("Message send failed token %d error code %d\n", response->token,
         response->code);
  opts.onSuccess = onDisconnect;
  opts.onFailure = onDisconnectFailure;
  opts.context = client;
  if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS) {
    printf("Failed to start disconnect, return code %d\n", rc);
    exit(EXIT_FAILURE);
  }
}

void free_mqtt_client(sa_datapump *pump) {
  sa_mqtt_instance *context = (sa_mqtt_instance *)pump->context;
  MQTTAsync *client = context->client;
  sa_datapump_destroy(pump);
  MQTTAsync_destroy(client);
  free(context);
}

ohandle mqtt_publishfn(bindtype env, ohandle instance, ohandle topic,
                       ohandle value) {

  char *dtopic;
  int rc, i;
  sa_datapump *pump;
  ohandle str = nil;
  MQTTAsync_responseOptions pub_opts = MQTTAsync_responseOptions_initializer;
  MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
  if (a_datatype(instance) != MEMORYTYPE)
    return a_throw_errorno(env, ILLEGAL_ARGUMENT, instance);
  pump = dr(instance, memorycell)->data;
  sa_mqtt_instance *context = pump->context;
  IntoString(topic, dtopic, env);
  if (a_datatype(value) == LISTTYPE) {
    if (a_length(value) == 1) {
      value = hd(value);
    }
  }
  if (a_datatype(value) == STRINGTYPE) {
    IntoString(value, pubmsg.payload, env);
    pubmsg.payloadlen = strlen(pubmsg.payload);
  } else if (a_datatype(value) == MEMORYTYPE) {
    pubmsg.payload = dr(value, memorycell)->data;
    pubmsg.payloadlen = dr(value, memorycell)->size;
  } else if (a_datatype(value) == BINARYTYPE) {
    size_t size = sa_size(value);
    size_t binsize;
    pubmsg.payload = malloc(size);
    sa_getbinary(pubmsg.payload, size, &binsize, value);
    pubmsg.payloadlen = size;
  } else {
    sa_json_stringify(&str, value);
    IntoStringRef(str, pubmsg.payload, env);
    pubmsg.payloadlen = sa_size(str);
  }

  pubmsg.qos = context->qos;
  pubmsg.retained = 0;
  i = 0;
  while ((rc = MQTTAsync_sendMessage(*context->client, dtopic, &(pubmsg),
                                     &(pub_opts))) != MQTTASYNC_SUCCESS) {
    if (rc == MQTTASYNC_MAX_BUFFERED_MESSAGES && i++ < 10) {
      a_sleep(0.1 * i * i);
      continue;
    }
    sa_raise_errormsg(MQTTAsync_strerror(rc), instance);
    break;
  }
  if (a_datatype(value) == BINARYTYPE)
    free(pubmsg.payload);
  if (str != nil)
    a_free(str);
  return nil;
}

ohandle mqtt_subscribefn(bindtype env, ohandle instance, ohandle topic) {

  sa_datapump *pump;
  sa_mqtt_instance *context;
  char *dtopic;
  int rc, i = 0;
  if (a_datatype(instance) != MEMORYTYPE)
    return a_throw_errorno(env, ILLEGAL_ARGUMENT, instance);
  pump = dr(instance, memorycell)->data;
  context = pump->context;
  IntoString(topic, dtopic, env);

  MQTTAsync_responseOptions ropts = MQTTAsync_responseOptions_initializer;

  ropts.onSuccess = onSubscribe;
  ropts.onFailure = onSubscribeFailure;
  ropts.context = (context->client);
  while ((rc = MQTTAsync_subscribe(*(context->client), dtopic, context->qos,
                                   &ropts)) != MQTTASYNC_SUCCESS &&
         i++ < 10) {
    a_sleep(0.1 * i * i);
  }
  if (i == 10) {
    sa_raise_errormsg(MQTTAsync_strerror(rc), topic);
    return nil;
  }
  return tsymbol;
}
ohandle mqtt_unsubscribefn(bindtype env, ohandle instance, ohandle topic) {

  char *dtopic;
  sa_datapump *pump;
  sa_mqtt_instance *context;
  MQTTAsync_responseOptions ropts = MQTTAsync_responseOptions_initializer;
  if (a_datatype(instance) != MEMORYTYPE)
    return a_throw_errorno(env, ILLEGAL_ARGUMENT, instance);
  pump = dr(instance, memorycell)->data;
  context = pump->context;
  IntoString(topic, dtopic, env);
  ropts.onSuccess = onUnSubscribe;
  ropts.onFailure = onUnSubscribeFailure;
  ropts.context = (context->client);
  int rc;
  if ((rc = MQTTAsync_unsubscribe(*(context->client), dtopic, &ropts)) !=
      MQTTASYNC_SUCCESS) {
    sa_raise_errormsg(MQTTAsync_strerror(rc), topic);
    return nil;
  }
  return tsymbol;
}

void make_mqtt_binary(ohandle *ret, sa_datapump *p,void *data) {
  sa_mqtt_buff_item *item = (sa_mqtt_buff_item *)data;
  sa_makebinary(ret, item->message->payload, item->message->payloadlen);
  MQTTAsync_freeMessage(&(item->message));
}

size_t get_topic_name(sa_datapump *p, void *data, char *const topic_name,
                      size_t topic_name_len) {
  sa_mqtt_buff_item *item = (sa_mqtt_buff_item *)data;

  strncpy(topic_name, item->topicName, topic_name_len);
  MQTTAsync_free(item->topicName);
  return strlen(topic_name);
}
#define SA_MQTT_CIRC_BUFF_SIZE 1000
ohandle mqtt_register_clientfn(bindtype env, ohandle name,
                               ohandle opts_record) {
  char *dname;
  sa_mqtt_instance *context;
  MQTTAsync_createOptions createOpts = MQTTAsync_createOptions_initializer;
  MQTTAsync *client = (MQTTAsync *)malloc(sizeof(*client));
  int rc;

  context = (sa_mqtt_instance *)malloc(sizeof(*context));
  context->client = client;
    IntoString(name, dname, env);

  if (stringlen(name) > 20) {
    return sa_raise_errormsg("Broker id cannot be more than 20 chars.",
                        opts_record);
  } else {
    memset(context->name, 0, 20);
    memcpy(context->name, dname, strlen(dname));
  }

  struct pubsub_opts opts = Pubsub_opts_initializer;
  opts.context = context;
  getopts(env, opts_record, &opts);
  context->qos = opts.qos;
  createOpts.sendWhileDisconnected = 1;
  if (opts.MQTTVersion >= MQTTVERSION_5) {
    createOpts.MQTTVersion = MQTTVERSION_5;
  }
  rc = MQTTAsync_createWithOptions(client, opts.connection, opts.clientid,
                                   MQTTCLIENT_PERSISTENCE_NONE, NULL,
                                   &createOpts);
  if (rc != MQTTASYNC_SUCCESS) {
    if (client != NULL)
      free(client);
    free(context);
    return sa_raise_errormsg(MQTTAsync_strerror(rc), opts_record);
  }

  rc = MQTTAsync_setCallbacks(*client, context, NULL, messageArrived, NULL);
  if (rc != MQTTASYNC_SUCCESS) {
    MQTTAsync_destroy(client);
    free(context);
    return sa_raise_errormsg(MQTTAsync_strerror(rc), opts_record);
  }

  rc = mqtt_connect(client, &opts);
  if (rc != MQTTASYNC_SUCCESS) {
    MQTTAsync_destroy(client);
    free(context);
    return sa_raise_errormsg(MQTTAsync_strerror(rc), opts_record);
  }
  sa_datapump *pump = sa_datapump_create();
  pump->create_item_fn = make_mqtt_binary;
  pump->get_flow_name_fn = get_topic_name;
  pump->free_pump_fn = free_mqtt_client;
  pump->pump_name = dname;
  pump->buffer_size = SA_MQTT_CIRC_BUFF_SIZE;
  pump->item_size = sizeof(sa_mqtt_buff_item);
  pump->context = context;
  context->pump = pump;
  return sa_datapump_init(pump);
}

EXPORT void a_initialize_extension(void *xa) {
  extfunction3("mqtt-publish", mqtt_publishfn);
  extfunction2("mqtt-subscribe", mqtt_subscribefn);
  extfunction2("mqtt-unsubscribe", mqtt_unsubscribefn);
  extfunction2("mqtt-register-client", mqtt_register_clientfn);
}
