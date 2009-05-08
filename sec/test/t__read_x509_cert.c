/*
Copyright (c) 2008,2009, David Beck

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
   @file t__read_x509_cert.c
   @brief tests to see how X509 and OpenSSL works
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <assert.h>

void test1()
{
  char *       filename = "server/server.crt";
  X509 *       x509 = NULL;
  X509 *       rc = NULL;
  FILE *       fp = NULL;
  X509_NAME *  name = NULL;
  char         buf[1024];
  int          nid = NID_undef;
  size_t       name_len = 0;
  char         cnbuf[1024];

  assert( (fp = fopen(filename,"rb")) != NULL );
  assert( (rc = PEM_read_X509(fp, &x509, (pem_password_cb *)0, NULL)) != NULL );
  assert( (name = X509_get_subject_name(x509)) != NULL );

  X509_NAME_oneline(X509_get_subject_name(x509), buf, sizeof(buf));
  assert( strcmp(buf,"/C=HU/ST=Hungary/L=Budapest/O=BeckGround Ltd./CN=server@beckground.hu/emailAddress=server@beckground.hu") == 0 );
  printf("%s\n",buf);

  assert( (nid = OBJ_txt2nid("commonName")) != NID_undef );
  assert( (name_len = X509_NAME_get_text_by_NID(name, nid, cnbuf, sizeof(cnbuf))) > 0 );
  assert( strcmp(cnbuf,"server@beckground.hu") == 0 );
  printf("%s\n",cnbuf);

  X509_free(x509);
  rc = x509 = NULL;
  fclose( fp );
}

int main()
{
  test1();
  return 0;
}

/* EOF */
