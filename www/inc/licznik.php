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
    $licznik['dev-engage']=$_POST['dev-engage'][$i];
    $licznik['dev-release']=$_POST['dev-release'][$i];
    $licznik['dev-active-low']=$_POST['dev-active-low'][$i];


    $licznik['thick']=$_POST['thick'][$i];
    $licznik['thick-active-low']=$_POST['thick-active-low'][$i];
    $licznik['thick-detect-direction']=$_POST['thick-detect-direction'][$i];
    $licznik['enab']=$_POST['enab'][$i];
    $licznik['enab-time']=$_POST['enab-time'][$i];
    $licznik['enab-active-low']=$_POST['enab-active-low'][$i];


    $liczniki[$i]=$licznik;
  }
  $icdtcp->liczniki_ustaw($liczniki);
  $info='<h4>Zmodyfikowano konfigurację</h4>';
} else {
  $info='';
}
$liczniki=$icdtcp->liczniki_pobierz();

print_r($liczniki);


$tresc='<div id="tresc">
  <h3>Ustawienia licznika</h3>
  '.$info;

function boolForm( $sect, $name, $label, $state, $yes, $no ) {
  $ret='<label for="'.$name.'_'.$sect.'">'.$label.' </label>
    <select name="'.$name.'" id="'.$name.'_'.$sect.'">
    ';

  if( $state=='yes' ) {
    $ret.='<option selected="selected" value="yes">'.$yes.'</option>
      <option value="no">'.$no.'</option>';
  } else {
    $ret.='<option value="yes">'.$yes.'</option>
      <option selected="selected" value="no">'.$no.'</option>';
  }
  $ret.='
    </select>';
  return $ret;
}
function textForm( $sect, $name, $label, $value ) {
  return '<label for="'.$name.'_'.$sect.'">'.$label.' </label>
    <input type="text" id="'.$name.'_'.$sect.'" name="'.$name.'" value="'.$value.'">';
}


foreach( $liczniki as $sect=>$licznik ) {

  $tresc.='<form action="./?strona=licznik" method="POST">
    <input type="hidden" value="'.$sect.'" name="sect">';

  $tresc.='<table class="liczniki">
    <tr><td class="liczniki_tab liczniki_top" colspan="3">Id licznika: '.$licznik['counter_id'].'
    <tr><td class="liczniki_tab" colspan="2">';
  $tresc.='<table>
    <tr><td>'
    .textForm( $sect, 'name', 'Nazwa:', $licznik['name'] ).
    '</td>
    <td>'
    .boolForm( $sect, 'enabled', 'Włączony:', $licznik['enabled'], 'Tak', 'Nie' ).
    '</td></tr>
    </table>';
  $tresc.='</td><td class="liczniki_tab" rowspan="2">';
  $tresc.='<table>
    <tr><td>'
    .textForm( $sect, 'slave', 'Wejście dodatkowe:', $licznik['slave'] ).
    '</td></tr>
    <tr><td>'
    .textForm( $sect, 'slave-mode', 'TODO tryb pracy:', $licznik['slave-mode'] ).
    '</td></tr>
    <tr><td>'
    .textForm( $sect, 'slave-engage', 'Czas narastania:', $licznik['slave-engage'] ).
    '</td></tr>
    <tr><td>'
    .textForm( $sect, 'slave-release', 'Czas opadania:', $licznik['slave-release'] ).
    '</td></tr>
    <tr><td>'
    .boolForm( $sect, 'slave-reversed', 'Stan aktywny:', $licznik['slave-reversed'], 'Niski', 'Wysoki' ).
    '</td></tr>
    </table>';
  $tresc.='</td></tr><tr><td class="liczniki_tab">';
  $tresc.='<table>
    <tr><td>'
    .textForm( $sect, 'master', 'Wejście główne:', $licznik['master'] ).
    '</td></tr>
    <tr><td>'
    .textForm( $sect, 'master-engage', 'Czas narastania:', $licznik['master-engage'] ).
    '</td></tr>
    <tr><td>'
    .textForm( $sect, 'master-release', 'Czas opadania:', $licznik['master-release'] ).
    '</td></tr>
    <tr><td>'
    .boolForm( $sect, 'master-reversed', 'Stan aktywny:', $licznik['master-reversed'], 'Niski', 'Wysoki' ).
    '</td></tr>
    </table>';
  $tresc.='</td><td class="liczniki_tab">';
  $tresc.='<table>
    <tr><td>'
    .textForm( $sect, 'stop', 'Wyłącznik czasowy:', $licznik['stop'] ).
    '</td></tr>
    <tr><td>'
    .textForm( $sect, 'stop-engage', 'Czas narastania:', $licznik['stop-engage'] ).
    '</td></tr>
    <tr><td>'
    .textForm( $sect, 'stop-release', 'Czas opadania:', $licznik['stop-release'] ).
    '</td></tr>
    <tr><td>'
    .boolForm( $sect, 'stop-reversed', 'Stan aktywny:', $licznik['stop-reversed'], 'Niski', 'Wysoki' ).
    '</td></tr>
    </table>';
  $tresc.='</td></tr>
    </table>';

  $tresc.='</form><br><br>';

}




