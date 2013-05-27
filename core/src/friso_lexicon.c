/*
 * friso lexicon implemented functions.
 * 		used to deal with the friso lexicon, like: load,remove,match...
 * 
 * @author	chenxin
 * @email	chenxin619315@gmail.com
 */
#include "friso_API.h"
#include "friso.h"
#include <stdlib.h>
#include <string.h>

#define __SPLIT_MAX_TOKENS__ 	5
#define __LEX_FILE_DELIME__ 	'#'
#define __FRISO_LEX_IFILE__		"friso.lex.ini"

//create a new lexicon
__EXTERN_API__ friso_dic_t friso_dic_new() {
	
	register uint_t t;
	friso_dic_t dic = ( friso_dic_t ) FRISO_CALLOC( sizeof( friso_hash_t ), __FRISO_LEXICON_LENGTH__ );
	if ( dic == NULL ) {
		___ALLOCATION_ERROR___
	}
	
	for ( t = 0; t < __FRISO_LEXICON_LENGTH__; t++ ) {
		dic[t] = new_hash_table();
	}
	
	return dic;
}

__EXTERN_API__ void friso_dic_free( friso_dic_t dic ) {
	register uint_t t;
	for ( t = 0; t < __FRISO_LEXICON_LENGTH__; t++ ) {
		free_hash_table( dic[t], 1 );
	}
	FRISO_FREE( dic );
}


//create a new lexicon entry
__EXTERN_API__ lex_entry_t new_lex_entry( string word, string syn, uint_t fre, char length, char type ) {
	
	lex_entry_t e = ( lex_entry_t ) FRISO_MALLOC( sizeof( lex_entry_cdt ) );
	if ( e == NULL ) {
		___ALLOCATION_ERROR___
	} 

	//initialize.
	e->word	= word;
	e->syn	= syn;
	e->fre	= fre;
	e->length = length;
	e->type = type;

	return e;
}

//free the given lexicon entry.
__EXTERN_API__ void free_lex_entry( lex_entry_t e ) {
	FRISO_FREE( e );
}


//add a new entry to the dictionary.
__EXTERN_API__ void friso_dic_add( friso_dic_t dic, friso_lex_t lex,
				string word, string syn ) {
	if ( lex >= 0 || lex < __FRISO_LEXICON_LENGTH__ ) {
		//printf("lex=%d, word=%s, syn=%s\n", lex, word, syn);
		hash_put_mapping( dic[lex], word, new_lex_entry( word, syn, 0, ( char ) strlen(word), ( char ) lex ) );
	}
}

__EXTERN_API__ void friso_dic_add_with_fre( friso_dic_t dic, friso_lex_t lex,
				string word, string syn, uint_t frequency ) {
	if ( lex >= 0 && lex < __FRISO_LEXICON_LENGTH__ ) {
		hash_put_mapping( dic[lex], word, new_lex_entry( word, syn, frequency, ( char ) strlen(word), ( char ) lex ) );
	}
}

/*
 * read a line from a specified string.
 * 		the newline will be cleared.
 * 
 * @date	2012-11-24 
 */
__EXTERN_API__ string file_get_line( string __dst, FILE * _stream ) {

	register int c;
	string cs;
	
	cs = __dst;
	while ( ( c = fgetc( _stream ) ) != EOF ) {
		if ( c == '\n' ) break;
		*cs++ = c; 
	}
	*cs = '\0';
	
	return ( c == EOF && cs == __dst ) ? NULL : __dst;
	
}

/*
 * static function to copy a string. 
 */
///instead of memcpy
__STATIC_API__ string string_copy( string _src, string __dst, uint_t blocks ) {
	
	register string __src = _src;
	register uint_t t;
	
	for ( t = 0; t < blocks; t++ ) {
		if ( *__src == '\0' ) break; 
		__dst[t] = *__src++;
	}
	__dst[t] = '\0';
	
	return __dst;
	
}

/*
 * find the postion of the first appear of the given char.
 *	adress of the char in the string will be return
 *		if found or return NULL
 */
__STATIC_API__ string indexOf( string __str, char delimiter ) {

	uint_t i, __length__;

	__length__ = strlen( __str );
	for ( i = 0; i < __length__; i++ ) {
		if ( __str[i] == delimiter ) 
			return __str + i;
	}

	return NULL;
}

