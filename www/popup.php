<?php

define( 'INSOFTER', 1 );

require_once('inc/c_icdtcp.php');


if( isset( $_GET['typ'] ) && $_GET['typ']=='transfer' )
{
  if( isset( $_GET['out'] ) ) {
    echo '<!DOCTYPE html><html lang="pl">
      <head>
      <meta http-equiv="content-type" content="text/html; charset=utf-8">
      <title>Test wysyłania</title>
      <meta http-equiv="refresh" content="5"></head>
      <body>
      <body><h3>Test połączenia</h3>
      <pre>'.file_get_contents( '/tmp/transfer-out' ).'</pre>
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
