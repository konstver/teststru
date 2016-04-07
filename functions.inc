<?php
// Name of configuration file
$configfile="settings.conf";

// Get information from config file for database connection
function getconfiguration($configfile){
   $retval=array();

   $fh=fopen($configfile,"r");
   while(!feof($fh)){
	$line=fgets($fh,4096);
	
	$config_array=explode("=",$line);

	$param=$config_array[0];
	$value=trim($config_array[1]);

	switch($param){
	   case("host"):
		$retval['host']=$value;
		break;
	   case("user"):
		$retval['username']=$value;
		break;
	   case("passwd"):
		$retval['passwd']=$value;
		break;
	   case("base"):
		$retval['base']=$value;
		break;
	   default:
		break;
	}
   }
    
   fclose($fh);
   return $retval;
}

// Function connects to database with parameters from configfile
function connectdb($dbc_host,$dbc_username,$dbc_passwd,$dbc_base){
	$db_link=mysql_connect($dbc_host,$dbc_username,$dbc_passwd) or die('Не удалось соединиться: ' . mysql_error());

	if(!$db_link){
		print "Database connection error.";
	}

	$res=mysql_select_db($dbc_base,$db_link);
	if(!$res){
		print "Error selecting database ".$dbc_base;
	}
	return $db_link;
}

// Close database connection after queryes executed
function closedb($db_link){
	mysql_close($db_link);
}

// Create structure in empty table
function createstru(){
	$sets=getconfiguration($configfile);
	$bl=connectdb(trim($sets['host']),trim($sets['username']),trim($sets['passwd']),trim($sets['base']));
	$query="CREATE TABLE
			tree (
				id INTEGER AUTO_INCREMENT PRIMARY KEY,
				left_key INTEGER NOT NULL,
				right_key INTEGER NOT NULL,
				name TEXT,
				profit DOUBLE,
		INDEX
			left_key (left_key, right_key, level)
		);";
	$result=mysql_query($query,$bl);
	closedb();
}
	
// Function returns total number of records in tree table
function get_total(){
	global $configfile;
	$sets=getconfiguration($configfile);
	$bl=connectdb($sets['host'],$sets['username'],$sets['passwd'],$sets['base']);

	$query="select count(id) from my_tree";
	$result=mysql_query($query,$bl);
	$line = mysql_fetch_array($result, MYSQL_NUM);
        $num=$line[0];

	mysql_free_result($result);
	closedb($bl);

	return $num;
}

// Main function to show entire tree structure
function getstru(){
	global $configfile;
                
	$sets=getconfiguration($configfile);

	$bl=connectdb($sets['host'],$sets['username'],$sets['passwd'],$sets['base']);

	$query="select
			node.id,
			concat(repeat('&nbsp;&nbsp;',(count(parent.id)-1)),node.name) as name,
			node.profit,
			(select
				sum(nd.profit)
			from
				my_tree as nd,
				my_tree as prnt
			where
				nd.left_key between prnt.left_key 
				and 
				prnt.right_key 
				and 
				prnt.id=node.id
			) as nodeprofite
		from
			my_tree as node,
			my_tree as parent
		where
			node.left_key between parent.left_key and parent.right_key
		group by node.id
		order by node.left_key;";

	$result=mysql_query($query,$bl);
	
	echo "<table border='1'>\n";
		
	$total_recs = get_total();
		
	$add_first="";
	if (!$total_recs){
		$add_first="<a href='index.php?content=add'>+</a>";
	}
		
	echo "\t<tr><th align='center'> Company $add_first</th><th align='center'>&nbsp;Profit&nbsp;</th><th align='center'>&nbsp;Total&nbsp;</th></tr>";
	while ($line = mysql_fetch_array($result, MYSQL_NUM)) {
		echo "\t<tr>\n";
		$iteration=0;
		$id=0;
		foreach ($line as $col_value) {
			if($iteration==0){
				$id=$col_value;
			}
			elseif($iteration==1){
				echo "\t\t<td><a href='index.php?content=edit&id=$id'>$col_value</a>&nbsp;<a href='index.php?content=add&id=$id'>+</a>&nbsp;<a href='index.php?content=del&id=$id'>X</a></td>\n";
			}
			else {
				echo "\t\t<td align='right'>$col_value</td>\n";
			};
			$iteration++;
		}
	//echo "\t\t<td>&nbsp;</td>\n";
	//echo "\t</tr>\n";
	}
	echo "</table>\n";

	mysql_free_result($result);

	closedb($bl);
}

