<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

require_once('inc/c_wpa.php');
$wpa = new c_wpa();

if( isset( $_POST['nazwa_pliku'] ) )
{
	$wpa->cfg_to_db();
	$cfg=$icdtcp->konfiguracja_pobierz();

	$eksport['data']=date(DATE_RFC822);
	$eksport['komentarz']=strtr($_POST['komentarz'], array(
        'ę'=>'e',
        'Ę'=>'E',
        'ó'=>'o',
        'Ó'=>'O',
        'ą'=>'a',
        'Ą'=>'A',
        'ś'=>'s',
        'Ś'=>'S',
        'ł'=>'l',
        'Ł'=>'L',
        'ż'=>'z',
        'Ż'=>'Z',
        'ź'=>'z',
        'Ź'=>'Z',
        'ć'=>'c',
        'Ć'=>'C',
        'ń'=>'n',
        'Ń'=>'N',
        '"'=>"'",
    ));

        ;
	$eksport['nazwa_pliku']=$_POST['nazwa_pliku'];

	$cfg['__dane_eksportu']['eksport']=$eksport;
	$cfg['__dane_eksportu']['skrypt']=$o_mnie;

	if( defined('JSON_PRETTY_PRINT') )
	{
		$json=json_encode($cfg, JSON_PRETTY_PRINT);
	} else {
		$json=strtr(json_encode($cfg),array(
			'"},'=>"\"\n},\n",
			'"}'=>"\"\n}\n",
			'":{'=>"\":\n{\n",
			'{"'=>"{\n\"",
			'":"'=>'": "',
			'",'=>"\",\n"
			) );
	}
	$json=strtr( $json, array("\r\n"=>"\n") );

	switch( $_POST['znak_lini'] ) {
	        case 'crlf':
	                $json=strtr( $json, array("\n"=>"\r\n") );
	                break;
	        case 'cr':
			$json=strtr( $json, array("\n"=>"\r") );
			break;
	}

	header('Content-Type: application/octet-stream');
	header("Pragma: public");
    	header("Expires: 0");
	header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
	header("Content-Type: text/plain");

 	header('Content-Disposition: attachment; filename="'.$_POST['nazwa_pliku'].'"');

	ob_clean();
	flush();
	echo($json);
	exit;


} else if( isset( $_POST['nowa_konf'] ) ) {
        if( $_FILES['plik_konf']['tmp_name']=='' ) {
                $tresc='<h3>Import konfiguracji</h3><h4 id="err">Brak pliku</h4>';
        } else {
		$json=file_get_contents( $_FILES['plik_konf']['tmp_name'] );
		$cfg=json_decode($json, true);
		if( json_last_error()!=JSON_ERROR_NONE ) {
			$tresc='<h3>Import konfiguracji</h3><h4 id="err">Błędny plik</h4>';
		} else {
		        $dane_eksportu=$cfg['__dane_eksportu'];
        		//print_r($dane_eksportu);
		        unset($cfg['__dane_eksportu']);
			//print_r($cfg);

	        	$tresc='<div id="tresc"><form action="./?strona=plik_konf" method="POST">
<h3>Sprawdź konfigurację</h3>';
if( !isset($dane_eksportu['skrypt']['wersja_rok']) || !isset($dane_eksportu['skrypt']['wersja_rok'])
	|| !isset($dane_eksportu['klasa']['wersja_miesiac']) || !isset($dane_eksportu['klasa']['wersja_miesiac'])
	|| $dane_eksportu['skrypt']['wersja_rok']!=$o_mnie['wersja_rok']
	|| $dane_eksportu['skrypt']['wersja_miesiac']!=$o_mnie['wersja_miesiac']
	|| $dane_eksportu['klasa']['wersja_rok']!=$icdtcp->o_mnie['wersja_rok']
	|| $dane_eksportu['klasa']['wersja_miesiac']!=$icdtcp->o_mnie['wersja_miesiac']
	|| !isset($dane_eksportu['skrypt']['nazwa']) || !isset($dane_eksportu['klasa']['nazwa'])
        || $dane_eksportu['skrypt']['nazwa']!=$o_mnie['nazwa']
	|| $dane_eksportu['klasa']['nazwa']!=$icdtcp->o_mnie['nazwa'] ) {
	$tresc.='<h4 id="err">Wersja pliku nie jest zgodna z wersją skryptu</h4>';
}
$tresc.='<table>
<tr><th>Data utworzenia:</th><td>'.$dane_eksportu['eksport']['data'].'</td></tr>
<tr><th>Komentarz:</th><td>'.$dane_eksportu['eksport']['komentarz'].'</td></tr>
<tr><th>Pierwotna nazwa pliku:</th><td>'.$dane_eksportu['eksport']['nazwa_pliku'].'</td></tr>
';

	 		foreach( $cfg as $sect=>$tab ) {
		                $tresc.='<tr><th colspan="2"><br>
<input type="checkbox" name="'.$sect.'[__importuj]" id="'.$sect.'" checked="checked">
<label for="'.$sect.'">'.$sect.'</label>

</th></tr>';
			        foreach( $tab as $key=>$val ) {
		   			$tresc.='<tr><th>
<label for="'.$sect.'__'.$key.'">'.$key.'</label>
</th><td>
<input type="text" name="'.$sect.'['.$key.']" id="'.$sect.'__'.$key.'" size="40" value="'.$val.'">
</td></tr>';
			        }
			}
			$tresc.='<tr><td colspan="2">
<input type="submit" value="Importuj konfigurację" name="__spr_konf">
</td></table></form></div>';
		}
 	}
} else if( isset( $_POST['__spr_konf'] ) ) {
	unset( $_POST['__spr_konf'] );
	foreach($_POST as $sect=>$tab) {
	        if( isset($tab['__importuj']) )
	        {
	                $cfg[$sect]=$tab;
	                unset( $cfg[$sect]['__importuj'] );
	 	}
 	}

	$wpa->cfg_to_db();

 	$icdtcp->konfiguracja_ustaw($cfg);

	$wpa->cfg_from_db();

	$tresc='<h3>Import konfiguracji</h3><h4>Zaimportowano nową konfigurację</h4>';
		//print_r($cfg);

} else {
	$wysylanie=$icdtcp->wysylanie_pobierz();
        $tresc='<div id="tresc"><form action="./?strona=plik_konf" method="POST">
<h3>Eksportuj konfigurację</h3>
<table>
<tr><th>
<label for="nazwa_pliku">Nazwa pliku</label> :
</th><td>
<input type="text" name="nazwa_pliku" id="nazwa_pliku" value="'.date('Y.m.d_H.i').'__'.$o_mnie['nazwa'].'__'.$wysylanie['ids'].'.json"  size="50">
</td></tr>
<tr><th>
<label for="komentarz">Komentarz</label> :
</th><td>
<input type="text" name="komentarz" id="komentarz" size="50">
</td></tr>
<tr><th>
<label for="znak_lini">Znak nowej lini</label> :
</th><td>
<select name="znak_lini" id="znak_lini">
<option selected="selected" value="crlf">Windows</option>
<option value="lf">Linux / Unix</option>
<option value="cr">Mac OS</option>
</select>
</td></tr>
<tr>
<td colspan="2">
<input type="submit" value="Eksportuj konfgurację">
</td></tr>
</table>
</form>
<form action="./?strona=plik_konf" method="POST" enctype="multipart/form-data">
<h3>Importuj konfigurację</h3>
<table>
<tr><th>
<label for="plik_konf">Plik konf.</label> :
</th><td>
<input type="file" name="plik_konf" id="plik_konf" size="50">
</td></tr>
<tr>
<td colspan="2">
<input type="submit" value="Importuj konfigurację" name="nowa_konf">
</td>
</tr>
</table>
</form>
</div>';


}

?>
