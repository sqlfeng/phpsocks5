@echo off
cd java
if not exist phpsocks5.properties goto notdeploy
start cmd /k polipo -c polipo.conf
echo Started.
java phpsocks5.PhpSocks5 > socks5out.log 2> socks5err.log
goto runend
:notdeploy
echo Please run deploy first./ÇëÏÈÔËĞĞdeploy¡£
:runend
pause