<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

require_once('inc/c_logi.php');

$logi = new c_logi();

if( isset($_GET['log']) ) {

  $tresc='<div id="tresc">
    <h3>'.$_GET['log'].'</h3>
    <pre>'.$logi->log($_GET['log']).'</pre></div>';

} else if( isset($_GET['pobierz']) ) {

  header('Content-Type: application/octet-stream');
  header("Pragma: public");
  header("Expires: 0");
  header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
  header("Content-Type: text/plain");
  header('Content-Disposition: attachment; filename="'.$_GET['pobierz'].'"');

  ob_clean();

  flush();
  echo($logi->pobierz($_GET['pobierz']));
  exit;


} else {
  $tresc='<div id="tresc"><h3>Logi systemowe</h3>
    <ul class="log">';
  $lista=$logi->lista();
  foreach( $lista as $log ) {
    $tresc.='<li><a href="./?strona=log&amp;log='.$log.'">'.$log.'</a> &nbsp; &nbsp; &nbsp; &nbsp;
    <a href="./?strona=log&amp;pobierz='.$log.'"><small>(pobierz)</small></a> </li>';
  }
  $tresc.='</ul></div>';
  //print_r($logi->lista());

}

?>