// Edit selected record
function edit($id){
	global $configfile;
                
	$sets=getconfiguration($configfile);

	$bl=connectdb($sets['host'],$sets['username'],$sets['passwd'],$sets['base']);

	$query="select
			name,
			profit
		from
			my_tree
		where
			id=$id;";

	$result=mysql_query($query,$bl);
	$line = mysql_fetch_array($result, MYSQL_NUM);
	$name=$line[0];
	$profit=$line[1];
		
	echo "<form action='index.php' method='get' >";
	echo "<h2>Edit record</h2>";
	echo "<input type='hidden' name='content' value='write_node'>";
	echo "<input type='hidden' name='id' value=$id>";
	echo "<table>";
	echo "<tr><th>Company name &nbsp;</th><th><input type='text' name='name' value=$name></th></tr>";
	echo "<tr><th>Profit &nbsp;</th><th><input type='number' name='profit' value=$profit></th></tr></table>";
	echo "<input type='submit' value='write' name='button' />";
	echo "</form>";
}

// Create form to enter data for new record input
function add($id){
	echo "<form action='index.php' method='get' >";
	echo "<h2>Add record</h2>";
	echo "<input type='hidden' name='content' value='add_node'>";
	echo "<input type='hidden' name='id' value=$id>";
	echo "<table>";
	echo "<tr><th>Company name &nbsp;</th><th><input type='text' name='name' value=$name></th></tr>";
	echo "<tr><th>Profit &nbsp;</th><th><input type='number' name='profit' value=$profit></th></tr></table>";
	echo "<input type='submit' value='write' name='button' />";
	echo "</form>";
}

// Write edited data into the database table
function write_node($id, $name, $profit){
	global $configfile;
	$sets=getconfiguration($configfile);
	$bl=connectdb($sets['host'],$sets['username'],$sets['passwd'],$sets['base']);
	$query="update
			my_tree
		set
			name='$name',
			profit='$profit'
		where
			id='$id';";

	$result=mysql_query($query,$bl);
	mysql_free_result($result);
	closedb($bl);
		
	getstru();
}

// Write new record into database table
function add_node($id, $name, $profit){
	global $configfile;
		
	$t_num=get_total();

	if($t_num>0){	
		$sets=getconfiguration($configfile);
	        $bl=connectdb($sets['host'],$sets['username'],$sets['passwd'],$sets['base']);
	        $query="lock table my_tree write;";
	        $result=mysql_query($query,$bl);
	        mysql_free_result($result);

		$query="select
				@rtk:=right_key
			from
				my_tree
			where
				id='$id';";
		$result=mysql_query($query,$bl);
		mysql_free_result($result);

		$query="update
				my_tree
			set
				right_key=right_key+2
			where
				right_key>=@rtk;";

		$result=mysql_query($query,$bl);
		mysql_free_result($result);

		$query="update
				my_tree
			set
				left_key=left_key+2
			where
				left_key>@rtk;";

		$result=mysql_query($query,$bl);
		mysql_free_result($result);
						
		$query="insert into
				my_tree(left_key,right_key,name,profit)
			values
				(@rtk,@rtk+1,'$name','$profit');";
	        $result=mysql_query($query,$bl);
        	mysql_free_result($result);

	        $query="unlock tables;";
       		$result=mysql_query($query,$bl);
	        mysql_free_result($result);

        	closedb($bl);
	}
	else{
	        $sets=getconfiguration($configfile);
        	$bl=connectdb($sets['host'],$sets['username'],$sets['passwd'],$sets['base']);

		$query="insert into
                                my_tree(left_key,right_key,name,profit)
                        values
                                ('1','2','$name','$profit');";
		
		$result=mysql_query($query,$bl);
		mysql_free_result($result);
	};
						
	getstru();
}

// Delete selected record
function del($id){
	global $configfile;
		
	$sets=getconfiguration($configfile);
	$bl=connectdb($sets['host'],$sets['username'],$sets['passwd'],$sets['base']);
	$query="lock table my_tree write;";
	$result=mysql_query($query,$bl);
	mysql_free_result($result);
		
	$query="select
			@lft:=left_key,
			@rgt:=right_key,
			@wdth:=right_key-left_key+1
		from
			my_tree
		where
			id='$id';";

	$result=mysql_query($query,$bl);
	mysql_free_result($result);

	$query="delete from
			my_tree
		where
			left_key between @lft and @rgt;";

	$result=mysql_query($query,$bl);
	mysql_free_result($result);

	$query="update
			my_tree
		set
			right_key=right_key-@wdth
		where
				right_key>@rgt;";

	$result=mysql_query($query,$bl);
	mysql_free_result($result);
						
	$query="update
			my_tree
		set
			left_key=left_key-@wdth
		where
			left_key>@rgt;";

	$result=mysql_query($query,$bl);
	mysql_free_result($result);
						
	$query="unlock tables;";
	$result=mysql_query($query,$bl);
	mysql_free_result($result);

	closedb($bl);
		
	getstru();
}

// Show standart header form
function show_header(){
	echo "<html><head><title>Organization structure</title></head>";
	echo "<body><h1>Organization structure</h1><hr>";	
}

// Show standart footer in the main form
function show_footer(){
		echo "<hr></body></html>";
}

?>

