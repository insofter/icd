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
                $log=strtr( file_get_contents( $this->katalog_logow.'/'.basename($nazwa), null, null, 0, 50000 ), $tab );

	        return(  $log );
	}
	function pobierz($nazwa) {
	        return( file_get_contents($this->katalog_logow.'/'.basename($nazwa)) );
	}
}
