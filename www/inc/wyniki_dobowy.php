<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

$i=1000;

// $wyniki=$icdtcp->raport_biezacy( time() );

$t=mktime( 0, 0, 0, date("n"), date("j"), date("Y") );
  

if( isset( $_POST['datepicker'] ) ) {
  $tab=explode( '.',  $_POST['datepicker'] );
  if( count( $tab ) == 3 ) {
    $t=mktime(0, 0, 0, $tab[1], $tab[0], $tab[2] );
  }
}

echo date(DATE_RFC822, $t);



$wyniki=$icdtcp->raport_od_do($t, $t+3600*24, 3600  );

//print_r( $wyniki );

$tresc='<div id="tresc">
  <h3>Wyniki</h3>
  <form action="./?strona=wyniki&typ=dobowy" method="POST">
<h4>  <input type="text" id="datepicker" name="datepicker" value="'.date('d.m.Y', $t).'">
  <input type="submit" value="Ustaw"></h4>
  <table border=1>
  <tr>
  <td>godzina&nbsp;\&nbsp;wejście</td>
  ';
  foreach( $wyniki['counters'] as $id=>$name ) {
    $tresc.='<td>'.$name.'</td>';
    $csum[ $id ]=0;
  }
  $tresc.='<td>sumy</td></tr>';

  foreach( $wyniki['values'] as $dtm=>$cnt ) {
    $tresc.='<tr><td>'.date( 'H',$dtm ).'<sup>'.date( 'i',$dtm ).'</sup> 
      - '.date( 'H', ( $dtm+3600 ) ).'<sup>'.date( 'i', ( $dtm+3600 ) ).'</sup></td>';

    $sum=0;
    foreach( $wyniki['counters'] as $id=>$name ) {

      if( isset( $cnt[ $id ] ) ) {
        $tresc.='<td>'.$cnt[ $id ].'</td>';
        $sum+=$cnt[ $id ];
        $csum[ $id ]+=$cnt[ $id ];

      } else {
        $tresc.='<td>--</td>';
      }
    }

    $tresc.='<td>'.$sum.'</td></tr>';
  }

  $tresc.='<tr><td>sumy</td>';

  $ccsum=0;
   
  foreach( $wyniki['counters'] as $id=>$name ) {
    $tresc.='<td>'.$csum[ $id ].'</td>';
    $ccsum+=$csum[ $id ];
  }


  $tresc.='<td>'.$ccsum.'</td></tr></table>
  </div>';
?>
