<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

$i=1000;

$wyniki=$icdtcp->wyniki();

$tresc='<div id="tresc">
  <h3>Wyniki</h3>
  <table>
  ';
foreach( $wyniki as $wynik ) {
  if( $i>20 ) {
    $tresc.='<tr>';
    foreach( $wynik as $key=>$val ) {
      $tresc.='<th>'.$key.'</th>';
    }
    $tresc.='</tr>
      ';
    $i=0;
  }
  $tresc.='<tr>';
  foreach( $wynik as $val ) {
    $tresc.='<td>'.$val.'</td>';
  }
  $tresc.='</tr>
    ';
  ++$i;
}



$tresc.='</table>
  </div>';
?>
