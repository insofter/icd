<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

if( isset($_GET['todo']) ) {
  switch( $_GET['todo'] ) {
  case 'tar':
    header('Content-Type: application/octet-stream');
    header("Pragma: public");
    header("Expires: 0");
    header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
    header("Content-Type: application/x-tar");
    header('Content-Disposition: attachment; filename="'.date('Y.m.d_H.i').'__data.tar"');

    passthru('cd /home && tar cf - data/');
    exit();

    break;
  }

} else {


$icdtcp = new c_icdtcp();

$tresc='<div id="tresc">';

$tresc.='<form action="./?strona=tcpip" method="POST">';

$tresc.='
  <h3>Administracja</h3>';

$tresc.='
  <ul>
  <li><a onclick="return confirm('."'Czy na pewno chcesz uruchomić ponownie rejestrator?'".');"
    href="popup.php?typ=reboot">Uruchom ponownie</a></li>

  <li><a href="./?strona=plik_konf">Plik konf.</a>

  <li><a href="./?strona=plik_konf">Zrzut bazy do CSV //TODO</a>

  <li><a href="./?strona=administracja&amp;todo=tar">Zrzut plików bazy (archiwum *.tar)</a>

  <li><a onclick="return confirm('."'Czy na pewno oznaczyć rekordy jako wysłane?'".');"
    href="./?strona=plik_konf">Oznacz wszystkie rekordy jako wysłane //TODO</a>

  <li><a onclick="return confirm('."'Czy na pewno usunąć konfigurację rejestratora?'".');"
    href="./?strona=plik_konf">Usuń konfigurację rejestratora //TODO</a>

  </ul>
  ';

$tresc.='</div>';

}
?>
