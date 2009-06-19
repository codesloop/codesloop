/*
Copyright (c) 2008,2009. David Beck, Tamas Foldi

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
   @file t__zfile.cc
   @brief Tests to verify zfile
*/

#include "zfile.hh"
#include "test_timer.h"
#include "common.h"
#include <assert.h>
#include <zlib.h>

using namespace csl::common;

/** @brief contains tests related to zfile */
namespace test_zfile {

/**
   @test Simple test to see how zlib works
*/
void test_plain_zlib()
{
  z_stream strm, strm2 ;
  memset( &strm,0,sizeof(strm) );
  memset( &strm2,0,sizeof(strm2) );

  Bytef p[16];
  unsigned char r[400];
  unsigned char r2[400];
  memset( r, 0xff, sizeof(r) );
  memset( r2, 0xff, sizeof(r2) );
  memcpy( p,"Hello world",strlen("Hello world")+1 );

  assert( deflateInit2( &strm, Z_BEST_SPEED, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY ) == Z_OK );

  strm.next_in = p;
  strm.avail_in = strlen( reinterpret_cast<const char *>(p) )+1;
  strm.next_out = r;
  strm.avail_out = sizeof(r);

  assert( deflate(&strm, Z_NO_FLUSH ) == Z_OK );
  assert( deflate(&strm, Z_FINISH ) == Z_STREAM_END );
  deflateEnd(&strm);

  assert( inflateInit2( &strm2, -MAX_WBITS ) == Z_OK );
  strm2.next_in = r;
  strm2.avail_in = strm.total_out;
  strm2.next_out = r2;
  strm2.avail_out = sizeof(r2);
  assert( inflate( &strm2, Z_SYNC_FLUSH ) >= 0 );

  inflateEnd(&strm2);

  assert( memcmp( r2,p, strlen("Hello world")+1 ) == 0 );
}

/**
   @test Read a file and check the read size

   The calculation summs the used uncompressed buffer sizes.
*/
void test_simple_read()
{
  zfile zf;
  assert( (zf.read_file("test_4_zfile.txt")) );
  assert( (zf.get_size() == 12296 ) );
}


/**
   @test Read a file, compress and check the compressed size

   The get_zsize() function enforces the compression of the read file.
   Then it calculates the total size of the compressed buffers.
*/
void test_compressed_size()
{
  zfile zf;

  /* this prevents valgrind complaining */
  zf.custom_zlib_allocator( true );
  zf.init_custom_memory( true );

  assert( (zf.read_file("test_4_zfile.txt")) );
  assert( (zf.get_zsize() > 0 ) );
}

/**
   @test Read a file and print debug infos
*/
void test_compressed_size_dbg()
{
  zfile zf;

  /* this prevents valgrind complaining */
  zf.custom_zlib_allocator( true );
  zf.init_custom_memory( true );

  assert( (zf.read_file("test_4_zfile.txt")) );
  printf("%ld [c]=> %ld \n", static_cast<unsigned long>(zf.get_size()), static_cast<unsigned long>(zf.get_zsize()) );
}

/**
   @test Compress a memory buffer and do various checks

   @li Put data into the uncompressed buffer
   @li Checks the buffer size
   @li Query the compressed size, which in turn compresses the data
   @li Gets the compressed data
   @li Put the compressed data into a different zfile
   @li Check the uncompressed size of the second zfile, which uncompresses the data
   @li Get the uncompressed data from the second zfile
   @li Compare the uncompressed data w/ the original data
*/
void test_compressed_data1()
{
  const char * p = "Hello world";
  unsigned char r[400];
  unsigned char r2[400];

  zfile zf,zf2;

  assert( zf.put_data( reinterpret_cast<const unsigned char*>(p), strlen(p)+1 ) );
  assert( zf.get_size() == strlen(p)+1 );
  assert( zf.get_zsize() > 0 );
  assert( zf.get_zdata(r) );

  assert( zf2.put_zdata(r,zf.get_zsize()) );
  assert( zf2.get_size() == strlen(p)+1 );
  assert( zf2.get_data(r2) );
  assert( memcmp(r2,p,zf.get_size()) == 0 );
}

/**
   @test Test reading and writing uncompressed and compressed files

   This tests the file sizes and the data too.
*/
void test_compressed_data2()
{
  unsigned char t1[13000];
  unsigned char t2[13000];
  unsigned char z1[13000];
  unsigned char z2[13000];

  memset( t1,0xff, sizeof(t1) );
  memset( t2,0xff, sizeof(t2) );
  memset( z1,0xff, sizeof(z1) );
  memset( z2,0xff, sizeof(z2) );

  zfile zf1, zf2, zf3;

  /* this prevents valgrind complaining */
  zf1.custom_zlib_allocator( true );
  zf1.init_custom_memory( true );

  assert( (zf1.read_file("test_4_zfile.txt")) );
  assert( (zf1.write_zfile("test_4_zfile.txt.z1")) );
  assert( (zf3.read_zfile("test_4_zfile.txt.z1")) );

  assert( (zf3.get_size()) == 12296 );

  assert( (zf1.get_zdata(z1)) );
  assert( (zf1.get_data(t1)) );
  assert( (zf2.put_zdata(z1,zf1.get_zsize())) );
  assert( (zf2.get_zdata(z2)) );
  assert( zf1.get_zsize() == 6759 );
  assert( memcmp( z1,z2,zf1.get_zsize()) == 0 );

  assert( zf2.get_zsize() == zf1.get_zsize() );

  assert( (zf1.get_data(t1)) );
  assert( (zf2.get_data(t2)) );
  assert( memcmp( t1,t2,sizeof(t1)) == 0 );
}

/**
   @test Testing the zfile::read_file() function
*/
void test_fun__read_file()
{
  zfile zf;
  assert( zf.read_file("non.existant.file") == false );
  assert( zf.read_file(0) == false );
  assert( zf.read_file("test_4_zfile.txt") == true );
  assert( zf.get_size() == 12296 );
}

/**
   @test Testing the zfile::read_zfile() function
*/
void test_fun__read_zfile()
{
  zfile zf;
  assert( zf.read_zfile("non.existant.file") == false );
  assert( zf.read_zfile(0) == false );
  assert( zf.read_zfile("test_4_zfile.txt.zf") == true );
  assert( zf.get_zsize() == 6759 );
}

/**
   @test Testing the zfile::write_file() function
*/
void test_fun__write_file()
{
  zfile zf;
  unsigned char t[2] = { 0, 0 };

  assert( zf.put_data(t,2) == true );
  assert( zf.write_file("non.existant.dir/non.existant.file") == false );
  assert( zf.write_file(0) == false );
}

/**
   @test Testing the zfile::write_zfile() function
*/
void test_fun__write_zfile()
{
  zfile zf;
  unsigned char t[2] = { 0, 0 };

  assert( zf.put_data(t,2) == true );
  assert( zf.write_zfile("non.existant.dir/non.existant.file") == false );
  assert( zf.write_zfile(0) == false );
}

/**
   @test Testing the zfile::get_size() function
*/
void test_fun__get_size()
{
  zfile zf;
  unsigned char t[2] = { 0xb, 0xd };
  assert( zf.put_data(t,2) == true );
  assert( zf.get_size() == 2 );
}

/**
   @test Testing the zfile::get_data() function
*/
void test_fun__get_data()
{
  zfile zf;
  unsigned char t[2]  = { 0xb, 0xd };
  unsigned char t2[3] = { 0xa, 0xb, 0xc };

  assert( zf.put_data(t,2) == true );
  assert( zf.get_data(0) == false );
  assert( zf.get_data(t2) == true );
  assert( t2[0] = 0xb && t2[1] == 0xd && t2[2] == 0xc );
}

/**
   @test Testing the zfile::put_data() function
*/
void test_fun__put_data()
{
  zfile zf;
  unsigned char t[2]  = { 0xb, 0xd };
  assert( zf.put_data(0,2) == false );
  assert( zf.put_data(t,0) == false );
  assert( zf.put_data(0,0) == false );
  assert( zf.put_data(t,2) == true );
}

/**
   @test Testing the zfile::get_zsize() function
*/
void test_fun__get_zsize()
{
  zfile zf;
  unsigned char t[2] = { 0xb, 0xd };
  assert( zf.put_data(t,2) == true );
  assert( zf.get_size() == 2 );
  assert( zf.get_zsize() > 0 );
}

/**
   @test Testing the zfile::get_zdata() function
*/
void test_fun__get_zdata()
{
  zfile zf;
  unsigned char t[2]  = { 0xb, 0xd };
  unsigned char t2[3] = { 0xa, 0xb, 0xc };

  assert( zf.put_zdata(t,2) == true );
  assert( zf.get_zdata(0) == false );
  assert( zf.get_zdata(t2) == true );
  assert( t2[0] = 0xb && t2[1] == 0xd && t2[2] == 0xc );
}

/**
   @test Testing the zfile::put_zdata() function
*/
void test_fun__put_zdata()
{
  zfile zf;
  unsigned char t[2]  = { 0xb, 0xd };
  assert( zf.put_zdata(0,2) == false );
  assert( zf.put_zdata(t,0) == false );
  assert( zf.put_zdata(0,0) == false );
  assert( zf.put_zdata(t,2) == true );
}

/**
   @test Testing the zfile::get_buff() function
*/
void test_fun__get_buff()
{
  zfile zf;
  unsigned char t[2]  = { 0xb, 0xd };
  assert( zf.put_data(t,2) == true );
  assert( zf.get_buff() != 0 );
}

/**
   @test Testing the zfile::get_zbuff() function
*/
void test_fun__get_zbuff()
{
  zfile zf;
  unsigned char t[2]  = { 0xb, 0xd };
  assert( zf.put_zdata(t,2) == true );
  assert( zf.get_zbuff() != 0 );
}

void zfile_copy()
{
  /** @todo test zfile copy */
}

void zfile_const_ops()
{
  /** @todo test zfile constant operations */
}

void get_pbuf()
{
  /** @todo test zfile pbuf interworking */
}

void put_pbuf()
{
  /** @todo test zfile pbuf interworking */
}

} // end of namespace test_zfile

