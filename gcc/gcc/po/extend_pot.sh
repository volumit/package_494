#! /bin/bash

source=$1
pot=$2

nl='
'

cat $source | while read line
do
	if [[ -n $line ]]
	then
		echo $nl >> $pot
		echo "#: manually_added_translation:1" >> $pot
		echo "#, gcc-internal-format" >> $pot
		echo "msgid \"$line\"" >> $pot
		echo "msgstr \"\"" >> $pot
	fi
done
