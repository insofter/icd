<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

if( isset($_POST['dhcp_toggle']) ) {
  $icdtcp->dhcp_ustaw($_POST['dhcp_toggle']);
  $info='<h4>Zmodyfikowano konfigurację</h4>';
} else if( isset($_POST['ip']) ) {
  $nowe=array('ip'=>$_POST['ip'],
    'mask'=>$_POST['mask'],
    'gate'=>$_POST['gate'],
    'dns1'=>$_POST['dns1'],
    'dns2'=>$_POST['dns2'],
    'net-name'=>$_POST['net-name'],
    'mac'=>$_POST['mac']);
  $icdtcp->tcpip_ustaw($nowe);
  $info='<h4>Zmodyfikowano konfigurację</h4>';
} else {
  $info='';
}


$current=$icdtcp->tcpip_pobierz('current');

$tcpip=$icdtcp->tcpip_pobierz('tcpip');

$dhcp=$icdtcp->dhcp_pobierz();



$tresc='<div id="tresc">
  <h3>Połączenie sieciowe</h3>
  '.$info.'<form action="./?strona=tcpip" method="POST">
  <table><tr><th>DHCP : ';
if( $dhcp['dhcp']=='yes' ) {
  $tresc.='aktywne
    </th><td><input type="submit" value="Wyłącz DHCP">
    <input type="hidden" name="dhcp_toggle" value="no">';
} else {
  $tresc.='nieaktywne
    </th><td><input type="submit" value="Włącz DHCP">
    <input type="hidden" name="dhcp_toggle" value="yes">';
}
$tresc.='</td></tr></table></form><br><br>';

$tresc.='<div id="prawy"><form action="./?strona=tcpip" method="POST"';

if( $dhcp['dhcp']=='yes' ) {
  $tresc.=' class="nieaktywne"';
}
$tresc.='>';

$tresc.='<table>
  <tr><th colspan="2">Konfiguracja statyczna</th></tr>
  <tr><th><label for="ip">Adres IP</label> :</th><td>
  <input type="text" name="ip" id="ip" value="'.$tcpip['ip'].'"></td></tr>
  <tr><th><label for="mask">Maska</label> :</th><td>
  <input type="text" name="mask" id="mask" value="'.$tcpip['mask'].'"></td></tr>
  <tr><th><label for="gate">Brama</label> :</th><td>
  <input type="text" name="gate" id="gate" value="'.$tcpip['gate'].'"></td></tr>
  <tr><th><label for="dns1">DNS 1</label> :</th><td>
  <input type="text" name="dns1" id="dns1" value="'.$tcpip['dns1'].'"></td></tr>
  <tr><th><label for="dns2">DNS 2</label> :<br><br></th><td>
  <input type="text" name="dns2" id="dns2" value="'.$tcpip['dns2'].'"><br><br></td></tr>
  <tr><th><label for="net-name">Nazwa</label> :</th><td>
  <input type="text" name="net-name" id="net-name" value="'.$tcpip['net-name'].'"></td></tr>
  <tr><th><label for="mac">MAC</label> :<br><br></th><td>
  <input type="text" name="mac" id="mac" value="'.$tcpip['mac'].'"><br><br></td></tr>
  <tr><th colspan="2"><input type="submit" value="Zmień konfigurację"></th></tr></table>';




$tresc.='</form></div><div id="lewy"><form action="./?strona=tcpip" method="POST"';
$tresc.='>';

$tresc.='<table>
  <tr><th colspan="2">Konfiguracja aktualna</th></tr>
  <tr><th><label for="ipa">Adres IP</label> :</th><td>
  <input type="text" name="ipa" id="ipa" readonly="readonly" value="'.$current['ip'].'"></td></tr>
  <tr><th><label for="maska">Maska</label> :</th><td>
  <input type="text" name="maska" id="maska" readonly="readonly" value="'.$current['mask'].'"></td></tr>
  <tr><th><label for="gatea">Brama</label> :</th><td>
  <input type="text" name="gatea" id="gatea" readonly="readonly" value="'.$current['gate'].'"></td></tr>
  <tr><th><label for="dns1a">DNS 1</label> :</th><td>
  <input type="text" name="dns1a" id="dns1a" readonly="readonly" value="'.$current['dns1'].'"></td></tr>
  <tr><th><label for="dns2a">DNS 2</label> :</th><td>
  <input type="text" name="dns2a" id="dns2a" readonly="readonly" value="'.$current['dns2'].'">
  </td></tr></table>';



$tresc.='</form></div></div>';



?>
