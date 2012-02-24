<?php
$secretkey = "gnuwisy78346g86s786d87f6782hjdkhkjchzxkjhkdjhdfhi2uq3yrsyidyfuishyidhyichyizxihyiuhyfiu89347979834ghe987t898d7uf897s89j";
$debuginfo = TRUE;	//	TRUE or FALSE

$version = "04";

$localhost = 'localhost';
$localport = 80;
$hostheader = 'localhost';

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
	error_log(date(DATE_RFC1123) . "\t" . $message . "\n", 3, $dbprefix . "log.log");
}

function phpsocks5_http_500($errmsg)
{
	header('HTTP/1.1 500');
	echo phpsocks5_encrypt(date(DATE_RFC1123) . "\t" . $errmsg);
	phpsocks5_log("http_500_" . $errmsg);
	exit;
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
	phpsocks5_log("background process 1 $phpsid");
	$sess_cookie = substr($postdata, 3);
	$local_socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
	if(!$local_socket)
		phpsocks5_http_500('socket_create error');
	if(!socket_bind($local_socket, "127.0.0.1"))
		phpsocks5_http_500('socket_bind error');
	if(!socket_getsockname($local_socket, $internal_addr, $internal_port))
		phpsocks5_http_500('socket_getsockname error');
	fsockopen($localhost, $localport);
	while(true)
	{
		$noop = true;
		phpsocks5_log("background process 7 $phpsid");
		if(feof($rmtskt))
			phpsocks5_http_500('feof');
		phpsocks5_log("background process 8 $phpsid");
		$cnt = fread($rmtskt, 1048576);
		phpsocks5_log("background process 9 $phpsid");
		if($cnt)
		{
			phpsocks5_log("background process 10 $phpsid fread: " . phpsocks5_tohex($cnt));
			if(!mysql_query("INSERT INTO ${dbprefix}recving (sid, cnt) VALUES ('" . $phpsid . "', '" . base64_encode($cnt) . "')"))
				phpsocks5_http_500('mysql_query INSERT error');
			phpsocks5_usleep(0);
			phpsocks5_log("background process 11 $phpsid");
			$noop = false;
		}
		phpsocks5_log("background process 12 $phpsid");
		phpsocks5_usleep($inv);
		phpsocks5_log("background process 13 $phpsid");
		$rslt = mysql_query("SELECT id, cnt FROM ${dbprefix}sending WHERE sid = '" . $phpsid . "' ORDER BY id ASC LIMIT 1");
		if(!$rslt)
			phpsocks5_http_500('mysql_query SELECT error');
		$row = mysql_fetch_row($rslt);
		phpsocks5_usleep(0);
		phpsocks5_log("background process 14 $phpsid");
		if($row)
		{
			$noop = false;
			phpsocks5_log("background process 15 $phpsid");
			mysql_query("DELETE FROM ${dbprefix}sending WHERE id = $row[0]");
			phpsocks5_usleep(0);
			phpsocks5_log("background process 16 $phpsid");
			if(!$row[1])
				phpsocks5_http_500('break');
			$cnt = base64_decode($row[1]);
			phpsocks5_log("background process 17 $phpsid fwrite: " . phpsocks5_tohex($cnt));
			if(!fwrite($rmtskt, $cnt))
				phpsocks5_http_500('fwrite error');
			phpsocks5_log("background process 18 $phpsid");
		}
		if($noop)
		{
			phpsocks5_log("background process 19 $phpsid");
			$inv += $invstep;
			if($inv > $invmax)
				$inv = $invmax;
		}
		else
		{
			phpsocks5_log("background process 20 $phpsid");
			set_time_limit(30);
			phpsocks5_log("background process 21 $phpsid");
			$inv = 0;
		}
		phpsocks5_usleep($inv);
		phpsocks5_log("background process 22 $phpsid");
	}
}
elseif($postdata[2] == "3")
{
	phpsocks5_log("send process 1 $phpsid");
	if(!mysql_query("INSERT INTO ${dbprefix}sending (sid, cnt) VALUES ('" . $phpsid . "', '" . base64_encode(substr($postdata, 3)) . "')"))
		phpsocks5_http_500('mysql_query INSERT INTO error');
}
elseif($postdata[2] == "4")
{
	phpsocks5_log("receive process 1 $phpsid");
	$inv = 0;
	while(true)
	{
		phpsocks5_log("receive process 2 $phpsid");
		$rslt = mysql_query("SELECT id, cnt FROM ${dbprefix}recving WHERE sid = '" . $phpsid . "' ORDER BY id ASC LIMIT 1");
		if(!$rslt)
			phpsocks5_http_500('mysql_query SELECT error');
		phpsocks5_log("receive process 3 $phpsid");
		$row = mysql_fetch_row($rslt);
		phpsocks5_usleep(0);
		if($row)
		{
			phpsocks5_log("receive process 4 $phpsid");
			mysql_query("DELETE FROM ${dbprefix}recving WHERE id = $row[0]");
			phpsocks5_usleep(0);
			if($row[1])
			{
				$cnt = base64_decode($row[1]);
				phpsocks5_log("receive process 5 $phpsid echo: " . phpsocks5_tohex($cnt));
				echo $prefix . phpsocks5_encrypt($cnt) . $postfix;
			}
			else
				phpsocks5_http_500('break');
			phpsocks5_log("receive process 6 $phpsid");
			break;
		}
		phpsocks5_log("receive process 7 $phpsid");
		$inv += $invstep;
		if($inv > $invmax)
			$inv = $invmax;
		phpsocks5_log("receive process 8 $phpsid");
		phpsocks5_usleep($inv);
		phpsocks5_log("receive process 9 $phpsid");
	}
}
elseif($postdata[2] == "5")
{
	phpsocks5_log("close process 1 $phpsid");
	mysql_query("INSERT INTO ${dbprefix}sending (sid, cnt) VALUES ('" . $phpsid . "', '')");
	mysql_query("INSERT INTO ${dbprefix}recving (sid, cnt) VALUES ('" . $phpsid . "', '')");
	phpsocks5_log("close process 2 $phpsid");
}

phpsocks5_log("process 7");

mysql_close();
?>