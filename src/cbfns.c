#include "MQTTAsync.h"
void onDisconnect(void *context, MQTTAsync_successData *response)
{
    //TRACE("");
    //TODO disconnect
}

void onSubscribe5(void *context, MQTTAsync_successData5 *response)
{
    //TRACE("");
    //TODO notify subscribe ok
}

void onSubscribe(void *context, MQTTAsync_successData *response)
{
    //TRACE("");
    //TODO notify subscribe ok
}

void onSubscribeFailure5(void *context, MQTTAsync_failureData5 *response)
{
    //TRACE("");
    //TODO notify subscribe failure
}

void onSubscribeFailure(void *context, MQTTAsync_failureData *response)
{
    //TRACE("");
    //TODO notify subscribe failure
}

void onUnSubscribe5(void *context, MQTTAsync_successData5 *response)
{
    //TRACE("");
    //TODO notify subscribe ok
}

void onUnSubscribe(void *context, MQTTAsync_successData *response)
{
    //TRACE("");
    //TODO notify subscribe ok
}

void onUnSubscribeFailure5(void *context, MQTTAsync_failureData5 *response)
{
    //TRACE("");
    //TODO notify subscribe failure
}

void onUnSubscribeFailure(void *context, MQTTAsync_failureData *response)
{
    //TRACE("");
    //TODO notify subscribe failure
}

void onConnectFailure5(void *context, MQTTAsync_failureData5 *response)
{
    //TRACE("");
    //TODO notify connection failure
}

void onConnectFailure(void *context, MQTTAsync_failureData *response)
{
    //TRACE("");
    //TODO notify connection failure
}


void onConnect5(void *context, MQTTAsync_successData5 *response)
{
    //TRACE("");
    //TODO notify that we are connected
}

void onConnect(void *context, MQTTAsync_successData *response)
{
    //TRACE(""); 
    //TODO notify that we are connected.
}

void onPublishFailure5(void *context, MQTTAsync_failureData5 *response)
{
    //TRACE("");
    //TODO set published false
}

void onPublishFailure(void *context, MQTTAsync_failureData *response)
{
    //TRACE("");
    //TODO set published false
}

void onPublish5(void *context, MQTTAsync_successData5 *response)
{
    //TRACE("");
    //TODO keep track of publish
}

void onPublish(void *context, MQTTAsync_successData *response)
{
    //TRACE("");
    //TODO keep track of publish
}