/*
 * split the given string with the given delimiter.
 * 
 * @return the pointer of all the tokens. 
 */
__STATIC_API__ string * string_split( string *__tokens, string str, const char delim ) {
	
	register uint_t t, _toks = 0, \
					length = strlen( str );
	string ch, s = str;
	
	//clear the tokens
	for ( t = 0; t < __SPLIT_MAX_TOKENS__; t++ ) {
		__tokens[t] = NULL;
	}
	
	//get the number of the tokens
	for ( t = 0; t < length; t++ ) {
		if ( str[t] == delim ) 
			_toks++;
	}
	
	if ( _toks > 0 ) {
		_toks = 0;
		while ( ( ch = indexOf(s, delim) ) != NULL ) {
			t = ch - s + 1;
			__tokens[_toks] = string_copy( s, ( string ) FRISO_MALLOC( t ), t - 1);
			s = ch + 1;
			_toks++;
			if ( _toks >= __SPLIT_MAX_TOKENS__  ) {
				break;
			} 
		}
		
		//have not reach the end, read the left chars.
		if ( _toks < __SPLIT_MAX_TOKENS__ && *s != '\0' ) {
			t = str + length - s + 1;
			__tokens[_toks] = string_copy( s, ( string ) FRISO_MALLOC( t ), t - 1);
		}
		
	} else {				
		//a string without tokens.
		t = length + 1;
		__tokens[_toks] = string_copy( s, ( string ) FRISO_MALLOC( t ), t - 1);
	}
	
	return __tokens;
	
}

//load words from a lexicon file.
__EXTERN_API__ void friso_dic_load( friso_dic_t dic, friso_lex_t lex, \
			string lex_file, uint_t length ) {
	
	FILE * _stream;
	register uint_t t;
	char __char[1024];
	string _line, __tokens[__SPLIT_MAX_TOKENS__];
	
	if ( ( _stream = fopen( lex_file, "rb" ) ) != NULL ) {
		
		while ( ( _line = file_get_line( __char, _stream ) ) != NULL ) {
			//passing the lexicon notes.
			if ( indexOf( _line, __LEX_FILE_DELIME__ ) == _line ) continue;
			
			string_split( __tokens, _line, '/' );
			if ( __tokens[0] != NULL ) {
				if ( strlen( __tokens[0] ) <= length ) {
					if ( __tokens[2] != NULL ) {					//word freedom frequency
						friso_dic_add_with_fre( dic, lex,
									__tokens[0],
									__tokens[1], atoi( __tokens[2] ) );
					} else {
						friso_dic_add( dic, lex,
									__tokens[0], __tokens[1] );
					}
				} else {
					FRISO_FREE( __tokens[0] );
				}
			}

			//free the useless allocations.
			for ( t = 2; t < __SPLIT_MAX_TOKENS__; t++ ) {
				if ( __tokens[t] != NULL )
					FRISO_FREE( __tokens[t] );
			}
			
		}
		
		fclose( _stream );
	} 
	
}

__STATIC_API__ int get_lexicon_type_with_constant( string _key ) {
	
	if ( strcmp( _key, "__LEX_CJK_WORDS__" ) == 0 ) {
		return __LEX_CJK_WORDS__;
	} 
	else if ( strcmp( _key, "__LEX_CJK_UNITS__" ) == 0 ) {
		return __LEX_CJK_UNITS__;
	}
	else if ( strcmp( _key, "__LEX_MIX_WORDS__" ) == 0 ) {
		return __LEX_MIX_WORDS__;
	}
	else if ( strcmp( _key, "__LEX_CN_LNAME__" ) == 0 ) {
		return __LEX_CN_LNAME__;
	}
	else if ( strcmp( _key, "__LEX_CN_SNAME__" ) == 0 ) {
		return __LEX_CN_SNAME__;
	}
	else if ( strcmp( _key, "__LEX_CN_DNAME1__" ) == 0 ) {
		return __LEX_CN_DNAME1__;
	}
	else if ( strcmp( _key, "__LEX_CN_DNAME2__" ) == 0 ) {
		return __LEX_CN_DNAME2__;
	}
	else if ( strcmp( _key, "__LEX_CN_LNA__" ) == 0 ) {
		return __LEX_CN_LNA__;
	}

	return -1;
}

