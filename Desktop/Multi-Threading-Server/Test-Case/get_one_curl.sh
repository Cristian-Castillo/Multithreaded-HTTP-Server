#!/bin/bash
curl http://localhost:8080 --request-target FILENAME00 > cmd0.output &
curl http://localhost:8080 --request-target FILENAME01 > cmd1.output &
curl http://localhost:8080 --request-target FILENAME02 > cmd2.output &
curl http://localhost:8080 --request-target FILENAME03 > cmd3.output &
curl http://localhost:8080 --request-target FILENAME04 > cmd4.output &
curl http://localhost:8080 --request-target FILENAME05 > cmd5.output &
curl http://localhost:8080 --request-target FILENAME06 > cmd6.output &
curl http://localhost:8080 --request-target FILENAME07 > cmd7.output &
curl http://localhost:8080 --request-target FILENAME08 > cmd8.output &
curl http://localhost:8080 --request-target FILENAME09 > cmd9.output &

