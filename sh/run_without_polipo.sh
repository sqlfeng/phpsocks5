cd java
if [ -a phpsocks5.properties ]
then
	java phpsocks5.PhpSocks5 > socks5out.log 2> socks5err.log
else
	echo Please run deploy.sh first.
	exit
fi
