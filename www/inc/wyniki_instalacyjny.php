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

if( isset( $wyniki['data'] ) ) {
  if( isset( $wyniki['data']['counters'] ) ) {
    $tresc.='<table border=1>
      <tr><td colspan="8">Data.db</td></tr>
      <tr>
      ';
    $tresc.='<td>id</td>';
    $tresc.='<td>data i czas</td>';
    $tresc.='<td>licznik</td>';
    $tresc.='<td>wartość</td>';
    $tresc.='<td>dark</td>';
    $tresc.='<td>work</td>';
    $tresc.='<td>test</td>';
    $tresc.='<td>flagi</td>';

    $tresc.='</tr>';

    $i=0;
    foreach( $wyniki['data']['values'] as $row ) {
      $tresc.='<tr>';
      $tresc.='<td><small>'.number_format( $row[ 'id' ], 0, '', ',' ).'</small></td>';
      $tresc.='<td>'
        .date( 'Y.m.d', $row[ 'dtm' ] )
        .'&nbsp;&nbsp;'
        .date( 'H:i', $row[ 'dtm' ] )
        .'</td>';

      $tresc.='<td>'.$wyniki['data']['counters'][ $row[ 'counter_id' ] ]
        .'&nbsp;&nbsp;<small>(id '.$row[ 'counter_id' ].')</small></td>';
      $tresc.='<td>'.number_format( $row[ 'cnt' ], 0, '', ',' ).'</td>';
      $tresc.='<td>'.number_format( $row[ 'dark_time' ], 0, '', ',' ).'</td>';
      $tresc.='<td>'.number_format( $row[ 'work_time' ], 0, '', ',' ).'</td>';
      $tresc.='<td>'.number_format( $row[ 'test' ], 0, '', ',' ).'</td>';
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
        $tresc.='<td>test</td>';
        $tresc.='<td>flagi</td>';
        $tresc.='</tr>';
        $i=0;
      }
    }
    $tresc.='</table><br><br>';
  }

  if( isset( $wyniki['live']['counters'] ) ) {
    $tresc.='<table border=1>
      <tr><td colspan="8">Live.db</td></tr>
      <tr>
      ';
    $tresc.='<td>id</td>';
    $tresc.='<td>data i czas</td>';
    $tresc.='<td>licznik</td>';
    $tresc.='<td>wartość</td>';
    $tresc.='<td>dark</td>';
    $tresc.='<td>work</td>';
    $tresc.='<td>test</td>';
    $tresc.='<td>flagi</td>';

    $tresc.='</tr>';

    $i=0;
    foreach( $wyniki['live']['values'] as $row ) {
      $tresc.='<tr>';
      $tresc.='<td><small>'.number_format( $row[ 'id' ], 0, '', ',' ).'</small></td>';
      $tresc.='<td>'
        .date( 'Y.m.d', $row[ 'dtm' ] )
        .'&nbsp;&nbsp;'
        .date( 'H:i', $row[ 'dtm' ] )
        .'</td>';

      $tresc.='<td>'.$wyniki['live']['counters'][ $row[ 'counter_id' ] ]
        .'&nbsp;&nbsp;<small>(id '.$row[ 'counter_id' ].')</small></td>';
      $tresc.='<td>'.number_format( $row[ 'cnt' ], 0, '', ',' ).'</td>';
      $tresc.='<td>'.number_format( $row[ 'dark_time' ], 0, '', ',' ).'</td>';
      $tresc.='<td>'.number_format( $row[ 'work_time' ], 0, '', ',' ).'</td>';
      $tresc.='<td>'.number_format( $row[ 'test' ], 0, '', ',' ).'</td>';
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
        $tresc.='<td>test</td>';
        $tresc.='<td>flagi</td>';
        $tresc.='</tr>';
        $i=0;
      }
    }
    $tresc.='</table>';
  }



} else {
  $tresc.='<h4 id="err">Brak wyników dla '.date('d.m.Y', $t).'r.</h4>';
}
$tresc.='</div>';
?>
