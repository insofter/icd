<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

$i=1000;

// $wyniki=$icdtcp->raport_biezacy( time() );

$t=(int)(time()/3600/24);
$t*=3600*24;


$wyniki=$icdtcp->raport_od_do($t, $t+3600*24, 3600*4  );

print_r( $wyniki );

$tresc='<div id="tresc">
  <h3>Wyniki</h3>
  <table>
  ';
/*foreach( $wyniki as $wynik ) {
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

 */

$tresc.='</table>
  </div>';
?>
