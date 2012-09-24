<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

if( isset( $_POST['name'] ) ) {

  for( $i=0; $i<$icdtcp->il_foto; ++$i ) {
    $licznik['name']=$_POST['name'][$i];
    if( isset($_POST['enabled'][$i]) ) {
      $licznik['enabled']='yes';
    } else {
      $licznik['enabled']='no';
    }
    $licznik['engage-delay-ms']=$_POST['engage-delay-ms'][$i];
    $licznik['release-delay-ms']=$_POST['release-delay-ms'][$i];
    $licznik['active-low']=$_POST['active-low'][$i];
    $liczniki[]=$licznik;
    //todo kierunek, drzwi
  }
  $icdtcp->liczniki_ustaw($liczniki);
  $info='<h4>Zmodyfikowano konfigurację</h4>';
} else {
  $info='';
}
$liczniki=$icdtcp->liczniki_pobierz();

$tresc='<div id="tresc">
  <h3>Stan czujników</h3>
  '.$info.'
  <form action="./?strona=licznik" method="POST">
  <table>
  <tr>
  <th>Numer</th><th>Nazwa</th><th>Włączona</th>
  <th>Czas zwłoki</th><th>Czas opóźnienia</th><th>Stan aktywny</th>
  <th>Wykrywanie kierunku</th><th>Wykrywanie podwójne</th></tr>';

foreach( $liczniki as $licznik ) {
  $tresc.='<tr><td>'.chr(ord('A')+$licznik['nr']).'</td>
    <td><input type="text" name="name['.$licznik['nr'].']" id="name'
    .$licznik['nr'].'" value="'.$licznik['name'].'"></td>
    <td><input type="checkbox" name="enabled['.$licznik['nr'].']" id="enabled'.$licznik['nr'].'"';
  if( $licznik['enabled']=='yes' ) {
    $tresc.=' checked="checked"';
  }
  $tresc.=' onclick="nieaktywny('.$licznik['nr'].')"></td>
    <td><input type="text" name="engage-delay-ms['.$licznik['nr'].']"
    id="engage-delay-ms'.$licznik['nr'].'" value="'.$licznik['engage-delay-ms'].'" class="cienki"></td>
    <td><input type="text" name="release-delay-ms['.$licznik['nr'].']"
    id="release-delay-ms'.$licznik['nr'].'" value="'.$licznik['release-delay-ms'].'" class="cienki"></td>
    <td>
    <select name="active-low['.$licznik['nr'].']" class="cienki">';
  if( $licznik['active-low']==1 ) {
    $tresc.='<option selected="selected" value="1">Niski</option>
      <option value="0">Wysoki</option>';
  } else {
    $tresc.='<option value="1">Niski</option>
      <option selected="selected" value="0">Wysoki</option>';
  }

  $tresc.='</select>
    </td>
    <td><input type="checkbox" name="kierunek['.$licznik['nr'].']" id="kierunek'.$licznik['nr'].'"';
  //if( $licznik['enabled']=='yes' ) {
  //	$tresc.=' checked="checked"';
  //}
  $tresc.=' disabled="disabled"></td>
    <td><input type="checkbox" name="drzwi['.$licznik['nr'].']" id="drzwi'.$licznik['nr'].'"';
  //if( $licznik['enabled']=='yes' ) {
  //	$tresc.=' checked="checked"';
  //}
  $tresc.=' disabled="disabled"></td>
    </tr>';
}

$tresc.='
  </table>
  <br>
  <input type="submit" value="Ustaw">
  <input type="reset" value="Anuluj">
  <input type="button" value="Test fotokomórek">
  </form></div>';


?>
