/* ************************************************************************ */
/*																			*/
/*  Neko-Aspell - a Neko VM library that wraps GNU Aspell					*/
/*  Copyright (c)2006,2007													*/
/*  Edwin van Rijkom														*/
/*																			*/
/*  The development of this library was kindly sponsored by Artifex			*/
/*  (www.afxfirm.com)														*/
/*																			*/
/* This library is free software; you can redistribute it and/or			*/
/* modify it under the terms of the GNU Lesser General Public				*/
/* License as published by the Free Software Foundation; either				*/
/* version 2.1 of the License, or (at your option) any later version.		*/
/*																			*/
/* This library is distributed in the hope that it will be useful,			*/
/* but WITHOUT ANY WARRANTY; without even the implied warranty of			*/
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU		*/
/* Lesser General Public License or the LICENSE file for more details.		*/
/*																			*/
/* ************************************************************************ */

#include <neko/neko.h>
#include <neko/neko_vm.h>
#include <aspell/aspell.h>

#include <stdlib.h>
#include <string.h>

/* Library loading
 *
 */ 

static char lib_path[1024];
 
#ifdef NEKO_WINDOWS

#include <Windows.h>

#define LIBASPELL_PATH "libaspell-15.dll"
#define resolve(L,S) GetProcAddress(L,S)

HINSTANCE lib(){
	static HINSTANCE hlib = NULL;
	if (!hlib) hlib = LoadLibrary(lib_path);
	if(!hlib) {		
		failure("Failed to load GNU Aspell library ("LIBASPELL_PATH")");
	}
	return hlib;
}

#endif

#if defined(NEKO_MAC) || defined(NEKO_LINUX)

#include <dlfcn.h>

#if defined(NEKO_MAC)
#define LIBASPELL_PATH "libaspell.15.dylib"
#elif defined(NEKO_LINUX)
#define LIBASPELL_PATH "libaspell.so.15"
#endif

#define resolve(L,S) dlsym(L,S)

void *lib(){
        static void *handle = NULL;
        if(!handle) handle = dlopen(lib_path, RTLD_NOW | RTLD_GLOBAL);
        if(!handle) {
                failure("Failed to load GNU Aspell library ("LIBASPELL_PATH")");
        }
        return handle;
}

#endif

#define STUB0R(NAME,R) R NAME() {static R (*call)() = NULL; if (!call) call = (R (*)()) resolve(lib(),#NAME); return call();}
#define STUB1R(NAME,R,A1) R NAME(A1 a1) {static R (*call)(A1) = NULL; if (!call) call = (R (*)(A1)) resolve(lib(),#NAME); return call(a1);}
#define STUB3R(NAME,R,A1,A2,A3) R NAME(A1 a1,A2 a2, A3 a3) {static R (*call)(A1,A2,A3) = NULL; if (!call) call = (R (*)(A1,A2,A3)) resolve(lib(),#NAME); return call(a1,a2,a3); }
#define STUB1V(NAME,A1)	void NAME(A1 a1) {static void (*call)(A1) = NULL; if (!call) call = (void (*) (A1)) resolve(lib(),#NAME); call(a1);}

STUB1V(delete_aspell_can_have_error,struct AspellCanHaveError *)
STUB1V(delete_aspell_speller,struct AspellSpeller *)
STUB1V(delete_aspell_config,struct AspellConfig *)
STUB0R(new_aspell_config,struct AspellConfig *)
STUB1R(to_aspell_speller,struct AspellSpeller *,struct AspellCanHaveError *)
STUB3R(aspell_config_replace,int,struct AspellConfig *,const char *,const char *)
STUB1R(aspell_error_number,unsigned int,const struct AspellCanHaveError *)
STUB1R(aspell_error_message,const char *,const struct AspellCanHaveError *)
STUB1R(new_aspell_speller,struct AspellCanHaveError *,struct AspellConfig *)
STUB3R(aspell_speller_check,int,struct AspellSpeller *, const char *, int)
STUB1V(delete_aspell_string_enumeration,struct AspellStringEnumeration *)
STUB1R(aspell_string_enumeration_next,const char *,struct AspellStringEnumeration *)
STUB1R(aspell_word_list_size,unsigned int,const struct AspellWordList *)
STUB1R(aspell_word_list_elements,struct AspellStringEnumeration *,const struct AspellWordList *)
STUB3R(aspell_speller_suggest,const struct AspellWordList *,struct AspellSpeller *, const char *, int)

