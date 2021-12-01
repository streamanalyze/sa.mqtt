(defun start-producer(args)
  "Start sa.engine MQTT-producer"
  (cond ((memq (os) '(linux osx))
         (start-program (concat "sa.engine " args  " > /dev/null")))
        (t (start-program (concat "sa.engine " args)))))

(start-producer "-O test/producer1.osql")
(start-producer "-O test/producer2.osql")
