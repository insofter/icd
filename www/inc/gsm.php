<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

require_once('inc/c_gsm.php');

$gsm=new c_gsm();
//$logi = new c_logi();


if( isset( $_POST['send'] ) ) {
  if( $_POST['send']=='Włącz' ) {
/*    if( isset( $_POST['pin-enabled'] ) && $_POST['pin']!=$_POST['pin2'] ) {
      $info='<h4 id="err">Kody PIN nie są zgodne</h4>';
    } else {
      $t=explode( '*', $_POST['modem'] );
      $ustawienia['provider']=$t[0];
      $ustawienia['modem']=$t[1];
      $ustawienia['pin']=$_POST['pin'];
      if( isset( $_POST['pin-enabled'] ) ) {
        $ustawienia['pin-enabled']='yes';
      } else {
        $ustawienia['pin-enabled']='no';
      }*/
      $ustawienia['enabled']='yes';

      $gsm->ustaw( $ustawienia );
      $info='<h4>Włączono moduł GSM</h4>';
    }
  } else {
    $gsm->disable();
    $info='<h4>Wyłączono moduł GSM</h4>';
  }
}


$ustawienia=$gsm->pob_ustawienia();

$modemy=$gsm->pob_modemy();

$tresc='<div id="tresc"><h3>GSM</h3>';
if( isset( $info ) ) {
  $tresc.=$info;
}
$tresc.='<form action="./?strona=gsm" method="POST">
  <table>
  <tr><th>Stan modułu GSM : </th><th>';
if( $ustawienia['enabled']=='yes' ) {
  $tresc.='aktywny';
} else {
  $tresc.='nieaktywny';
}

$tresc.='</th></tr><tr><th><label for="modem">Wybierz modem i dostawcę</label> :</th><td>
  <select name="modem" id="modem">';
foreach( $modemy as $provider=>$modem ) {
  $tresc.='<optgroup label="'.$provider.'">';
  foreach( $modem as $typ ) {
    if( $provider==$ustawienia['provider'] && $typ==$ustawienia['modem'] ) {
      $tresc.='<option value="'.$provider.'*'.$typ.'" selected="selected">'.$typ.' ('.$provider.')</option>';
    } else {
      $tresc.='<option value="'.$provider.'*'.$typ.'">'.$typ.' ('.$provider.')</option>';
    }
  }
  $tresc.='</optgroup>';
}
$tresc.='</select></td></tr>
  <tr><th><label for="pin-enabled">Włącz PIN</label> :</th><td>
  <input type="checkbox" name="pin-enabled" value="pin-enabled" id="pin-enabled"';
if( $ustawienia['pin-enabled']=='yes' ) {
  $tresc.='checked="checked"';
}

$tresc.='>
  <tr><th><label for="pin">PIN</label> :</th><td>
  <input type="text" name="pin" id="pin" value="'.$ustawienia['pin'].'" size="6"></td></tr>
  <tr><th><label for="pin2">powtórz PIN</label> :</th><td>
  <input type="text" name="pin2" id="pin2" value="*pin*" size="6"></td></tr>
  <tr><th colspan="2">
  <input type="submit" name="send" value="Wyłącz">
  <input type="submit" name="send" value="Włącz">
  </th></tr>
  </table>
  </form>';

$tresc.='</div>';


?>