/* Storage
 *
 */
typedef struct nas_spellers {
	AspellSpeller		*speller;	
	void				*config;
	int					deleted;
	struct nas_spellers *next;
} nas_spellers;

typedef struct nas_errors {	
	AspellCanHaveError	*error;
	void				*config;
	int					deleted;
	struct nas_errors	*next;
} nas_errors; 

typedef struct nas_config {
	AspellConfig		*config;	
	nas_spellers		*spellers;
	nas_errors			*errors;
} nas_config;

static nas_spellers *addSpeller(nas_config *c, AspellSpeller *a) {
	nas_spellers *s;
	nas_spellers *add = malloc(sizeof(nas_spellers));
	add->speller = a;
	add->config = c;
	add->deleted = 0;
	add->next = NULL;
	
	if ((s=c->spellers)==NULL)
		c->spellers = add;
	else {
		while(s->next) s = s->next;
		s->next = add;
	}
	return add;
}

static nas_errors *addError(nas_config *c, AspellCanHaveError *a) {
	nas_errors *e;
	nas_errors *add = malloc(sizeof(nas_errors));
	add->error = a;
	add->config = c;
	add->deleted = 0;
	add->next = NULL;
	
	if ((e=c->errors)==NULL)
		c->errors = add;
	else {
		while(e->next) e = e->next;
		e->next = add;
	}
	return add;
}

static void removeSpeller(nas_config *c, AspellSpeller *r) {	
	nas_spellers *s = c->spellers;
	nas_spellers *p = s;
	while(s) {
		if (s->speller==r) {
			if (p == c->spellers)
				c->spellers = s->next;				
			else
				p->next = s->next;
				
			free(s);
			return;
		}
		p = s;
		s = s->next;	
	}
}

static void removeError(nas_config *c, AspellCanHaveError *r) {
	nas_errors *e = c->errors;
	nas_errors *p = e;
	while(e) {
		if (e->error==r) {
			if (p == c->errors)
				c->errors = e->next;				
			else
				p->next = e->next;
				
			free(e);
			return;
		}
		p = e;
		e = e->next;
	}	
}

/* Kinds
 *
 */
#define val_nas_config(x)	((nas_config *)val_data(x))
#define val_nas_error(x)	((nas_errors *)val_data(x))
#define val_nas_speller(x)	((nas_spellers *)val_data(x))

DEFINE_KIND(k_nas_config);
DEFINE_KIND(k_nas_error);
DEFINE_KIND(k_nas_speller);

/* Tools
 *
 */
void finalize( value v ) {
	if (val_type(v)==VAL_ABSTRACT) {		
		if (val_kind(v)==k_nas_config) {
			// config			
			nas_config *c = val_nas_config(v);			
			nas_errors *es = c->errors;
			nas_spellers *ss = c->spellers;
			// flag spellers deleted
			// ASpell deletes them on deleting the config:
			while(ss) {				
				ss->deleted = 1;
				ss->speller = NULL;
				ss->config = NULL;				
				ss = ss->next;				
			}
			// flag errors deleted
			// ASpell deletes them on deleting the config:
			while(es) {
				es->deleted = 1;
				es->error = NULL;
				es->config = NULL;
				es = es->next;				
			}
			delete_aspell_config(c->config);
			free(c);			
		} else 		
		if (val_kind(v)==k_nas_speller) {
			// speller			
			nas_spellers *s = val_nas_speller(v);
			if (!s->deleted) {
				s->deleted = 1;
				delete_aspell_speller(s->speller);
			}
			if (s->config)				
				removeSpeller(s->config,s->speller);
			else
				free(s);
		}
		if (val_kind(v)==k_nas_error) {
			// error			
			nas_errors *e = val_nas_error(v);
			if (!e->deleted) {
				e->deleted = 1;
				delete_aspell_can_have_error(e->error);
			}
			if (e->config)
				removeError(e->config,e->error);
			else				
				free(e);
		}
	}	
}

