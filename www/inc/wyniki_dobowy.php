<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

$i=1000;

// $wyniki=$icdtcp->raport_biezacy( time() );

$t=(int)(time()/3600/24);
$t*=3600*24;

if( isset( $_POST['datepicker'] ) ) {
  $tab=explode( '.',  $_POST['datepicker'] );
  if( count( $tab ) == 3 ) {
    $t=mktime(0, 0, 0, $tab[1], $tab[0], $tab[2] );
  }
}

echo date(DATE_RFC822, $t);



$wyniki=$icdtcp->raport_od_do($t, $t+3600*24, 3600*4  );

print_r( $wyniki );

$tresc='<div id="tresc">
  <h3>Wyniki</h3>
  <form action="./?strona=wyniki&typ=dobowy" method="POST">
  <input type="text" id="datepicker" name="datepicker" value="'.date('d.m.Y', $t).'">
  <input type="submit" value="Ustaw">
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
