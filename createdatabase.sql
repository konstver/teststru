CREATE DATABASE teststru;

USE teststru;

CREATE TABLE
       my_tree (
       	       id INTEGER AUTO_INCREMENT PRIMARY KEY,
	       left_key INTEGER NOT NULL,
	       right_key INTEGER NOT NULL,
	       name TEXT,
	       profit DOUBLE,
INDEX
	left_key (left_key, right_key)
);

	
