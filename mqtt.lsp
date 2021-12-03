(load-extension "sa_mqtt" t)


(defun register-mqtt-client--+ (fno name opts r)
  (register-ext-pubsub name opts 
     'mqtt-register-client nil 
     'mqtt-subscribe 'mqtt-unsubscribe 
     'mqtt-publish)
  (osql-result name opts name))