/* Configuration
 *
 */

// Construct a new configuration
static value nas_new_config(value path) {
	nas_config *c = malloc(sizeof(nas_config));
	value r;
	
	// get prefered path if specified:
	strcpy((char*)lib_path,path == val_null? LIBASPELL_PATH : val_string(path));

	c->config = new_aspell_config();
	c->errors = NULL;
	c->spellers = NULL;

	r = alloc_abstract(k_nas_config,c);
	val_gc(r,finalize);
	return r;
}
DEFINE_PRIM(nas_new_config,1);

// Replace a configuration setting
static value nas_config_replace(value config, value var, value val) {
	nas_config *c;
	
	val_check_kind(config,k_nas_config);
	val_check(var,string);
	val_check(val,string);
	c = val_nas_config(config);
	if (c) {
		aspell_config_replace(c->config,val_string(var),val_string(val));
	}
	return val_null;
}
DEFINE_PRIM(nas_config_replace,3);

/* Error
 *
 */

// Get the error number for an error
static value nas_error_number(value error) {
	val_check_kind(error,k_nas_error);
	return alloc_int(aspell_error_number(val_nas_error(error)->error));
}
DEFINE_PRIM(nas_error_number,1);

// Get the message for an error
static value nas_error_message(value error) {
	val_check_kind(error,k_nas_error);
	return alloc_string(aspell_error_message(val_nas_error(error)->error));
}
DEFINE_PRIM(nas_error_message,1);

// Convert an error into a speller 
static value nas_error_to_speller(value error, value config) {
	AspellSpeller *s;
	nas_errors *e;
	nas_config *c;		
	value r;
	
	val_check_kind(error,k_nas_error);
	val_check_kind(config,k_nas_config);
	e = val_nas_error(error);
	c = val_nas_config(config);	
	// calls delete_can_have_error on the incoming error:
	s = to_aspell_speller(val_nas_error(error)->error);
	// so we raise the deletion flag:
	e->deleted = 1;	
	r = alloc_abstract(k_nas_speller,addSpeller(c,s));		
	val_gc(r,finalize);
	return r; 
}
DEFINE_PRIM(nas_error_to_speller,2);

/* Speller
 *
 */

// Create a new speller for a configuration:
static value nas_new_speller(value config) {
	AspellCanHaveError *e;
	nas_config *c;		
	value r;
	
	val_check_kind(config,k_nas_config);
	c = val_nas_config(config);
	e = new_aspell_speller(c->config);	
	
	r = alloc_abstract(k_nas_error,addError(c,e));	
	val_gc(r,finalize);
	return r;
}
DEFINE_PRIM(nas_new_speller,1);

static value nas_speller_check(value speller, value word) {	
	val_check_kind(speller,k_nas_speller);
	val_check(word,string);
	return alloc_int(
		aspell_speller_check
			( val_nas_speller(speller)->speller
			, val_string(word),val_strlen(word)
			)
		);
}
DEFINE_PRIM(nas_speller_check,2);

static value nas_speller_suggest(value speller, value word) {
	val_check_kind(speller,k_nas_speller);
	val_check(word,string);
	{
		nas_spellers *s = val_nas_speller(speller);
		const struct AspellWordList *suggestions = aspell_speller_suggest
			( s->speller
			, val_string(word),val_strlen(word)
			);
		AspellStringEnumeration *elements 
			= aspell_word_list_elements(suggestions);
		const char *word;
		int count = aspell_word_list_size(suggestions);
		int i = 0;
		value result = alloc_array(count);
		while( (word = aspell_string_enumeration_next(elements)) != NULL) {
			val_array_ptr(result)[i] = alloc_string(word);
			i++;
		}
		delete_aspell_string_enumeration(elements);
		return result;
	}
}
DEFINE_PRIM(nas_speller_suggest,2);
