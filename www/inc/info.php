<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

$info=$icdtcp->info();

$tresc='<div id="tresc">';

$tresc.='<form action="./?strona=tcpip" method="POST">';

$tresc.='
  <h3>Rejestrator Licznika Klientow ICDTCP3 </h3>
  <table>
  <tr><th>
  <label for="address">Adres danych</label> :
  </th><td>
  <input type="text" name="address" id="address" readonly="readonly" value="'.$info['address'].'" size="40">
  </td></tr>
  <tr><th>
  <label for="ids">ID salonu</label> :
  </th><td>
  <input type="text" name="ids" id="ids" readonly="readonly" value="'.$info['ids'].'" size="40">
  </td></tr>
  <tr><th>
  <label for="idd">ID urządzenia</label> :
  </th><td>
  <input type="text" name="idd" id="idd" readonly="readonly" value="'.$info['idd'].'" size="40">
  </td></tr>
  </table>
  <br><br>
  ';
$tresc.='</form><table>
  <caption>Stan czujników</caption>
  <tr>
  <th>Numer</th><th>Nazwa</th><th>Ostatni pomiar</th><th>Pomiar dzienny</th>';
/*$tresc.='<th>Stan</th>';*/
$tresc.='</tr>';
foreach($info['liczniki'] as $licznik) {
  $tresc.='<tr ';
  if( $licznik['enabled']!='yes' ) {
    $tresc.='class="nieaktywne"';
  }
  $tresc.='><td>'.$licznik['counter_id'].'</td><td>'.$licznik['name'].'</td>
    <td>'.$licznik['cnt_last'].'</td><td>'.$licznik['cnt_sum'].'</td>';
    
/*    $tresc.='<td><img src="img/';
  if( $licznik['enabled']!='yes' ) {
    $tresc.='black';
  } else if( $licznik['state']==1 ) {
    $tresc.='green';
  } else {
    $tresc.='red';
  }

  $tresc.='.gif" alt="'.$licznik['state'].'"></td>';*/
    
  $tresc.='</tr>';
}


$tresc.='</table>
  <form action="./" method="POST">
  <p id="adres">
  <input type="button" value="Test fotokomórek" onclick="window.open('."'".'popup.php?typ=test'."'".')">
  <input type="button" value="Test wysyłania" onclick="window.open('."'".'popup.php?typ=transfer'."'".')">
  <br><br>
  <br><br>
  Insofter<br>
  ul. Skłodowskiej-Curie 55/61<br>
  50-950 Wrocław<br>
  tel./fax +48 (071) 328 23 41<br>
  e-mail: <a href="mailto:liczniki@insofter.pl">liczniki@insofter.pl</a>
  <br>
  <br>
  <br>
  <br>
  Wygenerowano '.date('d.m.Y').'r. o godzinie '.date('G:i:s').'</p>
  </form>
  ';

$tresc.='</div>';
?>
