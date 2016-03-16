#XXX: http://stackoverflow.com/questions/2702564/how-can-i-quickly-sum-all-numbers-in-a-file
awk '{ sum += $1 } END { print sum }' $1

