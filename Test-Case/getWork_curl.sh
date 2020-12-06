#!/bin/bash

array0=(1 2 3 4 5 6 7 8)

for i in "${array0[@]}";do
	curl localhost:8080/123456789"$i" -v > curve_gods"$i" &

done