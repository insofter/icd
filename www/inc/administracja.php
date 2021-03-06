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
    if( ! isset($_GET['send'] ) ) {
      $tresc.='<h3>Eksport do pliku CSV</h3>';
      $tresc.='<form action="./" method="GET">
        <input type="hidden" name="strona" value="administracja">
        <input type="hidden" name="todo" value="csv">';
      $tresc.='<table border=1 width="90%"> 
<tr>
<td><label for="type_year">Raport roczny</label></td>
<td><input type="radio" name="type" value="year" checked="checked" id="type_year"></td>
<td><label for="year">rok:</label><input type="text" name="year" value="'.date('Y').'" id="year"></td>
</tr><tr>
<td><label for="type_month">Raport miesięczny</label></td>
<td><input type="radio" name="type" value="month" id="type_month"></td>
<td><label for="month">miesiąc:</label><input type="text" name="month" value="'.date('Y.m').'" id="month"></td>
</tr><tr>
<td><label for="type_period">Raport od - do</label></td>
<td><input type="radio" name="type" value="period" id="type_period"></td>
<td><label for="from">od:</label><input type="text" name="from" value="'.date('Y.m.d').'" id="from" size="10">
<label for="to">do:</label><input type="text" name="to" value="'.date('Y.m.d',time()+24*3600).'" id="to" size="10"></td>
</tr><tr>
<td><label for="header">Dołącz nagłówki</label></td>
<td colspan="2"><input type="checkbox" name="header" checked="checked" id="header"></td>
</tr><tr>
<td colspan="3">

<input type="submit" name="send" value="Pobierz">
<input type="submit" name="send" value="Oznacz">
<input type="reset" value="Anuluj">
<input type="submit" name="send" value="URI">
</td>
</tr>
</table>
        ';
      $tresc.='</form>';

    } else {//obsluz wysłany formularz
      switch( $_GET['type'] ) {
      case 'year':
        $od=mktime( 0,0,0, 1, 1, $_GET['year']     );
        $do=mktime( 0,0,0, 1, 1, ($_GET['year']+1) );
        break;
      case 'month':
        sscanf( $_GET['month'], '%i.%i', $year, $month );
        $od=mktime( 0,0,0,  $month,     1, $year );
        $do=mktime( 0,0,0,  ($month+1), 1, $year );
        break;
      case 'period':
        sscanf( $_GET['from'], '%i.%i.%i', $year, $month, $day );
        $od=mktime( 0,0,0, $month, $day, $year );
        sscanf( $_GET['to'], '%i.%i.%i', $year, $month, $day );
        $do=mktime( 0,0,0, $month, $day, $year );
        break;
      }
      if( isset( $_GET['header'] ) && $_GET['header'] == 'on' ) {
        $header=true;
      } else {
        $header=false;
      }
      if( $_GET['send']=='Pobierz' ) {
      header('Content-Type: application/octet-stream');
      header("Pragma: public");
      header("Expires: 0");
      header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
      header("Content-Type: text/csv");
      header('Content-Disposition: attachment; filename="'.date('Y.m.d_H.i').'__data.csv"');
      $icdtcp->csv_export($od, $do, $header);
      exit();
      } else if( $_GET['send']=='Oznacz' ) {
        $info='<h4>'.$icdtcp->csv_export_done($od, $do).'</h4>';
      } else {
        $uri = str_replace( '&', '&amp;', 'http://'. $_SERVER['HTTP_HOST'] . $_SERVER['REQUEST_URI'] );
        $info='<h4>'.$uri.'</h4>';
      }
    }
    break;
  case 'set_send':

    $icdtcp->set_send_flag();

    $info='<h4>Ustawiono wszystkie rekordy jako wysłane</h4>';
    break;

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

  <li><a href="./?strona=administracja&amp;todo=csv">Zrzut bazy do CSV</a>

  <li><a href="./?strona=administracja&amp;todo=tar">Zrzut plików bazy (archiwum *.tar)</a>

  <li><a onclick="return confirm('."'Czy na pewno oznaczyć rekordy jako wysłane?'".');"
  href="./?strona=administracja&amp;todo=set_send">Oznacz wszystkie rekordy jako wysłane</a>

  <li><a onclick="return confirm('."'Czy na pewno usunąć konfigurację rejestratora?'".');"
  href="./?strona=administracja&amp;todo=recreate_databases">Usuń konfigurację rejestratora</a>

  </ul>
  ';

$tresc.='</div>';

?>
