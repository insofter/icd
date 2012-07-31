<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

class c_wpa {

	public $polecenie;
	public $err;
	public $ook;
	
	function __construct() {
	        $this->polecenie='sudo icd-wpa';
 	}

	function start() {
		exec( $this->polecenie.' start' );
	}
		
	function stop() {
		exec( $this->polecenie.' stop' );
	}
	function stat() {
		exec( $this->polecenie.' stat', $out );
		$arr=array();
		foreach( $out as $x) {
			$t=explode('=', $x);
			$arr[ $t[0] ]=$t[1];
		}
		return $arr;
	}
	function lstn() {
		exec( $this->polecenie.' lstn', $out );
		$arr=array();
		foreach( $out as $x) {
//			print_r($x);
			$arr[]=explode("\t", $x);
		}
		return($arr);
	}
	function scan() {
		exec( $this->polecenie.' scan', $out );
	}
	function rscn() {
		exec( $this->polecenie.' rscn', $out );
		$arr=array();
		foreach( $out as $x) {
			$arr[]=explode("\t", strtr($x,array('< hidden >'=>'&lt; ukryta &gt;')));
		}
		return($arr);
	}
	function addn($net, $pas) {
		$net=preg_replace('#[^a-zA-Z0-9-_]+#', '', $net);
		$pas=preg_replace('#[^a-zA-Z0-9-_]+#', '', $pas);
		exec( $this->polecenie.' addn '.$net.' '.$pas, $out );
	}
	function deln($n) {
		exec( $this->polecenie.' deln '.((int)$n) );
	}
	function enab($n) {
		exec( $this->polecenie.' enab '.((int)$n) );
	}
	function disb($n) {
		exec( $this->polecenie.' disb '.((int)$n) );
	}
	function cfg_to_db() {
		exec( $this->polecenie.' cfg-to-db' );
	}
	function cfg_from_db() {
		exec( $this->polecenie.' cfg-from-db' );
	}
		
		
}
