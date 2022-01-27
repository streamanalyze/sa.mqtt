(load-extension "sa_mqtt" t)


(defun register-mqtt-client--+ (fno name opts r)
"Function that registers a new external MQTT pubsub 
with NAME and OPTS"
  (register-ext-pubsub name opts 
     'mqtt-register-client nil 
     'mqtt-publish
     'mqtt-subscribe 'mqtt-unsubscribe )
  (osql-result name opts name))
