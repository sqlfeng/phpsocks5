现在的使用PHP空间做代理服务器的程序有很多限制，最主要是不能完美支持HTTPS。

大多数自称完美支持HTTPS的使用PHP空间做代理服务器的程序（比如apjp）仍旧需要导入HTTPS的SSL证书才能让浏览器不提示证书错误，降低了HTTPS的安全性。其中还有部分程序需要PHP空间支持Mcrypt，大多数PHP空间都不支持Mcrypt。

还有少数程序（比如hyk-proxy中的PHP部分）可以完美支持HTTPS，无须导入HTTPS证书，但需要外网环境使用，或者在路由器中设置端口映射，或者需要开启路由器的UPnP。

本程序没有上述限制，完美支持HTTPS，无须PHP空间支持Mcrypt，无须外网环境使用，也无须在路由器上设置端口映射，也无须开启路由器的UPnP。

由于本程序是Socks5类型的代理，所以不仅可以代理HTTP和HTTPS协议，甚至可以代理SSH、POP3、FTP（被动模式）等协议。如果需要HTTP(S)类型的代理服务器，可以结合本程序使用Polipo等程序转换为HTTP(S)类型的代理服务器。

本程序相比其他使用PHP空间做代理服务器的程序唯一的要求是需要MySQL数据库的支持。

本代理程序的网络数据传输采用简单异或加密，但足以绕过机器审查。

<font color='red'>本程序客户端需要Java运行环境支持。可以去 <a href='http://www.java.com/'>http://www.java.com/</a> 下载安装Java。</font>

<font color='red'>本程序提供的是socks5类型的代理服务器，请确保客户端软件设置和使用的是socks5类型的代理服务器，而不是http类型的代理服务器。如果需要http类型的代理服务器，请搭配polipo转换为http代理使用。</font>

<font color='red' size='4'>建议在调试成功后手动将php中的debuginfo参数设置为False，并删除服务器端的log文件。debuginfo参数的具体意义可以参考Wiki页中的parameter_list页。</font>

使用方法：
```
0、0.5版本开始不用按照以下6个步骤使用了，只需要运行deploy按照向导操作即可。之后运行run_with_polipo或run_without_polipo。推荐自己配置polipo。

1、修改socks5.php前5行代码的数据库配置，修改为PHP虚拟主机提供的数据库配置。

2、修改socks5.php的第7行secretkey变量，修改为一个有大小写英文字母和数字的随机密钥，最好不要有符号，长度建议超过4096个字节。

3、将socks5.php随机改名（扩展名不要变），然后传到服务器上去。

4、打开浏览器，访问这个php页（比如 http://www.myhostingspace.com/abc.php ），如果出现Create tables successfully，说明php配置正确（第二次访问会出现Create table N error，是正常情况，因为数据表已经存在了）。

5、修改Java目录下的phpsocks5.properties，serverurl修改为上述浏览器访问的地址（比如 http://www.myhostingspace.com/abc.php ），secretkey修改为socks5.php中的secretkey变量中的内容。

6、在Java目录下运行java phpsocks5.PhpSocks5（1、编译PhpSocks5.java后运行。2、确保在Java目录下运行，比如Java文件所在的目录结构为D:\myprojects\phpsocks5\java\phpsocks5\PhpSocks5.java，那么需要在D:\myprojects\phpsocks5\java中运行java phpsocks5.PhpSocks5），之后本地127.0.0.1的10080端口就是socks5代理。
```

如果想参与本项目，请联系我，也就是本项目的Owner。现在还需做的工作有：Windows和Linux环境程序打包、命令行和图形界面的向导方式部署、宣传、代码优化等工作。

源代码下载可以在本页中的External Links中找到。

# 现在可以在Downloads中下载到打包好的程序了。使用之前按照上述6步操作（打包好的程序省去了编译Java的过程，直接运行run.bat，然后按照前5步操作即可）。不管是否能用希望大家多多反馈，可以在Issues中反馈，说明一下情况和环境（比如PHP是运行在Apache还是IIS上的，是Windows服务器还是Linux服务器等情况）。有人提BUG我就会尽力去解决 #

# 更新： #

# 2011年2月8日：项目启动。0.1版本。 #


---


# 2011年2月22日：0.2版本。增加了服务器端log，增加了客户端log，修复了有可能锁数据库的bug。如果还无法使用，请将客户端和服务器端的log发到Issues中，谢谢大家帮忙测试。服务器端log会在php文件的同目录中生成phpsocks5\_log.log文件。 #


