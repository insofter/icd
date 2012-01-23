<?php


try {
    /*** connect to SQLite database ***/
    $db = new PDO("sqlite:/tmp/live.db");

    /*** echo a message saying we have connected ***/
    echo 'Connected to database<br />';

    /*** The SQL SELECT statement ***/
    $sections = $db->query('SELECT * FROM config_section');
    foreach($sections as $section)
    {
      print $section['id'] .' - '. $section['name'] . '<br />';
    }

    /*** close the database connection ***/
    $db = null;

    }
catch(PDOException $e)
    {
    echo $e->getMessage();
    }
?>

<!-- ?php phpinfo(); ? -->


