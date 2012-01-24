<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
  <title>Welcome page</title>
  <style type="text/css" media="screen">
    body { background: #e7e7e7; font-family: Verdana, sans-serif; font-size: 11pt; }
    #page { background: #ffffff; margin: 50px; border: 2px solid #c0c0c0; padding: 10px; }
    #header { background: #4b6983; border: 2px solid #7590ae; text-align: center; padding: 10px; color: #ffffff; }
    #header h1 { color: #ffffff; }
    #body { padding: 10px; }
    span.tt { font-family: monospace; }
    span.bold { font-weight: bold; }
    a:link { text-decoration: none; font-weight: bold; color: #C00; background: #ffc; }
    a:visited { text-decoration: none; font-weight: bold; color: #999; background: #ffc; }
    a:active { text-decoration: none; font-weight: bold; color: #F00; background: #FC0; }
    a:hover { text-decoration: none; color: #C00; background: #FC0; }
    table { border-collapse:collapse; }
    table,th,td { border: 2px solid #7590ae; }
    td, th { padding-top:5px; padding-bottom:5px; padding-right:15px; padding-left:15px; }
    th { background: #4b6983; color: #ffffff; }
  </style>
</head>
<body>
  <div id="page">
    <div id="header">
      <h1> Icdtcp3 demo php page </h1>
      What you see is Icdtcp3 device's web page. It prosents the configuration tables stored in sqlite database.
    </div>
    <div id="body">
      <?php
        try 
        {
          $db = new PDO("sqlite:/tmp/live.db");
          $sections = $db->query('SELECT id, name FROM config_section');
          foreach($sections as $section)
          {
            echo '<h2>'.$section['name'].'</h2>';
            $entries = $db->query('SELECT key, value FROM config WHERE section_id == '.$section['id']);
            echo '<table >';
            echo '<tr> <th>key</th> <th>value</th> </tr>';
            foreach($entries as $entry)
              echo'<tr> <td>'.$entry['key'].'</td> <td>'.$entry['value'].'</td> </tr>';
            echo '</table>';
          }
          $db = null;
        }
        catch(PDOException $e)
        {
          echo $e->getMessage();
        }
      ?>
      <p style="text-align: center;"">Copyright 2012 by Tomasz Rozenszauch. All Rights Reserved.</p>
    </div>
  </div>
</body>
</html>
