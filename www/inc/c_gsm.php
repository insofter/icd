<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

class c_gsm {

  var $configDb;
  var $ICD_SHARE_DIR;

  function __construct() {
    foreach( file("/etc/profile.d/icd.sh", FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES) as $tmp ) {
      $tmp=explode( '=', strtr( $tmp, array('export '=>'', '$'=>'', '('=>'', ';'=>'', ) ) );
      eval( '$'.$tmp[ 0 ].'="'.$tmp[ 1 ].'";' );
    }
     try { 
       $this->configDb = new PDO('sqlite:'.$ICD_CONFIG_DB);
     } catch (PDOException $e) { 
       print $e->getMessage();
     } 
    $this->ICD_SHARE_DIR=$ICD_SHARE_DIR;
  }

  function disable() {
    $sql="UPDATE config SET `value`='no' WHERE section_id".
      "=(SELECT `id` FROM config_section WHERE name='gsm') AND `key`='enabled'";
    $ans=$this->configDb->query($sql);
  }

  function pob_modemy() {
    foreach( scandir( $this->ICD_SHARE_DIR.'/gsm' ) as $provider ) {
      if( $provider[0]!='.' ) {
        $modemy[]=$provider;
      }
    }
    return $modemy;
  }

  function pob_ustawienia() {
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id
      WHERE cs.name='gsm' AND ( c.key='enabled' OR c.key='pin-enabled' 
      OR c.key='pin' OR c.key='conf-dir' OR c.key='connect-with-transfer' )";
    $ans=$this->configDb->query($sql);
    foreach( $ans as $row ) {
      $ustawienia[$row['key']]=$row['value'];
    }
    return $ustawienia;
  }

  function ustaw($nowe) {
    foreach( $nowe as $pole=>$wart ) {
      $sql="UPDATE config SET `value`=".$this->configDb->quote($wart)."
        WHERE section_id=(SELECT `id` FROM config_section WHERE name='gsm')
        AND `key`=".$this->configDb->quote($pole);
      $this->configDb->query($sql);
    }
  }


}
