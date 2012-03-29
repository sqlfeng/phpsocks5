<?php
$secretkey = "gnuwisy78346g86s786d87f6782hjdkhkjchzxkjhkdjhdfhi2uq3yrsyidyfuishyidhyichyizxihyiuhyfiu89347979834ghe987t898d7uf897s89j";
$debuginfo = TRUE;	//	TRUE or FALSE
$send_buf_cnt = 8;
$recv_buf_cnt = 8;

$localhost = 'localhost';
$localport = 80;
$localurl = '/socks5.php';
$hostheader = 'localhost';

$version = "10";

function phpsocks5_encrypt($datastr)
{
	global $secretkey;
	$encrypted = '';
	for($i = 0; $i < strlen($datastr); $i++)
		$encrypted .= chr(ord($datastr[$i]) ^ ord($secretkey[$i % strlen($secretkey)]));
	return $encrypted;
}

function phpsocks5_decrypt($datastr)
{
	return phpsocks5_encrypt($datastr);
}

function phpsocks5_tohex($datastr)
{
	global $debuginfo;
	if(!$debuginfo)
		return '';
	$hexstr = bin2hex($datastr);
	$hexstr .= '(';
	for($i = 0; $i < strlen($datastr); $i++)
	{
		if($datastr[$i] < ' ' || $datastr[$i] > '~')
			$hexstr .= '.';
		else
			$hexstr .= $datastr[$i];
	}
	$hexstr .= ')';
	return $hexstr;
}

function phpsocks5_log($message)
{
	global $dbprefix;
	global $debuginfo;
	if(!$debuginfo)
		return;
	error_log(date(DATE_RFC1123) . "\t" . $message . "\n", 3, "phpsocks5_log.log");
}

function phpsocks5_http_500($errmsg)
{
	header('HTTP/1.1 500');
	echo phpsocks5_encrypt(date(DATE_RFC1123) . "\t" . $errmsg);
	phpsocks5_log("http_500_" . $errmsg);
	exit;
}

function socket_connect_hostname($socket, $hostname, $port)
{
	$ips = gethostbynamel($hostname);
	foreach($ips as $ip)
	{
		if(socket_connect($socket, $ip, $port))
			return TRUE;
	}
	return FALSE;
}

phpsocks5_log("process 1");

set_time_limit(60);

phpsocks5_log("process 2");

$postdata = file_get_contents("php://input");

phpsocks5_log("process 3 before decrypt postdata: " . phpsocks5_tohex($postdata));

$postdata = phpsocks5_decrypt($postdata);

phpsocks5_log("process 4 after decrypt postdata: " . phpsocks5_tohex($postdata));

if(!$postdata)
	exit;

phpsocks5_log("process 5");

if($postdata[0] != $version[0] || $postdata[1] != $version[1])
	phpsocks5_http_500('version not match');

phpsocks5_log("process 6");

