<?php
defined('INSOFTER') or die('<h1>Your Kung-Fu is too weak.</h1>');

$icdtcp = new c_icdtcp();

if( $_SESSION['zalogowany']!=0 ) {
  if( isset( $_POST['npass'] ) ) {
    if( $_POST['npass']==$_POST['ppass'] ) {

      if( strlen( $_POST['npass'] ) < 3 ) {
        $info='<h4 id="err">Hasło jest zbyt krótkie</h4>';
      } else {
        if( isset( $_POST['enab'] ) ) {
          $enab='yes';
        } else {
          $enab='no';
        }
        if( $icdtcp->ustaw_logowanie( $_POST['npass'], $_POST['cpass'], $enab ) ) {
          $info='<h4>Ustawiono nowa konfigurację</h4>';
          if( $enab=='yes' ) {
            $_SESSION['zalogowany']=2;
          } else {
            $_SESSION['zalogowany']=1;
          }
        } else {
          $info='<h4 id="err">Hasło jest niepoprawne</h4>';
        }
      }
    } else {
      $info='<h4 id="err">Hasła są różne</h4>';
    }
  } else {
    $info='';
  }
  $tresc='<div id="tresc"><form action="./?strona=logowanie" method="POST">
    <h3>Konfiguracja dostępu</h3>'
    .$info.'
    <table>
    <tr><th>
    <label for="npass">Nowe hasło</label> :</th><td>
    <input type="password" name="npass" id="npass" size="20">
    </td></tr>

    <tr><th>
    <label for="ppass">Powtórz hasło</label> :</th><td>
    <input type="password" name="ppass" id="ppass" size="20">
    </td></tr>
    <tr><th>
    <label for="enab">Włącz logowanie</label> :</th><td>
    <input type="checkbox" name="enab" id="enab"';
  if( $icdtcp->pob_logowanie() ) {
    $tresc.=' checked="checked"';
  }
  $tresc.='>
    </td></tr>
    <tr><th>
    <label for="cpass">Stare hasło</label> :</th><td>
    <input type="password" name="cpass" id="cpass" size="20">
    </td></tr>
    <tr><td colspan="2">
    <input type="submit" value="Ustaw">
    </td></tr>
    </table>
    ';

} else {
  $tresc='<div id="tresc"><form action="./?strona='.$_GET['strona'].'" method="POST">
    <h3>Logowanie</h3>
    <table>
    <tr><th>
    <label for="password">Hasło</label> :</th><td>
    <input type="password" name="password" id="password" size="20">
    </td><td>
    <input type="submit" value="Zaloguj">
    </td></tr>
    </table>
    </form></div>';
}
?>