using namespace test_zfile;

int main()
{
  csl_common_print_results( "test_plain_zlib         ",csl_common_test_timer_v0(test_plain_zlib),"" );
  csl_common_print_results( "test_simple_read        ",csl_common_test_timer_v0(test_simple_read), "" );
  csl_common_print_results( "test_compressed_size    ",csl_common_test_timer_v0(test_compressed_size), "" );
  csl_common_print_results( "test_compressed_data1   ",csl_common_test_timer_v0(test_compressed_data1), "" );
  csl_common_print_results( "test_compressed_data2   ",csl_common_test_timer_v0(test_compressed_data2), "" );
  csl_common_print_results( "test_fun__read_file     ",csl_common_test_timer_v0(test_fun__read_file), "" );
  csl_common_print_results( "test_fun__write_file    ",csl_common_test_timer_v0(test_fun__write_file), "" );
  csl_common_print_results( "test_fun__put_data      ",csl_common_test_timer_v0(test_fun__put_data), "" );
  csl_common_print_results( "test_fun__get_size      ",csl_common_test_timer_v0(test_fun__get_size), "" );
  csl_common_print_results( "test_fun__get_data      ",csl_common_test_timer_v0(test_fun__get_data), "" );
  csl_common_print_results( "test_fun__get_buff      ",csl_common_test_timer_v0(test_fun__get_buff), "" );
  csl_common_print_results( "test_fun__read_zfile    ",csl_common_test_timer_v0(test_fun__read_zfile), "" );
  csl_common_print_results( "test_fun__write_zfile   ",csl_common_test_timer_v0(test_fun__write_zfile), "" );
  csl_common_print_results( "test_fun__put_zdata     ",csl_common_test_timer_v0(test_fun__put_zdata), "" );
  csl_common_print_results( "test_fun__get_zsize     ",csl_common_test_timer_v0(test_fun__get_zsize), "" );
  csl_common_print_results( "test_fun__get_zdata     ",csl_common_test_timer_v0(test_fun__get_zdata), "" );
  csl_common_print_results( "test_fun__get_zbuff     ",csl_common_test_timer_v0(test_fun__get_zbuff), "" );
  
  test_compressed_size_dbg();

  return 0;
}

/* EOF */
