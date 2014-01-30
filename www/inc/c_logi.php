<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

class c_logi {

  public $katalog_logow;
  public $err;
  public $ook;

  function __construct() {
    $this->katalog_logow='/var/log';
    $this->err=array('WW', 'EE', 'WARNING', ':', 'Failed');
    $this->ook=array('++',  );
  }

  function linux() {
    $out='# free -m'."\n";
    $out.=shell_exec('free -m');

    $out.="\n\n".'# df -h'."\n";
    $out.=shell_exec('df -h');
    
    $out.="\n\n".'# top -n 1 -b'."\n";
    $out.=shell_exec('top -n 1 -b');
    return $out;
  }

  function lista() {
    $lista=scandir( $this->katalog_logow );
    for( $i=0; $i<count($lista); ++$i ) {
      if( $lista[$i]=='.' || $lista[$i]=='..'
        || is_dir( $this->katalog_logow.'/'.$lista[$i] ) ) {
          unset($lista[$i]);
        }
    }
    return($lista);
  }
  function log($nazwa) {
    $tab=array( '>'=>'&gt;', '<'=>'&lt;');

    foreach( $this->err as $e ) {
      $tab[$e]='<strong>'.$e.'</strong>';
    }
    foreach( $this->ook as $o ) {
      $tab[$o]='<em>'.$o.'</em>';
    }
    if( $nazwa == '__linux__' ) {
      $log=strtr( $this->linux(), $tab );
    } else {
      $log=strtr( file_get_contents( $this->katalog_logow.'/'.basename($nazwa), null, null, 0, 50000 ), $tab );
    }

    return(  $log );
  }
  function pobierz($nazwa) {
    if( $nazwa == '__linux__' ) {    
      return( $this->linux() );
    } else {
      return( file_get_contents($this->katalog_logow.'/'.basename($nazwa)) );
    }
  }
}
