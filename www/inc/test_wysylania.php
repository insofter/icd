<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

$tytul="Test wysyłania";

$tresc='<div id="tresc">
  <h3>Test wysyłania</h3>
  <pre>'.$icdtcp->test_wysylania().'</pre></div>';

?>
