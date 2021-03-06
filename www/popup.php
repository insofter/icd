<?php

define( 'INSOFTER', 1 );

require_once('inc/c_icdtcp.php');


if( isset( $_GET['typ'] ) ) {
  if( $_GET['typ']=='transfer' ) {//transfer
    if( isset( $_GET['out'] ) ) {
      $out=file_get_contents( '/tmp/transfer-out' );

      if( false===strpos( $out, '<!--EOF-->' ) ) {
        $wait=true;
      } else {
        $wait=false;
      }
      echo '<!DOCTYPE html><html lang="pl">
        <head>
        <meta http-equiv="content-type" content="text/html; charset=utf-8">
        <title>Test wysyłania</title>';
      if( $wait ) {
        echo '<meta http-equiv="refresh" content="5">
          <style>
          html {
            background-image: url(img/wait.gif); 
            background-repeat: no-repeat;
            background-position: right top;
      }
      </style>

        ';
      }
      echo '</head>
        <body><h3>Test połączenia</h3>';
      echo '<pre>'.$out.'</pre>
        </body>
        </html>';
    } else {
      $icdtcp = new c_icdtcp();
      $icdtcp->test_wysylania();
      header('Location: popup.php?typ=transfer&out');
    }
  } else if( $_GET['typ']=='test' ) {//test foto
    $icdtcp = new c_icdtcp();
    echo '<!DOCTYPE html><html lang="pl">
      <head>
      <meta http-equiv="content-type" content="text/html; charset=utf-8">
      <title>Test fotokomórek</title>
      </head>
      <body><h3>Test fotokomórek</h3>';
    $out =  $icdtcp->test_fotokomorek();
    $info= $icdtcp->info();

    echo '<table border=1>';
    echo '<tr><th>Nazwa wejścia</th><th>Fotokomórka</th><th>Wynik testu</th></tr>';
    foreach( $out as $i=>$f ) {
      echo '<tr>';
      echo '<td>'.$info['liczniki'][$i]['name'].'</td>';
      if( $info['liczniki'][$i]['enabled'] == 'no' ) {
        echo '<td>'.str_replace(':FAILED','</td><td>DISABLED',$f).'</td>';
      } else {
        echo '<td>'.str_replace(':','</td><td>',$f).'</td>';
      }
      echo '</tr>'."\n";
    }
    echo '</table></body>
      </html>';
  } else if( $_GET['typ']=='reboot') {

    echo '<!DOCTYPE html><html lang="pl">
      <head>
      <meta http-equiv="content-type" content="text/html; charset=utf-8">
      <title>Restart urządzenia</title>
      <meta http-equiv="refresh" content="30;URL='."'./'".'">
      </head>
      <body><h3>Strona odświeży się w ciągu 30 sekund.</h3>
      </body></html>';
    ob_flush();
    flush();

    $icdtcp = new c_icdtcp();
    $icdtcp->reset();

  }
}

?>
