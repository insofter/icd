<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

class c_icdtcp {

  public $czas;
  public $czas_dlugi;
  public $ilosc;
  public $o_mnie;

  var $baza;
  public $il_fotokomorek;

  function __construct() {
    $this->o_mnie=array('nazwa'=>'c_icdtcp',
      'plik'=>'inc/c_icdtcp.php',
      'wersja_rok'=>'2012',
      'wersja_miesiac'=>'07');
    $this->il_foto=4;
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


    foreach( file("/etc/profile.d/icd.sh", FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES) as $tmp ) {
      $tmp=explode( '=', strtr( $tmp, array('export '=>'', '$'=>'', '('=>'', ';'=>'', ) ) );
      eval( '$'.$tmp[ 0 ].'="'.$tmp[ 1 ].'";' );
    }


    //print_r( $ICD_DATA_DB ) ;




    try {
      /*	$this->dataDb = new PDO( 'mysql:host=localhost;dbname=icdtcp3db','root','');*/
      $this->dataDb = new PDO('sqlite:'.$ICD_DATA_DB);
      $this->configDb = new PDO('sqlite:'.$ICD_CONFIG_DB);
      $this->liveDb = new PDO('sqlite:'.$ICD_LIVE_DB);
/*      export ICD_DATA_DB=/home/insofter/projects/data/data.db
        export ICD_CONFIG_DB=/home/insofter/projects/data/config.db
        export ICD_LIVE_DB=/home/insofter/projects/data/live.db*/

    } catch (PDOException $e) {
      print $e->getMessage();
    }
  }

  function info() {
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c on cs.id=c.section_id
      WHERE cs.name='device' AND (c.key='idd' OR c.key='ids' OR c.key='address')";
    $ans=$this->configDb->query($sql);
    foreach( $ans as $row ) {
      $info[$row['key']]=$row['value'];
    }

    for( $i=0; $i<$this->il_foto; ++$i ) {//TODO:
      $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c on cs.id=c.section_id
        WHERE cs.name='itd$i' AND (c.key='name' OR c.key='enabled' ) ";
      $ans=$this->configDb->query($sql);

      foreach( $ans as $row ) {
        $licznik[$row['key']]=$row['value'];
      }
      $licznik['nr']=$i;

      $sql="SELECT `cnt` FROM flow WHERE itd='$i' ORDER BY `dtm` DESC LIMIT 1";
      $ans=$this->liveDb->query($sql);
      $licznik['cnt_last']=0;
      if( $ans!=NULL ) {
        while( $row=$ans->fetch() ) {
          $licznik['cnt_last']=$row['cnt'];
        }
      }
      $sql="SELECT sum(cnt) AS sum FROM flow WHERE itd='itd$i'
        AND `dtm` < ".(time()*1000)." AND `dtm` > ".( ((int)(time()/(24*3600)))*24*3600*1000 );
      $ans=$this->liveDb->query($sql);//TODO:
      $licznik['cnt_sum']=0;
      if( $ans!=NULL ) {
        while( $row=$ans->fetch() ) {
          $licznik['cnt_sum']=(int)$row['sum'];
        }
      }
      $sql="SELECT `state` FROM events WHERE itd='itd$i' ORDER BY `dtmms` DESC LIMIT 1";
      $ans=$this->dataDb->query($sql);
      $licznik['state']=1;
      if( $ans!=NULL ) {
        while( $row=$ans->fetch() ) {
          $licznik['state']=$row['state'];
        }
      }//TODO:

      $liczniki[]=$licznik;
    }

    $info['liczniki']=$liczniki;
    return $info;
  }

