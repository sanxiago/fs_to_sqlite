# fs_to_sqlite
A program to create a sqlitedb with the metadata of a filesystem
lists recursively all files and directories, from the current
working directory, into a sqlite3 database.

# Requirements 
sqlite3-dev
On Debian/Ubuntu you can install with:
```bash
sudo apt-get install libsqlite3-dev
```

#  Build Instructions:
```bash
gcc -o fs_to_sqlite fs_to_sqlite.c -lsqlite3
```

# Usage
```bash
./fs_to_sqlite 
Usage: ./fs_to_sqlite [-v] <directory> <sqlite_db>
```

# Other projects that already do this:
fs2sqlite - Python https://github.com/pletnes/fs2sqlite

