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
    $this->czas_dlugi=array( 1440=>'1 dzień',
      2880=>'2 dni',
      100080=>'7 dni',
      20160=>'14 dni',
      43200=>'30 dni',
      129600=>'3 miesiące',
      259200=>'6 miesięcy',
      518400=>'12 miesięcy');
    $this->ilosc=array( 1000=>'1 000',
      10000=>'10 000',
      100000=>'100 000',
      250000=>'250 000',
      500000=>'500 000',
      1000000=>'1 000 000');
    try {
      /*	$this->dataDb = new PDO( 'mysql:host=localhost;dbname=icdtcp3db','root','');*/
      $this->dataDb = new PDO("sqlite:/home/data/data.db");
      $this->tmpDb = new PDO("sqlite:/tmp/tmp.db");
    } catch (PDOException $e) {
      print $e->getMessage();
    }
  }

  function info() {
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c on cs.id=c.section_id
      WHERE cs.name='device' AND (c.key='idd' OR c.key='ids' OR c.key='address')";
    $ans=$this->dataDb->query($sql);
    foreach( $ans as $row ) {
      $info[$row['key']]=$row['value'];
    }

    for( $i=0; $i<$this->il_foto; ++$i ) {//TODO:
      $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c on cs.id=c.section_id
        WHERE cs.name='itd$i' AND (c.key='name' OR c.key='enabled' ) ";
      $ans=$this->dataDb->query($sql);

      foreach( $ans as $row ) {
        $licznik[$row['key']]=$row['value'];
      }
      $licznik['nr']=$i;

      $sql="SELECT `cnt` FROM flow WHERE itd='itd$i' ORDER BY `dtm` DESC LIMIT 1";
      $ans=$this->dataDb->query($sql);
      $licznik['cnt_last']=0;
      foreach( $ans as $row ) {
        $licznik['cnt_last']=$row['cnt'];
      }
      $sql="SELECT sum(cnt) AS sum FROM flow WHERE itd='itd$i'
        AND `dtm` < ".(time()*1000)." AND `dtm` > ".( ((int)(time()/(24*3600)))*24*3600*1000 );
      $ans=$this->dataDb->query($sql);
      $licznik['cnt_sum']=0;
      foreach( $ans as $row ) {
        $licznik['cnt_sum']=(int)$row['sum'];
      }
      $sql="SELECT `state` FROM events WHERE itd='itd$i' ORDER BY `dtmms` DESC LIMIT 1";
      $ans=$this->dataDb->query($sql);
      $licznik['state']=1;
      foreach( $ans as $row ) {
        $licznik['state']=$row['state'];
      }

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
    $ans=$this->dataDb->query($sql);
    foreach( $ans as $row ) {
      $tcp[$row['key']]=$row['value'];
    }
    return $tcp;

  }
  function dhcp_pobierz() {
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id
      WHERE cs.name='tcpip' AND c.key='dhcp' ";
    $ans=$this->dataDb->query($sql);
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
    $ans=$this->dataDb->query($sql);
  }

  function tcpip_ustaw($nowe) {
    foreach( $nowe as $pole=>$wart ) {
      $sql="UPDATE config SET `value`=".$this->dataDb->quote($wart)."
        WHERE section_id=(SELECT `id` FROM config_section WHERE name='tcpip')
        AND `key`=".$this->dataDb->quote($pole);
      $this->dataDb->query($sql);
    }
  }

  function liczniki_pobierz() {//TODO:
    for( $i=0; $i<$this->il_foto; ++$i ) {
      $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id
        WHERE cs.name='itd$i' ";
      $ans=$this->dataDb->query($sql);
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
        $sql="UPDATE config SET `value`=".$this->dataDb->quote($wart)."
          WHERE section_id=(SELECT `id` FROM config_section WHERE name='itd$i')
          AND `key`=".$this->dataDb->quote($pole);
        $this->dataDb->query($sql);
      }
    }
  }

  function wysylanie_pobierz()
  {
    $sql="SELECT `key`, `value` FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id
      WHERE cs.name='device' AND ( c.key='ids' OR c.key='idd' OR c.key='address'
      OR c.key='user' OR c.key='transfer-enabled' OR c.key='aggr-period-mins'
      OR c.key='event-retention-period-count' OR c.key='flow-entry-retention-period-mins'
      OR c.key='pass' OR c.key='server-sync-period-mins' OR c.key='test-retention-period-mins' )";
    $ans=$this->dataDb->query($sql);
    foreach( $ans as $row ) {
      $tcp[$row['key']]=$row['value'];
    }
    return $tcp;
  }
  function wysylanie_ustaw($nowe)
  {
    foreach( $nowe as $pole=>$wart ) {
      $sql="UPDATE config SET `value`=".$this->dataDb->quote($wart)."
        WHERE section_id=(SELECT `id` FROM config_section WHERE name='device')
        AND `key`=".$this->dataDb->quote($pole);
      $this->dataDb->query($sql);
    }
  }
  function konfiguracja_pobierz()
  {
    $cfg['__dane_eksportu']['eksport']='';
    $cfg['__dane_eksportu']['skrypt']='';
    $cfg['__dane_eksportu']['klasa']=$this->o_mnie;

    $sql="SELECT `key`, `value`, `name`  FROM config_section cs LEFT JOIN config c ON cs.id=c.section_id";
    $ans=$this->dataDb->query($sql);
    foreach( $ans as $row ) {
      $cfg[ $row['name'] ][ $row['key'] ] = $row['value'];
    }
    return $cfg;
  }
  function konfiguracja_ustaw($nowe)
  {
    foreach($nowe as $sect=>$tab) {
      $sql="SELECT `id` FROM config_section WHERE name=".$this->dataDb->quote($sect);
      $ans=$this->dataDb->query($sql);
      $id=-1;
      foreach( $ans as $row ) {
        $id=$row['id'];
      }
      if( $id==-1 ) {
        $sql="INSERT INTO config_section (`id`, `name`) VALUES(NULL,".$this->dataDb->quote($sect).")";
        $ans=$this->dataDb->query($sql);
        $id=$this->dataDb->lastInsertId();
      }
      foreach( $tab as $key=>$val ) {
        $sql="SELECT `id` FROM config WHERE config.key=".$this->dataDb->quote($key)." AND section_id=$id";
        $ans=$this->dataDb->query($sql);
        $idk=-1;
        foreach( $ans as $row ) {
          $idk=$row['id'];
        }
        if( $idk==-1 ) {
          $sql="INSERT INTO config (`id`, `section_id`, `key`, `value`)
            VALUES(NULL,$id, ".$this->dataDb->quote($key).", ".$this->dataDb->quote($val).")";
        } else {
          $sql="UPDATE config SET `value`=".$this->dataDb->quote($val)." WHERE id=$idk";
        }
        $ans=$this->dataDb->query($sql);
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
    return shell_exec( "icd-transfer-data --db=/tmp/live.db --log=long" ); 
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
