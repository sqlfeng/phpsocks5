@echo off
cd java
java phpsocks5.Deploy
if not exist deploy.tmp goto nojava
del deploy.tmp
goto deployend
:nojava
echo There's no Java installed detected.  Please install Java./没有检测到安装了Java，请安装Java。
:deployend
pause