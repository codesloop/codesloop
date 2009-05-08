#!/bin/sh
for i in `find . -name "t__*cc" | sed 's,.cc$,,g'`
do
	echo $i
	$i
done

