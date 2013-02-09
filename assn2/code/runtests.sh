#!/bin/bash
for i in {7..7}
do
    for j in {1,2,4,8,16}
    do
        echo "./bankdriver -w$j -t$i -y running"
        ./bankdriver -w$j -t$i -y5
    done
done
