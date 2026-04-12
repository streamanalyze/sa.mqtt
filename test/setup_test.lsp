(defun start-producer(args)
  "Start sa.engine MQTT-producer"
  (cond ((memq (os) '(linux osx))
         (start-program (concat "sa.engine " args)))
        (t (start-program (concat "sa.engine " args)))))

(start-producer "-O test/producer1.osql > /dev/null")
(start-producer "-O test/producer2.osql > /dev/null")
(start-producer "-O test/producer3.osql > /dev/null")