<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

/*print_r($_POST);*/

$icdtcp = new c_icdtcp();

if( isset($_POST['ids']) ) {
  $nowe=array('ids'=>$_POST['ids'],
    'idd'=>$_POST['idd'],
    'address'=>$_POST['address'],
    'user'=>$_POST['user'],
    'pass'=>$_POST['pass'],
    'aggr-period-mins'=>$_POST['aggr-period-mins'],
    'server-sync-period-mins'=>$_POST['server-sync-period-mins'],
    'event-retention-period-count'=>$_POST['event-retention-period-count'],
    'test-retention-period-mins'=>$_POST['test-retention-period-mins'],
    'flow-entry-retention-period-mins'=>$_POST['flow-entry-retention-period-mins']);
  if( isset( $_POST['transfer-enabled'] ) ) {
    $nowe['transfer-enabled']='yes';
  } else {
    $nowe['transfer-enabled']='no';
  }
  $icdtcp->wysylanie_ustaw($nowe);
  $info='<h4>Zmodyfikowano konfigurację</h4>';
} else {
  $info='';
}

$wysylanie=$icdtcp->wysylanie_pobierz();

$tresc='<div id="tresc"><form action="./?strona=wysylanie" method="POST">
  <h3>Konfiguracja parametrów wysyłania</h3>
  '.$info.'
  <table>
  <tr><th><label for="ids">ids</label> :</th><td>
  <input type="text" name="ids" id="ids" value="'.$wysylanie['ids'].'" size="40"></td></tr>
  <tr><th><label for="idd">idd</label> :</th><td>
  <input type="text" name="idd" id="idd" value="'.$wysylanie['idd'].'" size="40"></td></tr>
  <tr><th><label for="address">address</label> :</th><td>
  <input type="text" name="address" id="address" value="'.$wysylanie['address'].'" size="40"></td></tr>
  <tr><th><label for="user">user</label> :</th><td>
  <input type="text" name="user" id="user" value="'.$wysylanie['user'].'" size="40"></td></tr>
  <tr><th><label for="pass">pass</label> :</th><td>
  <input type="password" name="pass" id="pass" value="'.$wysylanie['pass'].'" size="40"></td></tr>


  <tr><th><label for="transfer-enabled">transfer-enabled</label> :</th><td>
  <input type="checkbox" name="transfer-enabled" id="transfer-enabled"';
if( $wysylanie['transfer-enabled']=='yes' ) {
  $tresc.=' checked="checked"';
}
$tresc.='><br>
  <tr><th><label for="aggr-period-mins">aggr-period-mins</label> :</th><td>
  <select name="aggr-period-mins" id="aggr-period-mins">';
foreach( $icdtcp->czas as $wart=>$opis ) {
  $tresc.='<option';
  if( $wart==$wysylanie['aggr-period-mins'] ) {
    $tresc.=' selected="selected"';
  }
  $tresc.=' value="'.$wart.'">'.$opis.'</option>';
}
$tresc.='</select></td></tr>
  <tr><th><label for="server-sync-period-mins">server-sync-period-mins</label> :</th><td>
  <select name="server-sync-period-mins" id="server-sync-period-mins">';
foreach( $icdtcp->czas as $wart=>$opis ) {
  $tresc.='<option';
  if( $wart==$wysylanie['server-sync-period-mins'] ) {
    $tresc.=' selected="selected"';
  }
  $tresc.=' value="'.$wart.'">'.$opis.'</option>';
}
$tresc.='</select></td></tr>
  <tr><th><label for="test-retention-period-mins">test-retention-period-mins</label> :</th><td>
  <select name="test-retention-period-mins" id="test-retention-period-mins">';
foreach( $icdtcp->czas as $wart=>$opis ) {
  $tresc.='<option';
  if( $wart==$wysylanie['test-retention-period-mins'] ) {
    $tresc.=' selected="selected"';
  }
  $tresc.=' value="'.$wart.'">'.$opis.'</option>';
}
$tresc.='</select></td></tr>
  <tr><th><label for="event-retention-period-count">event-retention-period-count</label> :</th><td>
  <select name="event-retention-period-count" id="event-retention-period-count">';
foreach( $icdtcp->ilosc as $wart=>$opis ) {
  $tresc.='<option';
  if( $wart==$wysylanie['event-retention-period-count'] ) {
    $tresc.=' selected="selected"';
  }
  $tresc.=' value="'.$wart.'">'.$opis.'</option>';
}
$tresc.='</select></td></tr>
  <tr><th><label for="flow-entry-retention-period-mins">flow-entry-retention-period-mins</label> :</th><td>
  <select name="flow-entry-retention-period-mins" id="flow-entry-retention-period-mins">';
foreach( $icdtcp->czas_dlugi as $wart=>$opis ) {
  $tresc.='<option';
  if( $wart==$wysylanie['flow-entry-retention-period-mins'] ) {
    $tresc.=' selected="selected"';
  }
  $tresc.=' value="'.$wart.'">'.$opis.'</option>';
}
$tresc.='</select></td></tr>
  <tr><th><input type="submit" value="Ustaw">
  <input type="reset" value="Anuluj">
  <input type="button" value="Test wysyłania" onclick="location.href='."'./?strona=test_wysylania'".'"></th><td>&nbsp;

</td></tr>
  </table></form></div>';


?>
