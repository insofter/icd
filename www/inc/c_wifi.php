<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

class c_wifi {

  var $configDb;
  function __construct() {
    $this->configDb=NULL;
  }

  function get_config() {
    if( $this->configDb==NULL ) {
      foreach( file("/etc/profile.d/icd.sh", FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES) as $tmp ) {
        $tmp=explode( '=', strtr( $tmp, array('export '=>'', '$'=>'', '('=>'', ';'=>'', ) ) );
        eval( '$'.$tmp[ 0 ].'="'.$tmp[ 1 ].'";' );
      }
      try {
        $this->configDb = new PDO('sqlite:'.$ICD_CONFIG_DB);
      } catch (PDOException $e) {
        print $e->getMessage();
      }
    }
    return $this->configDb;
  }

  function enabled() {

    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id
      WHERE cs.name='wifi' AND c.key='enabled' ";
    $ans=$this->get_config()->query($sql);
    foreach( $ans as $row ) {
      return $row['value'];
    }
  }

  function toggle($co) {
    if( $co!='no' ) {
      $co='yes';
    }
    $sql="UPDATE config SET `value`='$co' WHERE section_id".
      "=(SELECT `id` FROM config_section WHERE name='wifi') AND `key`='enabled'";
    $ans=$this->get_config()->query($sql);
  }







  function stat() {
    //exec( $this->polecenie.' stat', $out );
    exec( 'sudo wpa_cli -i wlan0 status', $out );
    $arr=array();
    foreach( $out as $x) {
      $t=explode('=', $x);
      $arr[ $t[0] ]=$t[1];
    }
    return $arr;
  }
  function lstn() {
    //exec( $this->polecenie.' lstn', $out );
    exec( 'sudo wpa_cli -i wlan0 list_networks | tail -n +2', $out );
    $arr=array();
    foreach( $out as $x) {
      //			print_r($x);
      $arr[]=explode("\t", $x);
    }
    return($arr);
  }
  function scan() {
    //    exec( $this->polecenie.' scan', $out );
    exec( 'sudo wpa_cli -i wlan0 scan', $out );
  }
  function rscn() {
    //    exec( $this->polecenie.' rscn', $out );
    exec( 'sudo wpa_cli -i wlan0 scan_results | tail -n +2', $out );
    $arr=array();
    foreach( $out as $x) {
      $arr[]=explode("\t", strtr($x,array('< hidden >'=>'&lt; ukryta &gt;')));
    }
    return($arr);
  }
  function addn($net, $pas) {
    $net=preg_replace('#[^a-zA-Z0-9-_]+#', '', $net);
    $pas=preg_replace('#[^a-zA-Z0-9-_]+#', '', $pas);
    //    exec( $this->polecenie.' addn '.$net.' '.$pas, $out );

    $num=exec( 'sudo wpa_cli -i wlan0 add_network', $out );
    if( $pas == '' ) {
      exec( 'sudo wpa_cli -i wlan0 set_network '.$num.' key_mgmt NONE' );
    } else {
      exec( 'sudo wpa_cli -i wlan0 set_network '.$num.' auth_alg OPEN');
      exec( 'sudo wpa_cli -i wlan0 set_network '.$num.' key_mgmt WPA-PSK');
      exec( 'sudo wpa_cli -i wlan0 set_network '.$num.' psk \"'.$pas.'\"');
      exec( 'sudo wpa_cli -i wlan0 set_network '.$num.' mode 0');
    }

    exec( 'sudo wpa_cli -i wlan0 set_network '.$num.' ssid \"'.$net.'\"' );
    exec( 'sudo wpa_cli -i wlan0 enable_network '.$num );
    exec( 'sudo wpa_cli -i wlan0 reassociate' );
    exec( 'sudo wpa_cli -i wlan0 save_config' );
    exec( 'icd-wifi-cfg to' );
  }
  function deln($n) {
    exec( 'sudo wpa_cli -i wlan0 remove_network '.((int)$n), $out );
    exec( 'sudo wpa_cli -i wlan0 save_config' );
    exec( 'icd-wifi-cfg to' );
  }
  function enab($n) {
    exec( 'sudo wpa_cli -i wlan0 enable_network '.((int)$n), $out );
    exec( 'sudo wpa_cli -i wlan0 save_config' );
    exec( 'icd-wifi-cfg to' );
  }
  function disb($n) {
    exec( 'sudo wpa_cli -i wlan0 disable_network '.((int)$n), $out );
    exec( 'sudo wpa_cli -i wlan0 save_config' );
    exec( 'icd-wifi-cfg to' );
  }

}
