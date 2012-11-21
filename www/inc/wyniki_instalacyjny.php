<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

// $wyniki=$icdtcp->raport_biezacy( time() );

$t=mktime( 0, 0, 0, date("n"), date("j"), date("Y") );


if( isset( $_POST['datepicker'] ) ) {
  $tab=explode( '.',  $_POST['datepicker'] );
  if( count( $tab ) == 3 ) {
    $t=mktime(0, 0, 0, $tab[1], $tab[2], $tab[0] );
  }
}

//echo date(DATE_RFC822, $t);



$wyniki=$icdtcp->raport_instalacyjny( $t );

//print_r( $wyniki );

$tresc='<div id="tresc">
  <h3>Wyniki</h3>
  <form action="./?strona=instalacyjny" method="POST">
  <h4>  <input type="text" id="datepicker" name="datepicker" value="'.date('Y.m.d', $t).'">
  <input type="submit" value="Ustaw"></h4>';

if( isset( $wyniki['counters'] ) ) {
  $tresc.='<table border=1>
    <tr>
    ';
  $tresc.='<td>id</td>';
  $tresc.='<td>data i czas</td>';
  $tresc.='<td>licznik</td>';
  $tresc.='<td>wartość</td>';
  $tresc.='<td>dark</td>';
  $tresc.='<td>work</td>';
  $tresc.='<td>flagi</td>';

  $tresc.='</tr>';

  $i=0;
  foreach( $wyniki['values'] as $row ) {
    $tresc.='<tr>';
    $tresc.='<td><small>'.number_format( $row[ 'id' ], 0, '', ',' ).'</small></td>';
    $tresc.='<td>'
      .date( 'Y.m.d', $row[ 'dtm' ] )
      .'&nbsp;&nbsp;'
      .date( 'H:i', $row[ 'dtm' ] )
      .'</td>';

    $tresc.='<td>'.$wyniki['counters'][ $row[ 'counter_id' ] ]
      .'&nbsp;&nbsp;<small>(id '.$row[ 'counter_id' ].')</small></td>';
    $tresc.='<td>'.number_format( $row[ 'cnt' ], 0, '', ',' ).'</td>';
    $tresc.='<td>'.number_format( $row[ 'dark_time' ], 0, '', ',' ).'</td>';
    $tresc.='<td>'.number_format( $row[ 'work_time' ], 0, '', ',' ).'</td>';
    $tresc.='<td>'.number_format( $row[ 'flags' ], 0, '', ',' ).'</td>';

    $tresc.='</tr>';
    if( ++$i>=16 ) {
      $tresc.='<tr>';
      $tresc.='<td>id</td>';
      $tresc.='<td>data i czas</td>';
      $tresc.='<td>licznik</td>';
      $tresc.='<td>wartość</td>';
      $tresc.='<td>dark</td>';
      $tresc.='<td>work</td>';
      $tresc.='<td>flagi</td>';
      $tresc.='</tr>';
      $i=0;
    }
/*  $tresc.='<tr>';
  foreach( $row as $val ) {
      $tresc.='<td>'.$val.'</td>';
  }
$tresc.='</tr>';*/
  }
/*
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
}*/


  $tresc.='</table>';
} else {
  $tresc.='<h4 id="err">Brak wyników dla '.date('d.m.Y', $t).'r.</h4>';
}
$tresc.='</div>';
?>
