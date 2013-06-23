<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

class c_icdtcp {

  public $czas;
  public $czas_dlugi;
  public $ilosc;
  public $o_mnie;

  private $dataDb;
  private $configDb;
  private $liveDb;
  private $ICD_DATA_DIR;

  function __construct() {
    $this->o_mnie=array('nazwa'=>'c_icdtcp',
      'plik'=>'inc/c_icdtcp.php',
      'wersja_rok'=>'2012',
      'wersja_miesiac'=>'07');
    $this->czas=array( 1=>'1 minuta',
      2=>'2 minuty',
      5=>'5 minut',
      10=>'10 minut',
      15=>'15 minut',
      20=>'20 minut',
      30=>'30 minut',
      60=>'1 godzina',
      120=>'2 godziny',
      240=>'4 godziny',
      360=>'6 godzin',
      480=>'8 godzin',
      720=>'12 godzin',
      1440=>'24 godziny');
    $this->czas_dni=array( 1=>'1 dzień',
      2=>'2 dni',
      3=>'3 dni',
      7=>'7 dni',
      14=>'14 dni',
      30=>'30 dni',
      90=>'3 miesiące',
      180=>'6 miesięcy',
      366=>'12 miesięcy',
      732=>'2 lata',
      1098=>'3 lata',
      1464=>'4 lata',
      1830=>'5 lat',
      3660=>'10 lat');
    $this->ilosc=array( 1000=>'1 000',
      10000=>'10 000',
      100000=>'100 000',
      250000=>'250 000',
      500000=>'500 000',
      1000000=>'1 000 000');
    $this->urzadzenia=array( '/dev/itd0'=>'A',
      '/dev/itd1'=>'B',
      '/dev/itd2'=>'C',
      '/dev/itd3'=>'D',
      ''=>'Brak' );
    $this->tryby_licznika=array( 'single'=>'Pojedynczy',
      'direction'=>'Wyk. kierunku',
      'thickness'=>'Wyk. długich obiektów',
      'nothick'=>'Wyk. krótkich obiektów' );




    foreach( file("/etc/profile.d/icd.sh", FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES) as $tmp ) {
      $tmp=explode( '=', strtr( $tmp, array('export '=>'', '$'=>'', '('=>'', ';'=>'', ) ) );
      eval( '$'.$tmp[ 0 ].'="'.$tmp[ 1 ].'";' );
    }


    try {
      $this->dataDb = new PDO('sqlite:'.$ICD_DATA_DB);
      $this->configDb = new PDO('sqlite:'.$ICD_CONFIG_DB);
      $this->liveDb = new PDO('sqlite:'.$ICD_LIVE_DB);
    } catch (PDOException $e) {
      print $e->getMessage();
    }
    $this->ICD_DATA_DIR=$ICD_DATA_DIR;
  }
  function zaloguj( $pass ) {
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c on cs.id=c.section_id
      WHERE cs.name='login' AND (c.key='enabled' OR c.key='pass')";
    $ans=$this->configDb->query($sql);

    foreach( $ans as $row ) {
      $login[$row['key']]=$row['value'];
    }
    if( $login['enabled']=='no' ) {
      return 1;
    }
    if( $login['pass']==$pass ) {
      return 2;
    }
    return 0;
  }
  function ustaw_logowanie( $npass, $pass, $enab ) {
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c on cs.id=c.section_id
      WHERE cs.name='login' AND (c.key='pass')";
    $ans=$this->configDb->query($sql);
    $row=$ans->fetch();

    if( $pass==$row['value'] ) {
      $sql="BEGIN TRANSACTION";
      $ans=$this->configDb->query($sql);

      $sql="UPDATE config SET `value`=".$this->configDb->quote($npass)."
        WHERE section_id=(SELECT `id` FROM config_section WHERE name='login')
        AND `key`='pass'";
      $ans=$this->configDb->query($sql);

      if( $enab!='no' ) {
        $enab='yes';
      }
      $sql="UPDATE config SET `value`='$enab' WHERE section_id".
        "=(SELECT `id` FROM config_section WHERE name='login') AND `key`='enabled'";
      $ans=$this->configDb->query($sql);

      $sql="COMMIT TRANSACTION";
      $ans=$this->configDb->query($sql);

      return true;
    }
    return false;
  }