if($postdata[2] == "1")
{
	phpsocks5_log("start session process 1");
	if(!session_start())
		phpsocks5_http_500('start session error');
	phpsocks5_log("start session process 2");
}
elseif($postdata[2] == "2")
{
	phpsocks5_log("background process 1");
	$sess_cookie = substr($postdata, 3);
	$local_socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
	if(!$local_socket)
		phpsocks5_http_500('background process socket_create 1 error');
	if(!socket_bind($local_socket, "127.0.0.1"))
		phpsocks5_http_500('background process socket_bind error');
	if(!socket_getsockname($local_socket, $internal_addr, $internal_port))
		phpsocks5_http_500('background process socket_getsockname error');
	$httpsocket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	if(!$httpsocket)
		phpsocks5_http_500('background process socket_create 2 error');
	if(!socket_connect_hostname($localhost, $localport))
		phpsocks5_http_500('background process socket_connect_hostname local error');
	$localport = phpsocks5_encrypt($version . "3" . "$localport");
	$localportlen = strlen($localport);
	$store_port_req = "POST $localurl HTTP/1.1\r\nHost: $hostheader\r\nCookie: $sess_cookie\r\nContent-Length: $localportlen\r\n\r\n$localport";
	if(!socket_send($httpsocket, $store_port_req, strlen($store_port_req), 0))
		phpsocks5_http_500('background process socket_send error');
	if(!socket_recvfrom($local_socket, $buf, 65535, 0, $peer_host, $peer_port))
		phpsocks5_http_500('background process socket_recvfrom 1 error');
	$remote_host = strtok($buf, ":");
	$remote_port = strtok(":") / 1;
	phpsocks5_log("background process 2 $remote_host $remote_port");
	$remote_socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	if(!$remote_socket)
		phpsocks5_http_500('background process socket_create 3 error');
	if(!socket_connect_hostname($remote_socket, $remote_host, $remote_port))
		phpsocks5_http_500('background process socket_connect_hostname remote error');
	if(!socket_sendto($local_socket, '1', 1, 0, $peer_host, $peer_port))
		phpsocks5_http_500('background process socket_sendto peer error');
	$send_bufque = array();
	$send_peer_host = NULL;
	$send_peer_port = 0;
	$recv_bufque = array();
	$recv_peer_host = NULL;
	$recv_peer_port = 0;
	$remote_closed = FALSE;
	while(TRUE)
	{
		phpsocks5_log("background process 3");
		$read_sockets = array($local_socket);
		if(!$remote_closed && count($recv_bufque) < $recv_buf_cnt)
			$read_sockets[] = $remote_socket;
		$write_sockets = array();
		if(!$remote_closed && count($send_bufque) > 0)
			$write_sockets[] = $remote_socket;
		$except_sockets = array();
		if(!socket_select($read_sockets, $write_sockets, $except_sockets, NULL))
			hpsocks5_http_500('background process socket_select error');
		foreach($read_sockets as $read_socket)
		{
			$buf = NULL;
			if($read_socket === $local_socket)
			{
				if(!socket_recvfrom($local_socket, $buf, 65535, 0, $peer_host, $peer_port))
					phpsocks5_http_500('background process socket_recvfrom 2 error');
				if($buf[0] == "1")
				{
					$send_peer_host = $peer_host;
					$send_peer_port = $peer_port;
					$buf = substr($buf, 1);
					$send_bufque[] = $buf;
					if($remote_closed)
					{
						if(!socket_sendto($local_socket, '0', 1, 0, $peer_host, $peer_port))
							phpsocks5_http_500('background process socket_sendto 1 error');
						$send_peer_port = 0;
					}
					elseif(count($send_bufque) < $send_buf_cnt)
					{
						if(!socket_sendto($local_socket, '1', 1, 0, $peer_host, $peer_port))
							phpsocks5_http_500('background process socket_sendto 2 error');
						$send_peer_port = 0;
					}
				}
				elseif($buf[0] == "2")
				{
					$recv_peer_host = $peer_host;
					$recv_peer_port = $peer_port;
					if(count($recv_bufque) > 0)
					{
						while(count($recv_bufque) > 0)
						{
							$buf = '1' . array_shift($recv_bufque);
							if(!socket_sendto($local_socket, $buf, strlen($buf), 0, $peer_host, $peer_port))
								phpsocks5_http_500('background process socket_sendto 3 error');
						}
						if($remote_closed)
						{
							if(!socket_sendto($local_socket, '0', 1, 0, $peer_host, $peer_port))
								phpsocks5_http_500('background process socket_sendto 4 error');
						}
						else
						{
							if(!socket_sendto($local_socket, '1', 1, 0, $peer_host, $peer_port))
								phpsocks5_http_500('background process socket_sendto 5 error');
						}
						$recv_peer_port = 0;
					}
					elseif($remote_closed)
					{
						if(!socket_sendto($local_socket, '0', 1, 0, $peer_host, $peer_port))
							phpsocks5_http_500('background process socket_sendto 6 error');
						$recv_peer_port = 0;
					}
				}
			}
			elseif($read_socket === $remote_socket)
			{
				if(!socket_recv($remote_socket, $buf, 65000, 0))
				{
					$remote_closed = TRUE;
					break;
				}
				$recv_bufque[] = $buf;
				if($recv_peer_port != 0)
				{
					while(count($recv_bufque) > 0)
					{
						$buf = '1' . array_shift($recv_bufque);
						if(!socket_sendto($local_socket, $buf, strlen($buf), 0, $peer_host, $peer_port))
							phpsocks5_http_500('background process socket_sendto 7 error');
					}
					if($remote_closed)
					{
						if(!socket_sendto($local_socket, '0', 1, 0, $peer_host, $peer_port))
							phpsocks5_http_500('background process socket_sendto 8 error');
					}
					else
					{
						if(!socket_sendto($local_socket, '1', 1, 0, $peer_host, $peer_port))
							phpsocks5_http_500('background process socket_sendto 9 error');
					}
					$recv_peer_port = 0;
				}
			}
		}
		foreach($write_sockets as $write_socket)
		{
			if($write_socket === $remote_socket)
			{
				$buf = array_shift($send_bufque);
				if(!socket_send($remote_socket, $buf, strlen($buf), 0))
				{
					$remote_closed = TRUE;
					break;
				}
				$recv_peer_port = 0;
			}
		}
	}
	phpsocks5_log("background process 4");
}
elseif($postdata[2] == "3")
{
	phpsocks5_log("saveport process 1");
	if(!session_start())
		phpsocks5_http_500('saveport process session_start error');
	$_SESSION['port'] = substr($postdata, 3);
	phpsocks5_log("saveport process 2");
}
elseif($postdata[2] == "4")
{
	phpsocks5_log("readport process 1");
	if(!session_start())
		phpsocks5_http_500('readport process session_start error');
	echo phpsocks5_decrypt($_SESSION['port']);
	phpsocks5_log("readport process 2");
}
elseif($postdata[2] == "5")
{
	phpsocks5_log("connect process 1");
	$localport = strtok(substr($postdata, 3), ':') / 1;
	$remotehost = strtok(':');
	$remoteport = strtok(':');
	phpsocks5_log("connect process 2");
	$local_socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
	if(!$local_socket)
		phpsocks5_http_500('connect process socket_create error');
	$buf = "$remotehost:$remoteport";
	if(!socket_sendto($local_socket, $buf, strlen(buf), 0, '127.0.0.1', $localport))
		phpsocks5_http_500('connect process socket_sendto error');
	$buf = NULL;
	if(!socket_recvfrom($local_socket, $buf, 65500, 0, $peer_host, $peer_port))
		phpsocks5_http_500('connect process socket_recvfrom error');
	echo phpsocks5_decrypt($buf);
	phpsocks5_log("connect process 3");
}
elseif($postdata[2] == "6")
{
	phpsocks5_log("send process 1");
	$buf = substr($postdata, 3);
	$port = strtok($buf, ';');
	$buf = substr($buf, strlen($post) + 1);
	$port = $port / 1;
	$local_socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
	if(!$local_socket)
		phpsocks5_http_500('send process socket_create error');
	while(strlen($buf) > 0)
	{
		$packet = '1' . substr($buf, 0, 65000);
		$buf = substr($buf, 65000);
		if(!socket_sendto($local_socket, $buf, strlen($buf), 0, '127.0.0.1', $port))
			phpsocks5_http_500('send process socket_sendto error');
		$buf = NULL;
		if(!socket_recvfrom($local_socket, $buf, 65500, 0, $peer_host, $peer_port))
			phpsocks5_http_500('send process socket_recvfrom error');
		if($buf[0] != "1")
			phpsocks5_http_500('send process eof');
	}
}
elseif($postdata[2] == "7")
{
	phpsocks5_log("recv process 1");
	$port = substr($postdata, 3) / 1;
	$local_socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
	if(!$local_socket)
		phpsocks5_http_500('recv process socket_create error');
	while(TRUE)
	{
		if(!socket_sendto($local_socket, '2', 1, 0, '127.0.0.1', $port))
			phpsocks5_http_500('recv process socket_sendto error');
		$buf = NULL;
		if(!socket_recvfrom($local_socket, $buf, 65500, 0, $peer_host, $peer_port))
			phpsocks5_http_500('recv process socket_recvfrom error');
	}
}
elseif($postdata[2] == "8")
{
	phpsocks5_log("close process 1");
}

phpsocks5_log("process 7");

?>