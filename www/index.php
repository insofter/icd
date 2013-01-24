<?php
session_start();

define( 'INSOFTER', 1 );
require_once('inc/c_icdtcp.php');

if( isset( $_POST['logout'] ) ) {
  unset( $_SESSION['zalogowany'] );
}

if( isset( $_POST['password'] ) ) {
  $icdtcp = new c_icdtcp();
  $_SESSION['zalogowany']=$icdtcp->zaloguj( $_POST['password'] );
  unset( $icdtcp );
} else if( ! isset( $_SESSION['zalogowany']) ) {
  $icdtcp = new c_icdtcp();
  $_SESSION['zalogowany']=$icdtcp->zaloguj( NULL );
  unset( $icdtcp );
} 
  



$o_mnie=array('nazwa'=>'ICDTCP3',
  'opis'=>'Interfejs konfiguracyjny rejestratora ICDTCP3',
  'wersja_rok'=>'2013',
  'wersja_miesiac'=>'01',
  'autor'=>'insofter',
  'strona'=>'www.insofter.pl');


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
  $( "#datepicker" ).datepicker({ dateFormat: "yy.mm.dd", duration: "fast" });
    });
  </script>
  ';

if( $_SESSION['zalogowany']==2 ) {
  $logo='<form action="./" method="POST">
    <h1>
    <input type="submit" value="Wyloguj się">
    <input type="hidden" name="logout" value="bye">
    </h1>
    </form>';
} else {
  $logo='<h1>
    </h1>';
}

$stopka='
  <p id="stopka">
  Prosimy o nie dokonywanie zmian w konfiguracji bez konsultacji z administratorem.<br><br>
  <a href="http://'.$o_mnie['strona'].'">&copy; '.$o_mnie['wersja_rok'].' '.$o_mnie['autor'].'</a>
  </p>
  </body></html>';

$menurozwijane=array(
  'licznik'=>'Licznik',
  'wysylanie'=>'Wysyłanie',
  'tcpip'=>'TCPIP',
  'wifi'=>'WiFi',
  'gsm'=>'GSM',
  'administracja'=>'Administracja',
  'log'=>'Pliki dziennika',
  'logowanie'=>'Konf. dostępu',
);

$menuglowne=array(
  'dobowy'=>'Raport dobowy',
  'miesieczny'=>'Raport miesięczny',
  'instalacyjny'=>'Raport instalacyjny',
  'konfiguracja'=>'Konfiguracja'
);





$menu='<ul class="menu">
  ';
if( isset($_GET['strona']) && ( isset($menuglowne[$_GET['strona']]) || isset($menurozwijane[$_GET['strona']]) ) ) {
  $menu.='<li class="menu menuinfo"><a href="./">Info</a></li>';
  if( isset($menuglowne[$_GET['strona']]) ) {
    $tytul.=' - '.$menuglowne[$_GET['strona']]; 
  } else {
    $tytul.=' - '.$menurozwijane[$_GET['strona']];
  }
} else {
  $menu.='<li class="menu menuinfo"><a class="klikniete" href="./">Info</a></li>';
}
foreach( $menuglowne as $key=>$val )
{
  if( $key=='konfiguracja' ) {
    $menu.='
      <li class="menurozwijane"><ul class="menurozwijane">
      <li>';
    if( isset($_GET['strona']) && isset($menurozwijane[$_GET['strona']]) ) {
      $menu.='<a class="klikniete" href="./">Konfiguracja</a>';
    } else {
      $menu.='<a href="./">Konfiguracja</a>';

    }
        $menu.='</li>
      ';
    foreach( $menurozwijane as $kc=>$vc ) {
      if( isset($_GET['strona']) && $_GET['strona']==$kc )
      {
        $menu.='
          <li><a class="klikniete" href="./?strona='.$kc.'">'.$vc.'</a></li>';
      } else {
        $menu.='
          <li><a href="./?strona='.$kc.'">'.$vc.'</a></li>';
      }
    }
    $menu.='
      </ul></li>';
  } else if( isset($_GET['strona']) && $_GET['strona']==$key ) {
    $menu.='
      <li class="menu"><a class="klikniete" href="./?strona='.$key.'">'.$val.'</a></li>';
  } else {
    $menu.='
      <li class="menu"><a href="./?strona='.$key.'">'.$val.'</a></li>';
  }
}
$menu.='
  </ul>
  ';



if( isset($_GET['strona']) ) {
  switch( $_GET['strona'] ) {
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
  case 'logowanie':
    include('inc/logowanie.php');
    break;


  case 'tcpip':
    if( $_SESSION['zalogowany']!=0 ) {
      include('inc/tcpip.php');
    } else {
      include('inc/logowanie.php');
    }
    break;
  case 'licznik':
    if( $_SESSION['zalogowany']!=0 ) {
      include('inc/licznik.php');
    } else {
      include('inc/logowanie.php');
    }
    break;
  case 'wysylanie':
    if( $_SESSION['zalogowany']!=0 ) {
      include('inc/wysylanie.php');
    } else {
      include('inc/logowanie.php');
    }
    break;
  case 'plik_konf':
    if( $_SESSION['zalogowany']!=0 ) {
      include('inc/plik_konf.php');
    } else {
      include('inc/logowanie.php');
    }
    break;
  case 'test_wysylania':
    if( $_SESSION['zalogowany']!=0 ) {
      include('inc/test_wysylania.php');
    } else {
      include('inc/logowanie.php');
    }
    break;
  case 'wifi':
    if( $_SESSION['zalogowany']!=0 ) {
      include('inc/wifi.php');
    } else {
      include('inc/logowanie.php');
    }
    break;
  case 'gsm':
    if( $_SESSION['zalogowany']!=0 ) {
      include('inc/gsm.php');
    } else {
      include('inc/logowanie.php');
    }
    break;
  case 'administracja':
    if( $_SESSION['zalogowany']!=0 ) {
      include('inc/administracja.php');
    } else {
      include('inc/logowanie.php');
    }
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