  function pob_logowanie() {
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c on cs.id=c.section_id
      WHERE cs.name='login' AND (c.key='enabled')";
    $ans=$this->configDb->query($sql);

    foreach( $ans as $row ) {
      $login[$row['key']]=$row['value'];
    }
    if( $login['enabled']=='no' ) {
      return false;
    } else {
      return true;
    }
  }

  function info() {
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c on cs.id=c.section_id
      WHERE cs.name='device' AND (c.key='idd' OR c.key='ids' OR c.key='address')";
    $ans=$this->configDb->query($sql);
    foreach( $ans as $row ) {
      $info[$row['key']]=$row['value'];
    }


    $sql="SELECT name FROM config_section WHERE name LIKE 'counter%'";
    $ans=$this->configDb->query($sql);
    $ans->setFetchMode(PDO::FETCH_ASSOC);

    foreach( $ans as $row ) {
      $sql2="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c on cs.id=c.section_id
        WHERE cs.name='".$row['name']."' AND (c.key='name' OR c.key='counter_id' OR c.key='enabled' ) ";
      $ans2=$this->configDb->query($sql2);
      $ans2->setFetchMode(PDO::FETCH_ASSOC);
      foreach( $ans2 as $row2 ) {
        $licznik[$row2['key']]=$row2['value'];
      }

      $sql2="SELECT `cnt`, `dtm` FROM flow WHERE counter_id='".$licznik['counter_id']."' ORDER BY `dtm` DESC LIMIT 1";


      $ans2=$this->liveDb->query($sql2);
      $ans2->setFetchMode(PDO::FETCH_ASSOC);

      $licznik['cnt_last']=0;
      if( $ans2!=NULL && $row2=$ans2->fetch() ) {
        $licznik['cnt_last']=$row2['cnt'];
        $start=$row2['dtm'];
      } else {
        $licznik['cnt_last']=0;
        $start=time();
      }
      $sql2="SELECT sum(cnt) AS sum FROM flow WHERE counter_id='".$licznik['counter_id']."'
        AND `dtm` < ".$start." AND `dtm` > ".( ((int)(time()/(24*3600)))*24*3600 );
      $ans2=$this->dataDb->query($sql2);
      $ans2->setFetchMode(PDO::FETCH_ASSOC);

      $licznik['cnt_sum']=0;
      if( $ans2!=NULL && $row2=$ans2->fetch() ) {
        $licznik['cnt_sum']=(int)$row2['sum']+$licznik['cnt_last'];
      } else {
        $licznik['cnt_sum']=$licznik['cnt_last'];
      }
      $liczniki[]=$licznik;
    }

    $info['liczniki']=$liczniki;
    return $info;
  }

  function tcpip_pobierz($typ) {
    if( $typ!='current' ) {

      $typ='tcpip';
      $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id
        WHERE cs.name='$typ' AND (c.key='dns1' OR c.key='dns2' OR c.key='gate' OR
        c.key='ip' OR c.key='mask' OR c.key='net-name' OR c.key='mac')";
      $ans=$this->configDb->query($sql);
      foreach( $ans as $row ) {
        $tcp[$row['key']]=$row['value'];
      }
    } else {
      $list=array('ip', 'mask', 'gate', 'dns1', 'dns2', 'mac');
      foreach( $list as $item ) {
        $tcp[$item]=shell_exec('icd-current eth '.$item);
      }

    }
    return $tcp;

  }
  function dhcp_pobierz() {
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id
      WHERE cs.name='tcpip' AND c.key='dhcp' ";
    $ans=$this->configDb->query($sql);
    foreach( $ans as $row ) {
      $tcp[$row['key']]=$row['value'];
    }
    return $tcp;
  }
  function dhcp_ustaw($co) {
    if( $co!='no' ) {
      $co='yes';
    }
    $sql="UPDATE config SET `value`='$co' WHERE section_id".
      "=(SELECT `id` FROM config_section WHERE name='tcpip') AND `key`='dhcp'";
    $ans=$this->configDb->query($sql);
  }

