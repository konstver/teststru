<?php

  $id=$_GET['id'];
  $content=$_GET['content'];
  $name=$_GET['name'];
  $profit=$_GET['profit'];

  //ini_set('display_errors',1);
  //error_reporting(E_ALL);

  include("functions.inc");
  
  show_header();

  if(!$content){
	  getstru();
  }
  elseif($content=="edit"){
	edit($id);  
  }
  elseif($content=="write_node"){
	write_node($id,$name,$profit);
  }
  elseif($content=="add"){
	  add($id);
  }
  elseif($content=="add_node"){
	  add_node($id,$name,$profit);
  }
  elseif($content=="del"){
	  del($id);
  };

  show_footer();

?>
