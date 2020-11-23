#!/bin/bash

curl -T 8 http://localhost:8080 --request-target FILENAME00 > cmd0.output &
curl -T 8 http://localhost:8080 --request-target FILENAME02 > cmd1.output &
curl -T 8 http://localhost:8080 --request-target FILENAME03 > cmd2.output &
curl -T 8 http://localhost:8080 --request-target FILENAME04 > cmd3.output &
curl -T 8 http://localhost:8080 --request-target FILENAME05 > cmd4.output &
curl -T 8 http://localhost:8080 --request-target FILENAME06 > cmd5.output &
curl -T 8 http://localhost:8080 --request-target FILENAME07 > cmd6.output &
curl -T 8 http://localhost:8080 --request-target FILENAME08 > cmd7.output &
curl -T 8 http://localhost:8080 --request-target FILENAME09 > cmd8.output &
curl -T 8 http://localhost:8080 --request-target FILENAME01 > cmd9.output &