  function dnsmasq_pobierz() {
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id
      WHERE cs.name='tcpip' AND c.key='dnsmasq' ";
    $ans=$this->configDb->query($sql);
    foreach( $ans as $row ) {
      $tcp[$row['key']]=$row['value'];
    }
    return $tcp;
  }
  function dnsmasq_ustaw($co) {
    if( $co!='no' ) {
      $co='yes';
    }
    $sql="UPDATE config SET `value`='$co' WHERE section_id".
      "=(SELECT `id` FROM config_section WHERE name='tcpip') AND `key`='dnsmasq'";
    $ans=$this->configDb->query($sql);
  }

  function tcpip_ustaw($nowe) {

    $sql="BEGIN TRANSACTION";
    $ans=$this->configDb->query($sql);

    foreach( $nowe as $pole=>$wart ) {
      $sql="UPDATE config SET `value`=".$this->dataDb->quote($wart)."
        WHERE section_id=(SELECT `id` FROM config_section WHERE name='tcpip')
        AND `key`=".$this->configDb->quote($pole);
      $this->configDb->query($sql);
    }

    $sql="COMMIT TRANSACTION";
    $ans=$this->configDb->query($sql);
  }

  function liczniki_pobierz() {

    $sql="SELECT `name`, `key`, `value` FROM config_section cs LEFT JOIN config c on cs.id=c.section_id
      WHERE cs.name LIKE 'counter%' ";

    $ans=$this->configDb->query($sql);
    $ans->setFetchMode(PDO::FETCH_ASSOC);

    foreach( $ans as $row ) {
      $liczniki[$row['name']][$row['key']]=$row['value'];
    }
    return $liczniki;
  }
  function liczniki_ustaw($sect, $nowy) {

    $sql="BEGIN TRANSACTION";
    $ans=$this->configDb->query($sql);

    foreach( $nowy as $pole=>$wart ) {
      $sql="UPDATE config SET `value`=".$this->configDb->quote($wart)."
        WHERE section_id=(SELECT `id` FROM config_section WHERE name="
        .$this->configDb->quote($sect).")
        AND `key`=".$this->configDb->quote($pole);
      $this->configDb->query($sql);
    }
    $sql="COMMIT TRANSACTION";
    $ans=$this->configDb->query($sql);
  }

  function ledy_pobierz() {

    $sql="SELECT `name`, `key`, `value` FROM config_section cs LEFT JOIN config c on cs.id=c.section_id
      WHERE cs.name LIKE 'led%' ";

    $ans=$this->configDb->query($sql);
    $ans->setFetchMode(PDO::FETCH_ASSOC);

    foreach( $ans as $row ) {
      $liczniki[$row['name']][$row['key']]=$row['value'];
    }
    return $liczniki;
  }

  function ledy_ustaw($tab) {

    $sql="BEGIN TRANSACTION";
    $ans=$this->configDb->query($sql);
    foreach( $tab as $sect=>$nowy ) {

      foreach( $nowy as $pole=>$wart ) {
        $sql="UPDATE config SET `value`=".$this->configDb->quote($wart)."
          WHERE section_id=(SELECT `id` FROM config_section WHERE name="
          .$this->configDb->quote($sect).")
          AND `key`=".$this->configDb->quote($pole);
        $this->configDb->query($sql);
      }
    }
    $sql="COMMIT TRANSACTION";
    $ans=$this->configDb->query($sql);
  }

  function wysylanie_pobierz()
  {
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id
      WHERE cs.name='device' AND ( c.key='ids' OR c.key='idd' OR c.key='address'
      OR c.key='user' OR c.key='transfer-enabled' OR c.key='aggr-period-mins'
      OR c.key='event-retention-period-count' OR c.key='flow-entry-retention-period-days'
      OR c.key='pass' OR c.key='server-sync-period-mins' OR c.key='test-retention-period-mins' )";
    $ans=$this->configDb->query($sql);
    foreach( $ans as $row ) {
      $tcp[$row['key']]=$row['value'];
    }
    return $tcp;
  }
  function wysylanie_ustaw($nowe) {
    $sql="BEGIN TRANSACTION";
    $ans=$this->configDb->query($sql);

    foreach( $nowe as $pole=>$wart ) {
      $sql="UPDATE config SET `value`=".$this->configDb->quote($wart)."
        WHERE section_id=(SELECT `id` FROM config_section WHERE name='device')
        AND `key`=".$this->configDb->quote($pole);
      $this->configDb->query($sql);
    }
    $sql="COMMIT TRANSACTION";
    $ans=$this->configDb->query($sql);
  }
  function konfiguracja_pobierz() {
    $cfg['__dane_eksportu']['eksport']='';
    $cfg['__dane_eksportu']['skrypt']='';
    $cfg['__dane_eksportu']['klasa']=$this->o_mnie;

    $sql="SELECT `key`, `value`, `name`  FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id";
    $ans=$this->configDb->query($sql);
    foreach( $ans as $row ) {
      $cfg[ $row['name'] ][ $row['key'] ] = $row['value'];
    }
    return $cfg;
  }
  function konfiguracja_ustaw($nowe) {
    $sql="BEGIN TRANSACTION";
    $ans=$this->configDb->query($sql);

    foreach($nowe as $sect=>$tab) {
      $sql="SELECT `id` FROM config_section WHERE name=".$this->configDb->quote($sect);
      $ans=$this->configDb->query($sql);
      $id=-1;
      foreach( $ans as $row ) {
        $id=$row['id'];
      }
      if( $id==-1 ) {
        $sql="INSERT INTO config_section (`id`, `name`) VALUES(NULL,".$this->configDb->quote($sect).")";
        $ans=$this->configDb->query($sql);
        $id=$this->configDb->lastInsertId();
      }
      foreach( $tab as $key=>$val ) {
        $sql="SELECT `id` FROM config WHERE config.key=".$this->configDb->quote($key)." AND section_id=$id";
        $ans=$this->configDb->query($sql);
        $idk=-1;
        foreach( $ans as $row ) {
          $idk=$row['id'];
        }
        if( $idk==-1 ) {
          $sql="INSERT INTO config (`id`, `section_id`, `key`, `value`)
            VALUES(NULL,$id, ".$this->configDb->quote($key).", ".$this->configDb->quote($val).")";
        } else {
          $sql="UPDATE config SET `value`=".$this->configDb->quote($val)." WHERE id=$idk";
        }
        $ans=$this->configDb->query($sql);
      }
    }
    $sql="COMMIT TRANSACTION";
    $ans=$this->configDb->query($sql);
  }
  function test_wysylania() {
    exec( "icd-run-www-transfer > /dev/null 2>&1 &" ); 
  }
  function test_fotokomorek() {
    $out = explode("\n",shell_exec( "source /etc/profile.d/icd.sh && icd-test --format=long" ), 5 );
    unset( $out[4] );
    return $out;
  }

  function raport_biezacy() {

    $data=(int)(time()/3600);
    $data*=3600;

    $sql="SELECT `cnt`, `counter_id` FROM flow WHERE dtm >= ".$data." AND dtm < ".($data+3600);
    $ans=$this->dataDb->query($sql);
    $ans->setFetchMode(PDO::FETCH_ASSOC);
    foreach( $ans as $row ) {
      if( isset( $wyniki[ $row['counter_id'] ]['cnt'] ) ) {
        $wyniki[ $row['counter_id'] ]['cnt'] += $row['cnt'];
      } else {
        $wyniki[ $row['counter_id'] ]['cnt'] = $row['cnt'];
      }
    }
    foreach( $wyniki as $pom=>&$wart ) {
      $sql="SELECT value FROM config WHERE key = 'name' AND section_id = ( SELECT section_id FROM config WHERE key = 'counter_id' AND value = $pom )";
      $ans=$this->configDb->query($sql);
      $ans->setFetchMode(PDO::FETCH_ASSOC);
      $row=$ans->fetch();
      $wart['name'] = $row[ 'value' ];
    }
    unset( $wart );
    return $wyniki;
  }
  function raport_dobowy($od, $do, $del) {
    if( $do - $od > 3600*24*31 ) {
      $od=$do - 3600*24*31;
    }
    $sql="SELECT `cnt`, `counter_id`, `dtm` FROM flow WHERE dtm >= ".$od." AND dtm < ".$do." ORDER BY dtm ASC";
    $ans=$this->dataDb->query($sql);
    $ans->setFetchMode(PDO::FETCH_ASSOC);
    foreach( $ans as $row ) {
      $t=(int)($row['dtm']/$del);
      $t*=$del;

      if( isset( $wyniki[ 'values' ][ $t ][ $row['counter_id'] ]) ) {
        $wyniki[ 'values' ][ $t ][ $row['counter_id'] ] += $row['cnt'];
      } else {
        $wyniki[ 'values' ][ $t ][ $row['counter_id'] ] = $row['cnt'];
      }
      $wyniki['counters'][ $row['counter_id' ] ] = NULL;
    }
    if( isset( $wyniki[ 'counters' ] ) ) {
      foreach( $wyniki[ 'counters' ] as $counter=>&$name ) {
        $sql="SELECT value FROM config WHERE key = 'name' AND section_id = ( SELECT section_id FROM config WHERE key = 'counter_id' AND value = $counter )";
        $ans=$this->configDb->query($sql);
        $ans->setFetchMode(PDO::FETCH_ASSOC);
        $row=$ans->fetch();
        $name = $row[ 'value' ];
      }
      unset( $name );
      natcasesort( $wyniki[ 'counters' ] );
      return $wyniki;
    }
  }

  function raport_miesieczny($od) {
    $do=mktime( 0, 0, 0, date( 'n', $od )+1, 1, date( 'Y', $od ) );
    $od=mktime( 0, 0, 0, date( 'n', $od ), 1, date( 'Y', $od ) );

    $sql="SELECT `cnt`, `counter_id`, `dtm` FROM flow WHERE dtm >= ".$od." AND dtm < ".$do." ORDER BY dtm ASC";
    $ans=$this->dataDb->query($sql);
    $ans->setFetchMode(PDO::FETCH_ASSOC);
    foreach( $ans as $row ) {
      $t=mktime( 0, 0, 0, date( 'n', $row['dtm'] ), date( 'j', $row['dtm'] ), date( 'Y', $row['dtm'] ) );

      if( isset( $wyniki[ 'values' ][ $t ][ $row['counter_id'] ]) ) {
        $wyniki[ 'values' ][ $t ][ $row['counter_id'] ] += $row['cnt'];
      } else {
        $wyniki[ 'values' ][ $t ][ $row['counter_id'] ] = $row['cnt'];
      }
      $wyniki['counters'][ $row['counter_id' ] ] = NULL;
    }
    if( isset( $wyniki[ 'counters' ] ) ) {
      foreach( $wyniki[ 'counters' ] as $counter=>&$name ) {
        $sql="SELECT value FROM config WHERE key = 'name' AND section_id = ( SELECT section_id FROM config WHERE key = 'counter_id' AND value = $counter )";
        $ans=$this->configDb->query($sql);
        $ans->setFetchMode(PDO::FETCH_ASSOC);
        $row=$ans->fetch();
        $name = $row[ 'value' ];
      }
      unset( $name );

      natcasesort( $wyniki[ 'counters' ] );

      return $wyniki;
    }
  }
  function raport_instalacyjny($od) {
    $do=mktime( 0, 0, 0, date( 'n', $od ), date( 'j', $od)+1 , date( 'Y', $od ) );
    $od=mktime( 0, 0, 0, date( 'n', $od ), date( 'j', $od) , date( 'Y', $od ) );

    $sql="SELECT dtm, counter_id, id, cnt, dark_time, work_time, "
      ."test, flags FROM flow WHERE dtm >= ".$od." AND dtm < ".$do." ORDER BY dtm, counter_id ASC";

    $ans=$this->dataDb->query($sql);
    $ans->setFetchMode(PDO::FETCH_ASSOC);
    foreach( $ans as $row ) {
      $datawyniki[ 'values' ][]=$row;
      $datawyniki['counters'][ $row['counter_id' ] ] = NULL;
    }

    if( isset( $datawyniki[ 'counters' ] ) ) {
      foreach( $datawyniki[ 'counters' ] as $counter=>&$name ) {
        $sql2="SELECT value FROM config WHERE key = 'name' AND section_id = ( SELECT section_id FROM config WHERE key = 'counter_id' AND value = $counter )";
        $ans=$this->configDb->query($sql2);
        $ans->setFetchMode(PDO::FETCH_ASSOC);
        $row=$ans->fetch();
        $name = $row[ 'value' ];
      }
      unset( $name );

      $wyniki['data']=$datawyniki;
    }

    $ans=$this->liveDb->query($sql);
    $ans->setFetchMode(PDO::FETCH_ASSOC);
    foreach( $ans as $row ) {
      $livewyniki[ 'values' ][]=$row;
      $livewyniki['counters'][ $row['counter_id' ] ] = NULL;
    }

    if( isset( $livewyniki[ 'counters' ] ) ) {
      foreach( $livewyniki[ 'counters' ] as $counter=>&$name ) {
        $sql2="SELECT value FROM config WHERE key = 'name' AND section_id = ( SELECT section_id FROM config WHERE key = 'counter_id' AND value = $counter )";
        $ans=$this->configDb->query($sql2);
        $ans->setFetchMode(PDO::FETCH_ASSOC);
        $row=$ans->fetch();
        $name = $row[ 'value' ];
      }
      unset( $name );

      $wyniki['live']=$livewyniki;
    }
    return $wyniki;
  }
  function reset() {
    exec( "icd-shutdown --reboot > /dev/null 2>&1 &" );
  }
  function set_send_flag() {
    $sql='UPDATE flow SET flags = -2 WHERE flags > 0';
    $this->dataDb->query($sql);
  }
  function recreate_databases() {
    $fp = fopen($this->ICD_DATA_DIR.'/recreate_databases', 'w');
    fwrite($fp, date(DATE_RFC822) );
    fclose($fp);
  }
  function csv_export($od, $do, $header) {
    if( $header ) {    
      $wysylanie=$this->wysylanie_pobierz();
      foreach( $wysylanie as $key=>$val ) {
        echo( $key."\t".$val."\r\n" );
      }
      $licznik=$this->liczniki_pobierz();
      echo( "\r\n" );
      echo( "Id licznika\tNazwa licznika\r\n" );
      for( $i=0; $i<4; ++$i ) {
        echo( $licznik['counter_id'][$i]."\t".$licznik['name'][$i]."\r\n" );
      }
      echo("id\tcounter_id\tdatetime\tcnt\tdark_time\twork_time\ttest\tflags\r\n" );
    }

    $sql="SELECT " 
      ."id, counter_id, datetime(dtm, 'unixepoch', 'localtime'), cnt, dark_time, work_time, test, flags "
      ." FROM flow WHERE flags > 0 AND dtm >= ".$od." AND dtm < ".$do." ORDER BY dtm, counter_id ASC";

    $ans=$this->dataDb->query($sql);
    $ans->setFetchMode(PDO::FETCH_NUM);
    foreach( $ans as $row ) {
      for( $i=0; $i<7; ++$i ) {
        echo( $row[$i]."\t" );
      }
      echo( $row[$i]."\r\n" );
    }
    if( $header ) { 
      echo( "\n\r__EOF__\n\r" );
    }
  }
  function csv_export_done($od, $do) {

    $sql='UPDATE flow SET flags = -2 WHERE flags > 0 AND dtm >= '.$od.' AND dtm < '.$do;
    $this->dataDb->query($sql);

    return 'Oznaczono dane z przedziału od '.date( 'Y-m-d H:i:s', $od ).' do '.date( 'Y-m-d H:i:s', ($do-1) );
  }
}

?>
