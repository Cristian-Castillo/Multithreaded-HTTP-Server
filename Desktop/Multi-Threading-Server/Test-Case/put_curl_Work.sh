#!/bin/bash
curl -T 4 localhost:8080/1234567890 > cmd1.output &
curl -T 4 localhost:8080/1234567891 > cmd2.output &



