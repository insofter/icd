<?php
session_start();

define( 'INSOFTER', 1 );

$o_mnie=array('nazwa'=>'ICDTCP3',
  'opis'=>'Interfejs konfiguracyjny rejestratora ICDTCP3',
  'wersja_rok'=>'2012',
  'wersja_miesiac'=>'11',
  'autor'=>'insofter',
  'strona'=>'www.insofter.pl');

require_once('inc/c_icdtcp.php');

$naglowek='<!DOCTYPE html><html lang="pl"><head>
  <meta http-equiv="content-type" content="text/html; charset=utf-8">
  <meta name="Keywords" content="">
  <title>';
$tytul=$o_mnie['nazwa'];
$naglowekcd='</title>
  <link rel="stylesheet" href="css/icdtcp.css" type="text/css">
  <link rel="stylesheet" href="css/jquery-ui.css">
  <script src="js/jquery-1.8.2.js"></script>
  <script src="js/jquery-ui.js"></script>
  <script>
  $(function() {
  $( "#datepicker" ).datepicker({ dateFormat: "dd.mm.yy", duration: "fast" });
    });
  </script>
  ';


$logo='<h1>
  <img src="img/insofter.jpg" width="716" height="96" alt="Insofter Komputerowe Systemy Automatyki">
  </h1>';

$stopka='
  <p id="stopka">
  Prosimy o nie dokonywanie zmian w konfiguracji bez konsultacji z administratorem.<br><br>
  <a href="http://'.$o_mnie['strona'].'">&copy; '.$o_mnie['wersja_rok'].' '.$o_mnie['autor'].'</a>
  </p>
  </body></html>';

$menuconfig=array(
  'tcpip'=>'TCPIP',
  'wifi'=>'WiFi',
  'licznik'=>'Licznik',
  'wysylanie'=>'Wysyłanie',
  'wyniki'=>'Wyniki',
  'log'=>'Logi',
  'plik_konf'=>'Plik konf.'
);

$menuglowne=array(
  'dobowy'=>'Raport dobowy',
  'miesieczny'=>'Raport miesięczny',
  'instalacyjny'=>'Raport instalacyjny',
  'konfiguracja'=>'Konfiguracja'
);





$menu='<ul class="menu">
  ';
if( isset($_GET['strona']) && isset($menulista[$_GET['strona']]) ) {
  $menu.='<li><a href="./">Info</a></li>';
  $tytul.=' - '.$menulista[$_GET['strona']];
} else {
  $menu.='<li><a id="klikniete" href="./">Info</a></li>';
}
foreach( $menuglowne as $key=>$val )
{
  if( $key=='konfiguracja' ) {
    $menu.='
      <li><ul id="menuconfig">
      <li><a href="./">Konfiguracja</a></li>
      ';
    foreach( $menuconfig as $kc=>$vc ) {
      if( isset($_GET['strona']) && $_GET['strona']==$kc )
      {
        $menu.='
          <li><a id="klikniete" href="./?strona='.$kc.'">'.$vc.'</a></li>';
      } else {
        $menu.='
          <li><a href="./?strona='.$kc.'">'.$vc.'</a></li>';
      }
    }
    $menu.='
      </ul></li>';
  } else if( isset($_GET['strona']) && $_GET['strona']==$key ) {
    $menu.='
      <li><a id="klikniete" href="./?strona='.$key.'">'.$val.'</a></li>';
  } else {
    $menu.='
      <li><a href="./?strona='.$key.'">'.$val.'</a></li>';
  }
}
$menu.='
  </ul>
  ';
/*
if( isset($_GET['strona']) && $_GET['strona']=='wyniki' ) {
  $menu.='<ul class="menu menuwyniki">
    ';
  if( ! isset($_GET['typ']) ) {
    $_GET['typ']='dobowy';
  }

  foreach( $menuwyniki as $key=>$val )
  {
    if( $_GET['typ']==$key )
    {
      $menu.='
        <li><a id="klikniete" href="./?strona=wyniki&amp;typ='.$key.'">'.$val.'</a></li>';
    } else {
      $menu.='
        <li><a href="./?strona=wyniki&amp;typ='.$key.'">'.$val.'</a></li>';
    }
  }
  $menu.='</ul>
    ';


}
 */



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
  case 'dobowy':
    include('inc/wyniki_dobowy.php');
    break;
  case 'miesieczny':
    include('inc/wyniki_miesieczny.php');
    break;
  case 'instalacyjny':
    include('inc/wyniki_instalacyjny.php');
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
