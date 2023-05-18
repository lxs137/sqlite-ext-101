C header files generate from SQLite v3.41.0

```bash
wget https://github.com/sqlite/sqlite/archive/refs/tags/version-3.41.0.tar.gz
tar xvzf version-3.41.0.tar.gz

cd sqlite-version-3.41.0
./configure && make sqlite3.h && make sqlite3ext.h
```