---


# 2011年3月2日：0.2.5版本。增加了更多的log。如果无法正常使用本程序，把客户端和服务器端的log都发到Issues中。客户端和服务器端的log要一起发上来，方便查找问题。 #


---


# 2011年3月5日：0.4版本。修复了大部分提示下载、空白页面、图片显示不全等问题。欢迎测试并反馈问题。反馈问题请描述问题具体现象，同时请附上客户端和服务器端的log文件。另外建议secretkey超过4096字节（下载后的压缩包中secretkey只有几百字节，太短），这样才能比较安全。感谢大家一直以来的支持！ #


---


# 2011年3月10日：增加了一个wiki页面，搜集各个php虚拟主机空间运行phpsocks5的情况。大家可以将自己的情况写到wiki页面中的comments中，我会整理到wiki页面中。 #


---


# 2011年3月12日：发现大多数不稳定为Firefox使用socks5类型的代理服务器时超时太短造成的，所以推荐使用的时候搭配polipo转换为http(s)代理，推荐IE、Firefox和Chrome都使用polipo转换后的http(s)代理。发布0.4.2版和0.4.2p版，0.4.2版不带polipo，使用时需要自己配置polipo，配置方法可以参考0.4.2p版；0.4.2p版自带polipo，使用时设置浏览器的http(s)代理地址为127.0.0.1、端口10088。如果会自己配置polipo，推荐使用0.4.2版自己配置polipo。 #


---


# 2011年3月17日：增加了一个wiki页面，详细解释了客户端和服务器端常用和不常用的参数作用。高级用户可以参照此wiki页修改参数。 #


---


# 2011年3月19日：0.4.3版本。有Polipo和无Polipo版本合二为一，需要Polipo的运行run\_with\_polipo.bat，不需要Polipo或自己配置Polipo的运行run\_without\_polipo.bat。推荐自己配置Polipo。增加了php服务器端debuginfo参数，设为False后服务器端不会显示任何调试信息，也不会输出任何log。调试正常后可将debuginfo设为False。增加了更多的客户端log，如需反馈，可以使用此版本返回log。反馈时同时带上服务器端log。 #


---


# 2011年3月25日：0.5版本。增加了首次部署程序，运行deploy后根据向导操作即可，会修改php中的配置也会生成客户端的配置文件。Linux用户运行run\_with\_polipo.sh之前先确保已经安装了Polipo，zip包中的Polipo只能在Windows下使用，Linux用户请自行安装。 #


---


# 2011年5月3日：0.5.4版本。增加php服务器和Java客户端之间的gzip压缩功能，减少流量；由于某些广告空间会丢失数据，但content-type为image时不会加入广告，也就不会丢失数据，所以php返回类型改为image/png；由于PHP会在多请求同时执行时会卡在start\_session()永远不会往下执行的BUG，所以无法将PHP的cookie PHPSESSID改回start\_session()，不知有谁能帮助解决此问题；客户端改为模拟XP IE6 with .NET 2/3/3.5，如果防火墙过滤此类User-Agent也就过滤了大部分IE6用户访问国外网站了，如果真如此，也算为了kill IE6贡献一部分力量吧。 #


---


# 2011年6月10日：0.6版本。加大了php服务器端的缓冲区，加快了速度。具体从0.5.4版上进行升级的方法详见下载页面。 #


---


# 2011年8月15日：0.6版的SP1补丁。修复了在某些情况下不稳定的情况。使用0.6版的用户请使用此补丁改善稳定性问题。使用方法详见下载区。 #


---


# 预告下次版本更新内容：1、部署程序增加Content-Type、$sesscookiekey、Host和User-Agent的修改，能增加删除HTTP Request Property和HTTP Response Header；2、Java客户端支持通过HTTP/HTTPS/Socks代理服务器与PHP服务器端进行连接并支持代理服务器认证（普通用户名+口令方式、NTLM认证方式等）；3、图形化界面；4、由于某些空间对cookie支持不好，取消使用cookie，转而将session id放在post请求数据中发给服务器 #


---


<font color='red' size='4'>大家要多多反馈啊，看到有很多下载，但不管是能否使用反馈的都不多，你的举手之劳能改善这个软件，并且软件改善之后也会使你受益。大家还是要多多反馈哦，我自己的可测试php空间不多，很多时候就需要大家帮忙反馈了，而且现在能在wiki页面中反馈各个php虚拟主机运行phpsocks5的情况了。</font>