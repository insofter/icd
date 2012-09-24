<?php
session_start();

define( 'INSOFTER', 1 );

$o_mnie=array('nazwa'=>'ICDTCP3',
	'opis'=>'Interfejs konfiguracyjny rejestratora ICDTCP3',
 	'wersja_rok'=>'2012',
	'wersja_miesiac'=>'07',
	'autor'=>'insofter',
	'strona'=>'www.insofter.pl');

require_once('inc/c_icdtcp.php');

$naglowek='<!DOCTYPE html><html><head>
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<meta name="Keywords" content="">
<title>';
$tytul=$o_mnie['nazwa'];
$naglowekcd='</title>
<link rel="stylesheet" href="css/icdtcp.css" type="text/css">';


$logo='<h1>
<img src="img/insofter.jpg" width="716" height="96" alt="Insofter Komputerowe Systemy Automatyki">
</h1>';

$stopka='
<p id="stopka">
Prosimy o nie dokonywanie zmian w konfiguracji bez konsultacji z administratorem.<br><br>
<a href="http://'.$o_mnie['strona'].'">&copy; '.$o_mnie['wersja_rok'].' '.$o_mnie['autor'].'</a>
</p>
</body></html>';

$menulista=array('tcpip'=>'TCPIP',
	'wpa'=>'WiFi',
	'licznik'=>'Licznik',
	'wysylanie'=>'Wysyłanie',
	'wyniki'=>'Wyniki',
	'log'=>'Logi',
	'plik_konf'=>'Plik konf.'
	);

$menu='<ul class="menu">
';
if( isset($_GET['strona']) ) {
	$menu.='<li><a href="./">Info</a></li>';
	$tytul.=' - '.$menulista[$_GET['strona']];
} else {
	$menu.='<li><a id="klikniete" href="./">Info</a></li>';
}
foreach( $menulista as $key=>$val )
{
	if( isset($_GET['strona']) && $_GET['strona']==$key )
	{
		$menu.='
<li><a id="klikniete" href="./?strona='.$key.'">'.$val.'</a></li>';
	} else {
		$menu.='
<li><a href="./?strona='.$key.'">'.$val.'</a></li>';
	}
}
$menu.='</ul>
';

if( isset($_GET['strona']) ) {
	switch( $_GET['strona'] ) {
	        case 'tcpip':
	                include('inc/tcpip.php');
	                break;
	        case 'licznik':
	                include('inc/licznik.php');
	                break;
	        case 'wysylanie':
	                include('inc/wysylanie.php');
	                break;
	        case 'wyniki':
	                include('inc/wyniki.php');
	                break;
	        case 'log':
	                include('inc/log.php');
	                break;
   	        case 'plik_konf':
	                include('inc/plik_konf.php');
	                break;
   	        case 'wpa':
	                include('inc/wpa.php');
	                break;
          case 'test_wysylania':
                include('inc/test_wysylania.php');
                break;
	        default:
	                $tresc='<div id="tresc">taka strona nie istnieje</div>';
	                break;
         }
 } else {
	include('inc/info.php');
 }

echo($naglowek);
echo($tytul);
echo($naglowekcd);
echo('</head><body>');
echo($logo);
echo($menu);
echo($tresc);
echo($stopka);
