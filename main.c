
/*
 * Copyright (c) 2017, Brian Leishman
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * USAGE INSTRUCTIONS:
 *
 * make sure libmysqlclient-dev is installed:
 * apt-get install libmysqlclient-dev
 *
 * Replace "/usr/lib/mysql/plugin" with your MySQL plugins directory (can be found by running "select @@plugin_dir;")
 * gcc -O3 -I/usr/include/mysql -o murmurhash3.so -shared main.c -fPIC && cp murmurhash3.so /usr/lib/mysql/plugin/murmurhash3.so
 *
 * Then, on the server:
 * create function`murmurhash3`returns integer soname'murmurhash3.so';
 *
 * And use/test like:
 * select cast(`murmurhash3`('yeet')as unsigned); -- should return 3329607078
 *
 * Yeet!
 *
 */

#ifdef STANDARD
/* STANDARD is defined, don't use any mysql functions */
#include <string.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong; /* Microsofts 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>
#if defined(MYSQL_SERVER)
#include <m_string.h>
#else
/* when compiled as standalone */
#include <string.h>
#endif
#endif
#include <mysql.h>
#include <stdint.h>
#include "include/murmur3.c"

#ifdef HAVE_DLOPEN

my_bool murmurhash3_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
void murmurhash3_deinit(UDF_INIT *initid);
uint32_t murmurhash3(UDF_INIT *initid __attribute__ ((unused)), UDF_ARGS *args, char *is_null, char *message __attribute__ ((unused)));

my_bool murmurhash3_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
	if (args->arg_count != 1) {
		strcpy(message, "`murmurhash3`() requires 1 parameter: the string to be encoded");
		return 1;
	}

	args->arg_type[1] = STRING_RESULT;

	initid->maybe_null = 0; //cannot return null

	return 0;
}

#define HASH_32_INIT 0x7a239f37UL
#define HASH_NULL_DEFAULT 0x0a0b0c0d

uint32_t murmurhash3(UDF_INIT *initid __attribute__ ((unused)), UDF_ARGS *args, char *is_null, char *message __attribute__ ((unused))) {

	uint32_t hash[1];
	uint32_t seed = HASH_32_INIT;

	if (args->args[0] != NULL) {
		MurmurHash3_x86_32(args->args[0], args->lengths[0], seed, hash);
	} else {
		uint32_t null_default = HASH_NULL_DEFAULT;
		MurmurHash3_x86_32(&null_default, sizeof(null_default), seed, hash);
	}

	return hash[0];
}

#endif /* HAVE_DLOPEN */