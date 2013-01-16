<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

require_once('inc/c_wifi.php');

$wifi = new c_wifi();

//$wifi->start();
if( isset( $_GET['usun'] ) ) {
  $wifi->deln($_GET['usun']);
  header('Location ./?strona=wifi');
}
if( isset( $_GET['wylacz'] ) ) {
  $wifi->disb($_GET['wylacz']);
  header('Location ./?strona=wifi');
}
if( isset( $_GET['wlacz'] ) ) {
  $wifi->enab($_GET['wlacz']);
  header('Location ./?strona=wifi');
}


if( isset( $_GET['dodaj'] ) ) {

  if( $_GET['dodaj']=='' ) {

    if( !isset( $_SESSION['lastscan'] ) || time()-$_SESSION['lastscan']>30 ) {
      $wifi->scan();
      $_SESSION['lastscan']=time();
    }

    $scan=$wifi->rscn();//scan();
    //	$wifi->scan();
    //	print_r($scan);

    $naglowekcd.='<meta http-equiv="refresh" content="10">';

    $tresc='<div id="tresc"><h3>Dodaj sieć</h3><table>
      <tr><th colspan="3">Wyniki skanowania</th>';

    foreach($scan as $s) {
      $tresc.='<tr><th>
        <a href="./?strona=wifi&amp;dodaj='.$s[4].'">'.$s[4].'</a></th><td>Moc: '.$s[2].'</td><td>'.$s[3].'</td></tr>';
    }
    $tresc.='</table></div>';
  } else {
    $tresc='<div id="tresc"><h3>Dodaj sieć</h3>
      <form action="./?strona=wifi" method="POST">
      <table>
      <tr><th><label for="ssid">SSID</label></th><td>
      <input type="text" name="ssid" id="ssid" value="'.$_GET['dodaj'].'">
      </td></tr>
      <tr><th><label for="psk">Klucz</label></th><td>
      <input type="text" name="psk" id="psk">
      </td></tr>
      <tr><td colspan="2">
      <input type="submit" value="Dodaj">
      </td></tr>
      </table>
      </form>';

  }
} else {

  $naglowekcd.='<meta http-equiv="refresh" content="15">';

  $tresc='<div id="tresc"><h3>Konfiguracja WiFi</h3>';

  if( isset( $_POST['ssid'] ) ) {
    $wifi->addn( $_POST['ssid'], $_POST['psk'] );
    $tresc.='<h4>Dodano sieć: '.$_POST['ssid'].'</h4>';
  }
  if( isset( $_POST['wifi_toggle'] ) ) {
    $wifi->toggle($_POST['wifi_toggle']);
  }



  $tresc.='<form action="./?strona=wifi" method="POST">
    <p id="wifi">';

  if( $wifi->enabled()=='yes' ) {
    $tresc.='<input type="submit" value="Wyłącz WiFi">
      <input type="hidden" name="wifi_toggle" value="no">';
  } else {
    $tresc.='<input type="submit" value="Włącz WiFi">
      <input type="hidden" name="wifi_toggle" value="yes">';
  }
  $tresc.='</form>';




  $tresc.='<table><tr><th colspan="2">Stan</th></tr>';
  $stat=$wifi->stat();
  foreach($stat as $k=>$v) {
    $tresc.='<tr><th>'.$k.'</th><td>'.$v.'</td></tr>';
  }

  $tresc.='</table>';

  $lstn=$wifi->lstn();

  $tresc.='<br><br><table><tr><th colspan="3">Sieci skonfigurowane</th></tr>';
  foreach($lstn as $net) {
    $tresc.='<tr><th>'.$net[1].'</th><td>';

    if( isset( $net[3] ) ) {
      switch( $net[3] ) {
      case '[DISABLED]':
        $tresc.='nieaktywna';
        break;
      case '[ENABLED]':
        $tresc.='aktywna';
        break;
      case '[CURRENT]':
        $tresc.='<em>połączona</em>';
        break;
      }
    } else {
      $tresc.='aktywna';
    }
    $tresc.='</td><td>
      <a onclick="return confirm(\'Usunąć sieć '.$net[1].'?\');" href="./?strona=wifi&amp;usun='.$net[0].'">usuń</a>';
    if( isset( $net[3] ) && $net[3]=='[DISABLED]' ) {
      $tresc.=' <a href="./?strona=wifi&amp;wlacz='.$net[0].'">włącz</a>';
    } else {
      $tresc.=' <a href="./?strona=wifi&amp;wylacz='.$net[0].'">wyłącz</a>';
    }
    $tresc.='</td></tr>';
  }

  $tresc.='<tr><th colspan="3"><a href="./?strona=wifi&amp;dodaj">Dodaj sieć</a></th><td></td></tr>';
  $tresc.='</table></div>';
}
?>
