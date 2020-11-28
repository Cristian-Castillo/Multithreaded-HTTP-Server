#!/bin/bash

array0=(1 2 3 4 5 6)

for i in "${array0[@]}";do
	curl localhost:8080 --request-target 123456789"$i" -v > cris"$i" &

done



