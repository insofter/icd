<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

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
  case 'csv':
    if( ! isset($_GET['czas'] ) ) {
      //TODO here
    }
    header('Content-Type: application/octet-stream');
    header("Pragma: public");
    header("Expires: 0");
    header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
    header("Content-Type: text/csv");
    header('Content-Disposition: attachment; filename="'.date('Y.m.d_H.i').'__data.csv"');
    $icdtcp->csv_export();
    exit();
    break;
  case 'set_send':

    $icdtcp->set_send_flag();

    $info='<h4>Ustawiono wszystkie rekordy jako wysłane</h4>';

  case 'recreate_databases':

    $icdtcp->recreate_databases();

    $info='<h4>Konfigruracja zostanie usunięta po restarcie</h4>';

    break;
  }

} 



$tresc='<div id="tresc">';

$tresc.='<form action="./?strona=tcpip" method="POST">';

$tresc.='
  <h3>Administracja</h3>';
if( isset( $info ) ) {
  $tresc.=$info;
}

$tresc.='
  <ul>
  <li><a onclick="return confirm('."'Czy na pewno chcesz uruchomić ponownie rejestrator?'".');"
    href="popup.php?typ=reboot">Uruchom ponownie</a></li>

  <li><a href="./?strona=plik_konf">Plik konf.</a>

  <li><a href="./?strona=administracja&amp;todo=csv">Zrzut bazy do CSV //TODO</a>

  <li><a href="./?strona=administracja&amp;todo=tar">Zrzut plików bazy (archiwum *.tar)</a>

  <li><a onclick="return confirm('."'Czy na pewno oznaczyć rekordy jako wysłane?'".');"
    href="./?strona=administracja&amp;todo=set_send">Oznacz wszystkie rekordy jako wysłane</a>

  <li><a onclick="return confirm('."'Czy na pewno usunąć konfigurację rejestratora?'".');"
    href="./?strona=administracja&amp;todo=recreate_databases">Usuń konfigurację rejestratora</a>

  </ul>
  ';

$tresc.='</div>';

?>