/*
for( $i=0; $i<4; ++$i ) {
  $tresc.='<td>'.chr(ord('A')+$i).'</td>';
}
$tresc.='</tr><tr><th>Nazwa</th>
  ';
for( $i=0; $i<4; ++$i ) {
  $tresc.='<td><input type="text" name="name['.$i.']" id="name'
    .$i.'" value="'.$licznik['name'][$i].'"></td>';
}
$tresc.='</tr>
  <tr><th>Włączona</th>';
for( $i=0; $i<4; ++$i ) {
  $tresc.='<td><input type="checkbox" name="enabled['.$i.']" id="enabled'.$i.'"';
  if( $licznik['enabled'][$i]=='yes' ) {
    $tresc.=' checked="checked"';
  }
  $tresc.='></td>';
}
$tresc.='</tr>
  <tr><th>Czas zwłoki</th>';
for( $i=0; $i<4; ++$i ) {
  $tresc.='<td><input type="text" name="dev-engage['.$i.']"
    id="dev-engage'.$i.'" value="'.$licznik['dev-engage'][$i].'"></td>';
}
$tresc.='</tr>
  <tr><th>Czas opóźnienia</th>';
for( $i=0; $i<4; ++$i ) {
    $tresc.='<td><input type="text" name="dev-release['.$i.']"
      id="dev-release'.$i.'" value="'.$licznik['dev-release'][$i].'"></td>';
}
$tresc.='</tr>
  <tr><th>Stan aktywny</th>';
for( $i=0; $i<4; ++$i ) {
      $tresc.='<td>
    <select name="dev-active-low['.$i.']">';
  if( $licznik['dev-active-low'][$i]=='yes' ) {
    $tresc.='<option selected="selected" value="yes">Niski</option>
      <option value="no">Wysoki</option>';
  } else {
    $tresc.='<option value="yes">Niski</option>
      <option selected="selected" value="no">Wysoki</option>';
  }

  $tresc.='</select>
    </td>';
}
$tresc.='</tr>';


$tresc.='<tr><th>Wyłącznik czasowy</th>';
for( $i=0; $i<4; ++$i ) {
        $tresc.='<td><select name="enab['.$i.']" id="enab'.$i.'">';
  foreach( $icdtcp->urzadzenia as $wart=>$opis ) {
    $tresc.='<option';
    if( $wart==$licznik['enab'][$i] ) {
      $tresc.=' selected="selected"';
    }
    $tresc.=' value="'.$wart.'">'.$opis.'</option>';
  }
  $tresc.='</select>
    </td>';
}
$tresc.='</tr>
  <tr><th>Czas wyłącznika</th>';
for( $i=0; $i<4; ++$i ) {
          $tresc.='<td>
  <input type="text" name="enab-time['.$i.']"
      id="enab-time'.$i.'" value="'.$licznik['enab-time'][$i].'">

      </td>';
}
$tresc.='</tr>
  <tr><th>Stan aktywny wyłącznika</th>';
for( $i=0; $i<4; ++$i ) {
            $tresc.='<td>
  <select name="enab-active-low['.$i.']">';
  if( $licznik['enab-active-low'][$i]=='yes' ) {
    $tresc.='<option selected="selected" value="yes">Niski</option>
      <option value="no">Wysoki</option>';
  } else {
    $tresc.='<option value="yes">Niski</option>
      <option selected="selected" value="no">Wysoki</option>';
  }

  $tresc.='</select>
    </td>';
}
$tresc.='</tr>
  <tr><th>Wykrywanie grubości</th>';
for( $i=0; $i<4; ++$i ) {
              $tresc.='<td><select name="thick['.$i.']" id="thick'.$i.'">';
  foreach( $icdtcp->urzadzenia as $wart=>$opis ) {
    $tresc.='<option';
    if( $wart==$licznik['thick'][$i] ) {
      $tresc.=' selected="selected"';
    }
    $tresc.=' value="'.$wart.'">'.$opis.'</option>';
  }
  $tresc.='</select>
    </td>';
}
$tresc.='</tr>
  <tr><th>i kierunku</th>
  ';
for( $i=0; $i<4; ++$i ) {
              $tresc.='<td>
  <select name="thick-detect-direction['.$i.']">';
  if( $licznik['thick-detect-direction'][$i]=='yes' ) {
    $tresc.='<option selected="selected" value="yes">Grubość i kierunek</option>
      <option value="no">Grubość</option>';
  } else {
    $tresc.='<option value="yes">Grubość i kierunek</option>
      <option selected="selected" value="no">Grubość</option>';
  }
  $tresc.='</select>

    </td>';
}
$tresc.='</tr>
  <tr><th>Stan aktywny wykrywania</th>';
for( $i=0; $i<4; ++$i ) {
                $tresc.='<td>
  <select name="thick-active-low['.$i.']">';
  if( $licznik['thick-active-low'][$i]=='yes' ) {
    $tresc.='<option selected="selected" value="yes">Niski</option>
      <option value="no">Wysoki</option>';
  } else {
    $tresc.='<option value="yes">Niski</option>
      <option selected="selected" value="no">Wysoki</option>';
  }
  $tresc.='</select>
    </td>';
}
$tresc.='</tr>';


$tresc.='</table><br><br>';

$tresc.='
  </table>
  <br>
  <input type="submit" value="Ustaw">
  <input type="reset" value="Anuluj">

  <input type="button" value="Test fotokomórek" onclick="window.open('."'".'popup.php?typ=test'."'".')">

  </form>

 */
$tresc.='</div>';


?>
