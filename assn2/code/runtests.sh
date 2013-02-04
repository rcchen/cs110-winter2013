#!/bin/bash
echo "Run with how many threads: "
read threads
for i in {1..7}
do
    ./bankdriver -w$threads -t$i
done
