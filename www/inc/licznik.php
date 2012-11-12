<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

//print_r( $_POST );

$icdtcp = new c_icdtcp();

if( isset( $_POST['name'] ) ) {

  for( $i=0; $i<$icdtcp->il_foto; ++$i ) {
    $licznik['name']=$_POST['name'][$i];
    if( isset($_POST['enabled'][$i]) ) {
      $licznik['enabled']='yes';
    } else {
      $licznik['enabled']='no';
    }
    $licznik['dev-engage']=$_POST['dev-engage'][$i];
    $licznik['dev-release']=$_POST['dev-release'][$i];
    $licznik['dev-active-low']=$_POST['dev-active-low'][$i];
    $licznik['thick']=$_POST['thick'][$i];
    $licznik['thick-active-low']=$_POST['thick-active-low'][$i];
    $licznik['enab']=$_POST['enab'][$i];
    $licznik['enab-time']=$_POST['enab-time'][$i];
    $licznik['enab-active-low']=$_POST['enab-active-low'][$i];
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
  <h3>Ustawienia licznika</h3>
  '.$info.'
  <form action="./?strona=licznik" method="POST">
  <table border=1>
  <tr>
  <th>Numer</th><th>Nazwa</th><th>Włączona</th>
  <th>Czas zwłoki</th><th>Czas opóźnienia</th><th>Stan aktywny</th>
  <th>Wyłącznik czasowy</th>
  <th>Czas wyłącznika</th>
  <th>Stan aktywny wyłącznika</th>
  <th>Wykrywanie grubości</th>
  <th>i kierunku</th>
  <th>Stan aktywny wykrywania</th></tr>
  ';


foreach( $liczniki as $licznik ) {
  $tresc.='<tr><td>'.chr(ord('A')+$licznik['nr']).'</td>
    <td><input type="text" name="name['.$licznik['nr'].']" id="name'
    .$licznik['nr'].'" value="'.$licznik['name'].'"></td>
    <td><input type="checkbox" name="enabled['.$licznik['nr'].']" id="enabled'.$licznik['nr'].'"';
  if( $licznik['enabled']=='yes' ) {
    $tresc.=' checked="checked"';
  }
  $tresc.='></td>
    <td><input type="text" name="dev-engage['.$licznik['nr'].']"
    id="dev-engage'.$licznik['nr'].'" value="'.$licznik['dev-engage'].'" class="cienki"></td>
    <td><input type="text" name="dev-release['.$licznik['nr'].']"
    id="dev-release'.$licznik['nr'].'" value="'.$licznik['dev-release'].'" class="cienki"></td>
    <td>
    <select name="dev-active-low['.$licznik['nr'].']" class="cienki">';
  if( $licznik['dev-active-low']=='yes' ) {
    $tresc.='<option selected="selected" value="yes">Niski</option>
      <option value="no">Wysoki</option>';
  } else {
    $tresc.='<option value="yes">Niski</option>
      <option selected="selected" value="no">Wysoki</option>';
  }

  $tresc.='</select>
    </td>
    <td><select name="enab['.$licznik['nr'].']" id="enab'.$licznik['nr'].'">';
  foreach( $icdtcp->urzadzenia as $wart=>$opis ) {
    $tresc.='<option';
    if( $wart==$licznik['enab'] ) {
      $tresc.=' selected="selected"';
    }
    $tresc.=' value="'.$wart.'">'.$opis.'</option>';
  }
  $tresc.='</select>
  </td>
  <td>
  <input type="text" name="enab-time['.$licznik['nr'].']"
      id="enab-time'.$licznik['nr'].'" value="'.$licznik['enab-time'].'" class="cienki">
  
  </td>
  <td>
  <select name="enab-active-low['.$licznik['nr'].']" class="cienki">';
  if( $licznik['enab-active-low']=='yes' ) {
    $tresc.='<option selected="selected" value="yes">Niski</option>
      <option value="no">Wysoki</option>';
  } else {
    $tresc.='<option value="yes">Niski</option>
      <option selected="selected" value="no">Wysoki</option>';
  }

  $tresc.='</select>

  
  </td>
    <td><select name="thick['.$licznik['nr'].']" id="thick'.$licznik['nr'].'">';
  foreach( $icdtcp->urzadzenia as $wart=>$opis ) {
    $tresc.='<option';
    if( $wart==$licznik['thick'] ) {
      $tresc.=' selected="selected"';
    }
    $tresc.=' value="'.$wart.'">'.$opis.'</option>';
  }
  $tresc.='</select>
  </td>
  <td>
  <select name="thick-detect-direction['.$licznik['nr'].']" class="cienki">';
  if( $licznik['thick-detect-direction']=='yes' ) {
    $tresc.='<option selected="selected" value="yes">Grubość i kierunek</option>
      <option value="no">Grubość</option>';
  } else {
    $tresc.='<option value="yes">Grubość i kierunek</option>
      <option selected="selected" value="no">Grubość</option>';
  }
  $tresc.='</select>
  
  </td>
  <td>
  <select name="thick-active-low['.$licznik['nr'].']" class="cienki">';
  if( $licznik['thick-active-low']=='yes' ) {
    $tresc.='<option selected="selected" value="yes">Niski</option>
      <option value="no">Wysoki</option>';
  } else {
    $tresc.='<option value="yes">Niski</option>
      <option selected="selected" value="no">Wysoki</option>';
  }
  $tresc.='</select>
    </td>
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
