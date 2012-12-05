<?php

define( 'INSOFTER', 1 );

require_once('inc/c_icdtcp.php');


if( isset( $_GET['typ'] ) && $_GET['typ']=='transfer' )
{
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
} else if( isset( $_GET['typ'] ) && $_GET['typ']=='test' ) {
  $icdtcp = new c_icdtcp();
  echo '<!DOCTYPE html><html lang="pl">
    <head>
    <meta http-equiv="content-type" content="text/html; charset=utf-8">
    <title>Test fotokomórek</title>
    <body><h3>Test fotokomórek</h3>';
  echo $icdtcp->test_fotokomorek();
  echo '</body>
    </html>';
}

?>