/*
 * load the lexicon configuration file.
 *		and load all the valid lexicon from the configuration file.
 */
__EXTERN_API__ void friso_dic_load_from_ifile( friso_dic_t dic, string _path, uint_t _limits  ) {

	//1.parse the configuration file.
	FILE * __stream;
	char __chars__[1024], __key__[30], *__line__;
	uint_t __length__, i, t;
	friso_lex_t lex_t;
	string_buffer_t sb;

	sb = new_string_buffer();
	string_buffer_append( sb, _path );
	string_buffer_append( sb, __FRISO_LEX_IFILE__ );

	if ( ( __stream = fopen( sb->buffer, "rb" ) ) != NULL ) {
		while ( ( __line__ = file_get_line( __chars__, __stream ) ) != NULL ) {
			//comment filter.
			if ( __line__[0] == '#' ) continue;
			if ( __line__[0] == '\0' ) continue;

			__length__ = strlen( __line__ );
			//item start
			if ( __line__[ __length__ - 1 ] == '[' ) {
				//get the type key
				for ( i = 0; i < __length__
						&& ( __line__[i] == ' ' || __line__[i] == '\t' ); i++ ); 
				for ( t = 0; i < __length__; i++,t++ ) {
					if ( __line__[i] == ' ' || __line__[i] == '\t' || __line__[i] == ':' ) break;
					__key__[t] = __line__[i];
				}
				__key__[t] = '\0';

				//get the lexicon type
				lex_t = get_lexicon_type_with_constant(__key__);
				if ( lex_t == -1 ) continue; 
				
				//printf("key=%s, type=%d\n", __key__, lex_t );
				while ( ( __line__ = file_get_line( __chars__, __stream ) ) != NULL ) {
					//comments filter.
					if ( __line__[0] == '#' ) continue;
					if ( __line__[0] == '\0' ) continue; 

					__length__ = strlen( __line__ );
					if ( __line__[ __length__ - 1 ] == ']' ) break;
					
					for ( i = 0; i < __length__ 
							&& ( __line__[i] == ' ' || __line__[i] == '\t' ); i++ );
					for ( t = 0; i < __length__; i++,t++ ) {
						if ( __line__[i] == ' ' || __line__[i] == '\t' || __line__[i] == ';' ) break;
						__key__[t] = __line__[i]; 
					}
					__key__[t] = '\0';

					//load the lexicon item from the lexicon file.
					string_buffer_clear( sb );
					string_buffer_append( sb, _path );
					string_buffer_append( sb, __key__ );
					friso_dic_load( dic, lex_t, sb->buffer, _limits );
				}

			} 


		} //end while

		fclose( __stream );
	}

	free_string_buffer(sb);	
}

//match the item.
__EXTERN_API__ int friso_dic_match( friso_dic_t dic, friso_lex_t lex, string word ) {
	if ( lex >= 0 && lex < __FRISO_LEXICON_LENGTH__ ) {
		return hash_exist_mapping( dic[lex], word );
	}
	return 0;
}

//get the lex_entry_t associated with the word.
__EXTERN_API__ lex_entry_t friso_dic_get( friso_dic_t dic, friso_lex_t lex, string word ) {
	if ( lex >= 0 && lex < __FRISO_LEXICON_LENGTH__ ) {
		return ( lex_entry_t ) hash_get_value( dic[lex], word );
	}
	return NULL;
}

//get the size of the specified type dictionary.
__EXTERN_API__ uint_t friso_spec_dic_size( friso_dic_t dic, friso_lex_t lex ) {
	if ( lex >= 0 && lex < __FRISO_LEXICON_LENGTH__ ) {
		return hash_get_size( dic[lex] );
	}
	return 0;
}

//get size of the whole dictionary.
__EXTERN_API__ uint_t friso_all_dic_size( friso_dic_t dic ) {
	
	register uint_t size = 0, t;
	
	for ( t = 0; t < __FRISO_LEXICON_LENGTH__; t++ ) {
		size += hash_get_size( dic[t] );
	}
	
	return size;
}
