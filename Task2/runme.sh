#!/bin/bash

make 1>>/dev/null

rm test.lck 2>>/dev/null
rm result.txt 2>>/dev/null

count=10
pids=()
for (( i=0; i<$count; i++ ))
do
	./myprogram test &
	pids+=($!)
done

sleep 300

completedTasks=0
for pid in ${pids[@]}
do
	kill -SIGINT $pid
	if [[ $? -eq 0 ]]
	then
		((completedTasks++))
	fi
done

echo "Number of running processes: " $count >> result.txt
echo "Number of successfully completed processes: " $completedTasks >> result.txt

cat stat >> result.txt

rm stat 