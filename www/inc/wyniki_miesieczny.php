<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

$t=mktime( 0, 0, 0, date("n"), date("j"), date("Y") );
  

if( isset( $_POST['datepicker'] ) ) {
  $tab=explode( '.',  $_POST['datepicker'] );
  if( count( $tab ) == 3 ) {
    $t=mktime(0, 0, 0, $tab[1], $tab[2], $tab[0] );
  }
}




$wyniki=$icdtcp->raport_miesieczny( $t );

//print_r( $wyniki );

$tresc='<div id="tresc">
  <h3>Wyniki</h3>
  <form action="./?strona=miesieczny" method="POST">
<h4>  <input type="text" id="datepicker" name="datepicker" value="'.date('Y.m.d', $t).'">
  <input type="submit" value="Ustaw"></h4>';

if( isset( $wyniki['counters'] ) ) {
$tresc.='<table border=1>
  <tr>
  <td>data&nbsp;\&nbsp;wejście</td>
  ';
  foreach( $wyniki['counters'] as $id=>$name ) {
    $tresc.='<td>'.$name.'</td>';
    $csum[ $id ]=0;
  }
  $tresc.='<td>sumy</td></tr>';

  foreach( $wyniki['values'] as $dtm=>$cnt ) {
    $tresc.='<tr><td>'.date( 'm.d', $dtm+3600 ).'</td>';

    $sum=0;
    foreach( $wyniki['counters'] as $id=>$name ) {

      if( isset( $cnt[ $id ] ) ) {
        $tresc.='<td>'.number_format( $cnt[ $id ], 0, '', ',' ).'</td>';
        $sum+=$cnt[ $id ];
        $csum[ $id ]+=$cnt[ $id ];

      } else {
        $tresc.='<td>--</td>';
      }
    }

    $tresc.='<td>'.number_format( $sum, 0, '', ',' ).'</td></tr>';
  }

  $tresc.='<tr><td>sumy</td>';

  $ccsum=0;
   
  foreach( $wyniki['counters'] as $id=>$name ) {
    $tresc.='<td>'.number_format( $csum[ $id ], 0, '', ',' ).'</td>';
    $ccsum+=$csum[ $id ];
  }


  $tresc.='<td>'.number_format( $ccsum, 0, '', ',' ).'</td></tr></table>';
} else {
$tresc.='<h4 id="err">Brak wyników dla '.date('Y.m', $t).'r.</h4>';
}
  $tresc.='</div>';
?>
