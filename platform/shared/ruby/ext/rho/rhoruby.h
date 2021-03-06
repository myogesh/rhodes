/*
 *  rhoruby.h
 *  rhorubylib
 *
 *  Created by evgeny vovchenko on 10/1/08.
 *  Copyright 2008 RhoMobile. All rights reserved.
 *
 */
#ifndef _RHO_RVM_H
#define _RHO_RVM_H

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef __SYMBIAN32__
#include <sys/types.h>
#endif //__SYMBIAN32__

#ifndef RUBY_RUBY_H
typedef unsigned long VALUE;
#endif //!RUBY_RUBY_H
	
#if defined( OS_WINDOWS ) || defined( OS_WINCE )
typedef unsigned __int64 uint64__;
#else
typedef unsigned long long uint64__;
#endif
	
void RhoRubyStart();
void RhoRubyThreadStart();
void RhoRubyThreadStop();

VALUE getnil();	
VALUE createHash();
VALUE addTimeToHash(VALUE hash, const char* key, time_t val);	
VALUE addIntToHash(VALUE hash, const char* key, int val);	
VALUE addStrToHash(VALUE hash, const char* key, const char* val, int len);
VALUE addHashToHash(VALUE hash, const char* key, VALUE val);	

char* getStringFromValue(VALUE val);
int getStringLenFromValue(VALUE val);
void  releaseValue(VALUE val);

VALUE callFramework(VALUE hashReq);
VALUE callServeIndex(char* index_name);
void RhoRuby_RhomAttribManager_save(int nSrcID);
void RhoRuby_RhomAttribManager_delete_attribs(int nSrcID,uint64__ objID);
void RhoRuby_RhomAttribManager_add_attrib(int nSrcID,const char* szAttrib);

void RhoRubyStop();

const char* RhoGetRootPath();


#if defined(__cplusplus)
}
#endif
		
	
#endif //_RHO_RVM_H
