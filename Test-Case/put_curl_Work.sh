#!/bin/bash
curl -T 1 localhost:8080/1234567890 > cmd1.output &
curl -T 2 localhost:8080/1234567891 > cmd2.output &
curl -T 3 localhost:8080/1234567892 > cmd3.output &
curl -T 4 localhost:8080/1234567893 > cmd4.output &
curl -T 5 localhost:8080/1234567894 > cmd5.output &
curl -T 6 localhost:8080/1234567895 > cmd6.output &
curl -T 7 localhost:8080/1234567896 > cmd7.output &
curl -T 8 localhost:8080/1234567897 > cmd8.output &


