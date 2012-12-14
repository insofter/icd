<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

$info=$icdtcp->info();

$tresc='<div id="tresc">';

$tresc.='<form action="./?strona=tcpip" method="POST">';

$tresc.='
  <h3>Administracja</h3>';

//  'plik_konf'=>'Plik konf.'
//
$tresc.='
  <ul>
  <li><a onclick="return confirm('."'Czy na pewno chcesz uruchomić ponownie rejestrator?'".');"
    href="popup.php?typ=reboot">Uruchom ponownie</a></li>

  <li><a href="./?strona=plik_konf">Plik konf.</a>

  <li><a href="./?strona=plik_konf">Zrzut bazy do CSV //TODO</a>

  <li><a href="./?strona=plik_konf">Zrzut plików bazy //TODO</a>

  <li><a onclick="return confirm('."'Czy na pewno oznaczyć rekordy jako wysłane?'".');"
    href="./?strona=plik_konf">Oznacz wszystkie rekordy jako wysłane //TODO</a>

  <li><a onclick="return confirm('."'Czy na pewno usunąć konfigurację rejestratora?'".');"
    href="./?strona=plik_konf">Usuń konfigurację rejestratora //TODO</a>

  </ul>
  ';

$tresc.='</div>';
?>
