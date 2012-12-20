<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

$tresc='<div id="tresc">';

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
      $tresc.='<h3>Eksport do pliku CSV</h3>';
      $tresc.='<form action="./" method="GET">
        <input type="hidden" name="strona" value="administracja">
        <input type="hidden" name="todo" value="csv">';
      $tresc.='<table border=1 width="90%"> 
<tr>
<td><label for="type_year">Raport roczny</label></td>
<td><input type="radio" name="type" value="year" checked="checked" id="type_year"></td>
</tr>
<tr>
<td><label for="type_month">Raport miesięczny</label></td>
<td><input type="radio" name="type" value="month" id="type_month"></td>
</tr>
<tr>
<td><label for="type_period">Raport od - do</label></td>
<td><input type="radio" name="type" value="period" id="type_period"></td>
</tr>
<tr>
<td><label for="header">Dołącz nagłówek</label></td>
<td><input type="checkbox" name="header" checked="checked" id="header"></td>
</tr>

<tr>
<td><label for="aggr">Agregacja pomiarów</label></td>
<td>
<select name="aggr" id="aggr">
<option selected="selected" value="no">Brak</option>
<option value="day">Dzień</option>
<option value="hour">Godzinowa</option>
<option value="quarter">15 minut</option>
</select>
</td>
</tr>

        </table>

        <input type="submit" name="send" value="Ustaw">
        <input type="submit" name="send" value="Oznacz">
        <input type="reset" value="Anuluj">

        ';
      $tresc.='</form>';

      //TODO here
    } else {
      header('Content-Type: application/octet-stream');
      header("Pragma: public");
      header("Expires: 0");
      header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
      header("Content-Type: text/csv");
      header('Content-Disposition: attachment; filename="'.date('Y.m.d_H.i').'__data.csv"');
      $icdtcp->csv_export();
      exit();
    }
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
