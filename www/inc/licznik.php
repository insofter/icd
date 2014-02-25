<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();
// print_r($_POST);

if( isset( $_POST['sect'] ) ) {


  $licznik['name']=$_POST['name'];
  $licznik['enabled']=$_POST['enabled'];
  $licznik['slave']=$_POST['slave'];
  $licznik['slave-mode']=$_POST['slave-mode'];
  $licznik['slave-engage']=$_POST['slave-engage'];
  $licznik['slave-release']=$_POST['slave-release'];
  $licznik['slave-reversed']=$_POST['slave-reversed'];
  $licznik['master']=$_POST['master'];
  $licznik['master-engage']=$_POST['master-engage'];
  $licznik['master-release']= $_POST['master-release'];
  $licznik['master-reversed']=$_POST['master-reversed'];
  $licznik['stop']= $_POST['stop'];
  $licznik['stop-engage']= $_POST['stop-engage'];
  $licznik['stop-release']= $_POST['stop-release'];
  $licznik['stop-reversed']=$_POST['stop-reversed'];

  if( substr( $_POST['sect'], 0, 7 )== 'counter' ) {
    $icdtcp->liczniki_ustaw( $_POST['sect'], $licznik );
    $info='<h4>Zmodyfikowano konfigurację</h4>';
  }  else {
    $info='';
  }
} else if( isset( $_POST['master-counter_id'] ) ) {

  foreach( $_POST['master-counter_id'] as $sect=>$id ) {
    $ledy[$sect]['master-counter_id']=$id;
  }
  $icdtcp->ledy_ustaw( $ledy );
  $info='<h4>Zmodyfikowano konfigurację</h4>';

} else if( isset( $_POST['restart'] ) && $_POST['restart']=='Restart modułu liczącego' ) {
  $icdtcp->liczniki_restart();
  $info='<h4>Uruchomiono ponownie moduł liczący</h4>';
} else {
  $info='';
}
$liczniki=$icdtcp->liczniki_pobierz();
$ledy=$icdtcp->ledy_pobierz();




$tresc='<div id="tresc">
  <h3>
  <form action="./?strona=licznik" method="POST">
  Ustawienia licznika &nbsp; &nbsp; &nbsp; 
  <input type="button" value="Test fotokomórek" onclick="window.open('."'".'popup.php?typ=test'."'".')">
  &nbsp; &nbsp;
  <input type="submit" value="Restart modułu liczącego" name="restart">
  </form>
  </h3>
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
function selectForm( $sect, $name, $label, $selected, $options_array ) {
  $ret='<label for="'.$name.'_'.$sect.'">'.$label.' </label>
    <select name="'.$name.'" id="'.$name.'_'.$sect.'">
    ';
  foreach( $options_array as $opt_val=>$opt_label ) {
    $ret.='<option';
    if( $opt_val == $selected ) {
      $ret.=' selected="selected"';
    }
    $ret.=' value="'.$opt_val.'">'.$opt_label.'</option>
      ';
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
  $liczniki_led_lista[$licznik['counter_id']]='Licz.: '.$licznik['counter_id'].' - '.$licznik['name'];

  $tresc.='<form action="./?strona=licznik" method="POST">
    <input type="hidden" value="'.$sect.'" name="sect">';

  $tresc.='<table class="liczniki">
    <tr><td class="liczniki_tab liczniki_top" colspan="3">Id licznika: '.$licznik['counter_id'].'
    <input type="submit" value="Modyfikuj"></td>
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
    .selectForm( $sect, 'slave', 'Wejście dodatkowe:', $licznik['slave'], $icdtcp->urzadzenia ).
    '</td></tr>
    <tr><td>'
    .selectForm( $sect, 'slave-mode', 'Tryb pracy:', $licznik['slave-mode'], $icdtcp->tryby_licznika ).
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
    .selectForm( $sect, 'master', 'Wejście główne:', $licznik['master'], $icdtcp->urzadzenia ).
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
    .selectForm( $sect, 'stop', 'Wyłącznik czasowy:', $licznik['stop'], $icdtcp->urzadzenia ).
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

$tresc.='<form action="./?strona=licznik" method="POST">
  <table><tr class="liczniki_tab">
  <td class="liczniki_tab">Konfiguracja diod 
  <input type="submit" value="Modyfikuj">';


foreach( $ledy as $led=>$params ) {
  $tresc.='<tr class="liczniki_tab"><td class="liczniki_tab">'
    .selectForm( $led, 'master-counter_id['.$led.']', 'Led '.chr($led[3]+ord('A')).':', $params['master-counter_id'], $liczniki_led_lista ).
    '</td></tr>';
}
  $tresc.='</table></form>';





$tresc.='</div>';


?>
