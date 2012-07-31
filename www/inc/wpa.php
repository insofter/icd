<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

require_once('inc/c_wpa.php');

$wpa = new c_wpa();

//$wpa->start();
if( isset( $_GET['usun'] ) ) {
	$wpa->deln($_GET['usun']);
	header('Location ./?strona=wpa');
}
if( isset( $_GET['wylacz'] ) ) {
	$wpa->disb($_GET['wylacz']);
	header('Location ./?strona=wpa');
}
if( isset( $_GET['wlacz'] ) ) {
	$wpa->enab($_GET['wlacz']);
	header('Location ./?strona=wpa');
}


if( isset( $_GET['dodaj'] ) ) {
	
	if( $_GET['dodaj']=='' ) { 

	if( !isset( $_SESSION['lastscan'] ) || time()-$_SESSION['lastscan']>30 ) {
		$wpa->scan();
		$_SESSION['lastscan']=time();
	}

	$scan=$wpa->rscn();//scan();
//	$wpa->scan();
//	print_r($scan);
	
	$naglowekcd.='<meta http-equiv="refresh" content="10">';

	$tresc='<div id="tresc"><h3>Dodaj sie�</h3><table>
		<tr><th colspan="3">Wyniki skanowania</th>';

	foreach($scan as $s) {
		$tresc.='<tr><th>
<a href="./?strona=wpa&amp;dodaj='.$s[4].'">'.$s[4].'</a></th><td>Moc: '.$s[2].'%</td><td>'.$s[3].'</td></tr>';
	}
	$tresc.='</table></div>';
	} else {
		$tresc='<div id="tresc"><h3>Dodaj sie�</h3>
<form action="./?strona=wpa" method="POST">
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
		$wpa->addn( $_POST['ssid'], $_POST['psk'] );
		$tresc.='<h4>Dodano sie�: '.$_POST['ssid'].'</h4>';
	}

	$tresc.='<table><tr><th colspan="2">Stan</th></tr>';
	$stat=$wpa->stat();
	foreach($stat as $k=>$v) {
		$tresc.='<tr><th>'.$k.'</th><td>'.$v.'</td></tr>';
	}

	$tresc.='</table>';

	$lstn=$wpa->lstn();

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
					$tresc.='<em>po��czona</em>';
					break;
			}
		} else {
			$tresc.='aktywna';
		}
		$tresc.='</td><td>
			<a onclick="return confirm(\'Usun�� sie� '.$net[1].'?\');" href="./?strona=wpa&amp;usun='.$net[0].'">usu�</a>';
		if( isset( $net[3] ) && $net[3]=='[DISABLED]' ) {
			$tresc.=' <a href="./?strona=wpa&amp;wlacz='.$net[0].'">w��cz</a>';
		} else {
			$tresc.=' <a href="./?strona=wpa&amp;wylacz='.$net[0].'">wy��cz</a>';
		}
		$tresc.='</td></tr>';
	}

	$tresc.='<tr><th colspan="3"><a href="./?strona=wpa&amp;dodaj">Dodaj sie�</a></th><td></td></tr>';
	$tresc.='</table></div>';
}
?>