  function tcpip_pobierz($typ) {
    if( $typ!='current' ) {
      $typ='tcpip';
    }
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id
      WHERE cs.name='$typ' AND (c.key='dns1' OR c.key='dns2' OR c.key='gate' OR
      c.key='ip' OR c.key='mask' OR c.key='net-name' OR c.key='mac')";
    $ans=$this->configDb->query($sql);
    foreach( $ans as $row ) {
      $tcp[$row['key']]=$row['value'];
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

  function tcpip_ustaw($nowe) {
    foreach( $nowe as $pole=>$wart ) {
      $sql="UPDATE config SET `value`=".$this->dataDb->quote($wart)."
        WHERE section_id=(SELECT `id` FROM config_section WHERE name='tcpip')
        AND `key`=".$this->configDb->quote($pole);
      $this->configDb->query($sql);
    }
  }

  function liczniki_pobierz() {//TODO:
    for( $i=0; $i<$this->il_foto; ++$i ) {
      $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id
        WHERE cs.name='itd$i' ";
      $ans=$this->configDb->query($sql);
      foreach( $ans as $row ) {
        $licznik[$row['key']]=$row['value'];
      }
      $licznik['nr']=$i;
      $liczniki[]=$licznik;
    }
    return $liczniki;
  }

  function liczniki_ustaw($nowe) {//TODO:
    for( $i=0; $i<$this->il_foto; ++$i ) {
      foreach( $nowe[$i] as $pole=>$wart ) {
        $sql="UPDATE config SET `value`=".$this->configDb->quote($wart)."
          WHERE section_id=(SELECT `id` FROM config_section WHERE name='itd$i')
          AND `key`=".$this->configDb->quote($pole);
        $this->configDb->query($sql);
      }
    }
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
  function wysylanie_ustaw($nowe)
  {
    foreach( $nowe as $pole=>$wart ) {
      $sql="UPDATE config SET `value`=".$this->configDb->quote($wart)."
        WHERE section_id=(SELECT `id` FROM config_section WHERE name='device')
        AND `key`=".$this->configDb->quote($pole);
      $this->configDb->query($sql);
    }
  }
  function konfiguracja_pobierz()
  {
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
  function konfiguracja_ustaw($nowe)
  {
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
  }
  function wyniki($czas=0, $itd=-1)
  {
    if( $czas==0 ) {
      $czas=mktime( 0, 0, 1, date('n'), date('j'), date('Y') );
    }
    $konc=$czas+24*3600;
    if( $itd!=-1 ) {
      $sql="SELECT `itd`, `dtm`, `cnt`, `dark_time`, `work_time`, `flags`
        FROM flow WHERE itd='".((int)$itd)."' AND dtm > $czas AND dtm < $konc ORDER BY itd ASC, dtm ASC";
    } else {
      $sql="SELECT `itd`, `dtm`, `cnt`, `dark_time`, `work_time`, `flags`
        FROM flow WHERE dtm > $czas AND dtm < $konc ORDER BY dtm ASC";
    }
    $ans=$this->dataDb->query($sql);
    $ans->setFetchMode(PDO::FETCH_ASSOC);
    foreach( $ans as $row ) {
      $wyniki[] = $row;
    }
    if( !isset( $wyniki ) ) {
      $wyniki=array();
    }
    return $wyniki;
  }
  function test_wysylania()
  {
    return shell_exec( "icd-transfer-data --log=long" ); 
  }

  function raport_biezacy($data) {
    $data=mktime( date("H"), 0, 0, date("n"),  date("j"), date("Y") );
    $sql="SELECT `id`, `name`, `enabled` FROM counters";
    $ans=$this->dataDb->query($sql);
    $ans->setFetchMode(PDO::FETCH_ASSOC);
    foreach( $ans as $row ) {
      $sql2="SELECT `cnt` FROM flow WHERE counter_id = ".$row['id']." AND dtm >= ".$data." AND dtm < ".($data+3600);

      $ans2=$this->dataDb->query($sql2);
      $ans2->setFetchMode(PDO::FETCH_ASSOC);

      $row['cnt']=0;
      foreach( $ans2 as $row2 ) {
        $row['cnt']+=$row2['cnt'];
      }

      $wyniki[] = $row;
    }
    return $wyniki;
  }
  function raport_dobowy($data) {
    $data=mktime( 0, 0, 0, date("n"),  date("j"), date("Y") );
    $sql="SELECT `id`, `name` FROM counters";
    $ans=$this->dataDb->query($sql);
    $ans->setFetchMode(PDO::FETCH_ASSOC);
    foreach( $ans as $row ) {
      $sql2="SELECT `cnt` FROM flow WHERE counter_id = ".$row['id']." AND dtm >= ".$data." AND dtm < ".($data+3600);

      $ans2=$this->dataDb->query($sql2);
      $ans2->setFetchMode(PDO::FETCH_ASSOC);

      $row['cnt']=0;
      foreach( $ans2 as $row2 ) {
        $row['cnt']+=$row2['cnt'];
      }

      $wyniki[] = $row;
    }
    return $wyniki;
  }
  function raport_tygodniowy($data) {
  }
  function raport_miesieczny($data) {
  }
  function csv_export($data, $typ) {
  }
}

?>
