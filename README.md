MysticPrism FILE SYSTEM ##
BY: Mahesh R. Baheti##
An experimental filesystem model

> compiled under gcc 4.7.2  OS f17

- the filesystem resides in file mpfs.fs
- if mpfs.fs is not present IT WILL BE CREATED AUTOMATICALLY.
- all file descriptors written to disk.
- it is implemented as a data structure like tree where
  every SIMIILAR siblings has adress links to adjacent 
  siblings and links to parents and childrens.
- e.g.
  consider directory structure 
  root
  |cde\	abc\ a.txt d.txt 
	|b.txt c.txt

  tree:
	root _______
	|           \
	cde->abc    a.txt->d.txt
	      |
	      b.txt->c.txt

- The code is self explanatory as comments are included.

- commands:
	cd [DIRECTORY_NAME]
    		changes directory
		.. to access parent
	mkdir [DIR NAME]
		create directory in current direcory
	mkf [FILE NAME]
		create file
	ls
		list current directory
	rm [FILE NAME]
		delete file in current directory
	rmdir [DIR NAME]
		delete a directory in current directory
	cat [FILE NAME]
		displays file contents in current directory
	exit
		exit MPFS
- NO FULL PATHS ALLOWED
