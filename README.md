# Usage Instructions:

make sure libmysqlclient-dev is installed:
`apt install libmysqlclient-dev`

Replace `/usr/lib/mysql/plugin` with your MySQL plugins directory (can be found by running `select @@plugin_dir;`)
`gcc -O3 -I/usr/include/mysql -o murmurhash3.so -shared main.c -fPIC && cp murmurhash3.so /usr/lib/mysql/plugin/murmurhash3.so`

Then, on the server:
``create function`murmurhash3`returns integer soname'murmurhash3.so';``

And use/test like:
``select cast(`murmurhash3`('yeet')as unsigned); -- should return 3329607078``

Yeet!
