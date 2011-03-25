cd java
java phpsocks5.Deploy
if [ ! -a deploy.tmp ]
then
	rm deploy.tmp
else
	echo There\'s no Java installed detected.  Please install Java.
	exit
fi
