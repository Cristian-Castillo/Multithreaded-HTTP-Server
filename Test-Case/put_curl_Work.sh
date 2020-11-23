#!/bin/bash

array0=(1 2 3 4 5 6)

for i in "${array0[@]}";do
	curl -T "$i" http://localhost:8080/111111111"$i" -v > cris"$i"
    curl -T "$i" http://localhost:8080/222222222"$i" -v > cris"$i"
    curl -T "$i" http://localhost:8080/123456789"$i" -v > cris"$i"
    curl -T "$i" http://localhost:8080/123456789"$i" -v > cris"$i"
    curl -T "$i" http://localhost:8080/123456789"$i" -v > cris"$i"
    curl -T "$i" http://localhost:8080/123456789"$i" -v > cris"$i"
    curl -T "$i" http://localhost:8080/123456789"$i" -v > cris"$i"
    curl -T "$i" http://localhost:8080/123456789"$i" -v > cris"$i"
    curl -T "$i" http://localhost:8080/123456789"$i" -v > cris"$i"
    curl -T "$i" http://localhost:8080/123456789"$i" -v > cris"$i"


done

