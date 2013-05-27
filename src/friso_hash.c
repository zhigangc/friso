/*
 * friso hash table implements functions
 * 		defined in header file "friso_API.h".
 * 
 * @author	chenxin
 * @email	chenxin619315@gmail.com
 */
#include "friso_API.h"
#include <stdlib.h>
#include <string.h>

//-166411799L
//31 131 1331 13331 133331 ..
//31 131 1313 13131 131313 ..	the best
#define HASH_FACTOR 1313131

/* ************************
 *  mapping function area *
 **************************/
__STATIC_API__ uint_t hash( string str, uint_t length ) {
	//hash code
	uint_t hashcode = 0;
	
	while ( *str != '\0' )
		hashcode = hashcode * HASH_FACTOR + ( *str++ );

	return (hashcode % length);
}

/*test if a integer is a prime.*/
__STATIC_API__ int is_prime( int n ) {
	
	int j;
	if ( n == 2 || n == 3 )
		return 1;
	if ( n == 1 || n % 2 == 0 )
		return 0;
		
	for ( j = 3; j * j < n; j++ )
		if ( n % j == 0 )
			return 0;
		
	return 1;
}

/*get the next prime just after the speicified integer.*/
__STATIC_API__ int next_prime( int n ) {
	if ( n % 2 == 0 )
		n++;
	for ( ; ! is_prime( n ); n = n + 2 ) ;
	
	return n;
}

//string copy, return the pointer of the new string.
//static string string_copy( string _src ) {
	//int bytes = strlen( _src );
	//string _dst = ( string ) FRISO_MALLOC( bytes + 1 );
	//register int t = 0;
	
	//do {
		//_dst[t] = _src[t];
		//t++;
	//} while ( _src[t] != '\0' );
	//_dst[t] = '\0';

	//return _dst;
//}

/* *********************************
 * static hashtable function area. *
 ***********************************/
__STATIC_API__ hash_entry_t new_hash_entry( string key, void * value, hash_entry_t next ) {
	
	hash_entry_t e = ( hash_entry_t ) FRISO_MALLOC( sizeof( friso_hash_entry ) );
	if ( e == NULL ) {
		___ALLOCATION_ERROR___
	}

	//e->_key = string_copy( key );
	e->_key = key;
	e->_val = value;
	e->_next = next;
	
	return e;
}

//create blocks copy of entries.
__STATIC_API__ hash_entry_t * create_hash_entries( uint_t blocks ) {
	
	register uint_t t;
	hash_entry_t *e = ( hash_entry_t * ) FRISO_CALLOC( sizeof( hash_entry_t ), blocks );
	if ( e == NULL ) {
		___ALLOCATION_ERROR___
	}

	for ( t = 0; t < blocks; t++ ) {
		e[t] = NULL;
	}

	return e;
}

//a static function to do the re-hash work.
__STATIC_API__ void rebuild_hash( friso_hash_t _hash ) {
	//printf("rehashed.\n");
	//find the next prime as the length of the hashtable.
	uint_t t, length = next_prime( _hash->length * 2 + 1 );
	hash_entry_t e, next, *_src = _hash->table, \
					*table = create_hash_entries( length );
	uint_t bucket;
	
	//copy the nodes
	for ( t = 0; t < _hash->length; t++ ) {
		e = *( _src + t );
		if ( e != NULL ) {
			do {
				next = e->_next;
				bucket = hash( e->_key, length );
				e->_next = table[bucket];
				table[bucket] = e;
				e = next;
			} while ( e != NULL );
		}
	}
	
	_hash->table = table;
	_hash->length = length;
	_hash->threshold = ( uint_t ) ( _hash->length * _hash->factor );
	
	//free the old hash_entry_t blocks allocations.
	FRISO_FREE( _src );
}

/* ********************************
 * hashtable interface functions. *
 * ********************************/
 
//create a new hash table.
__EXTERN_API__ friso_hash_t new_hash_table( void ) {
	friso_hash_t  _hash = ( friso_hash_t ) FRISO_MALLOC( sizeof ( friso_hash_cdt ) );
	if (  _hash == NULL ) {
		___ALLOCATION_ERROR___
	}

	//initialize the the hashtable
	_hash->length 		= DEFAULT_LENGTH;
	_hash->size			= 0;
	_hash->factor		= DEFAULT_FACTOR;
	_hash->threshold 	= ( uint_t ) ( _hash->length * _hash->factor );
	_hash->table 		= create_hash_entries( _hash->length );
	
	return _hash;
}

__EXTERN_API__ void free_hash_table( friso_hash_t _hash, uint_t fkey ) {
	
	register uint_t j;
	hash_entry_t e, n;

	for ( j = 0; j < _hash->length; j++ ) {
		e = *( _hash->table + j );
		for ( ; e != NULL ; ) {
			n = e->_next;
			if ( fkey == 1 ) FRISO_FREE( e->_key );
			FRISO_FREE( e );
			e = n;
		}
	}

	//free the pointer array block ( 4 * htable->length continuous bytes ).
	FRISO_FREE( _hash->table );
	FRISO_FREE( _hash );
}

 
//put a new mapping insite.
//the value cannot be NULL.
__EXTERN_API__ void hash_put_mapping( friso_hash_t _hash, string key, void * value ) {

	uint_t bucket = ( key == NULL ) ? 0 : hash( key, _hash->length );
	hash_entry_t e = *( _hash->table + bucket );
	
	//check the given key is already exists or not.
	for ( ; e != NULL; e = e->_next ) {
		if ( key == e->_key 
			|| ( key != NULL && e->_key != NULL && strcmp( key, e->_key ) == 0 ) ) {
			e->_val = value;
			return;
		}
	}

	//put a new mapping into the hashtable.
	_hash->table[bucket] = new_hash_entry( key, value, _hash->table[bucket] );
	_hash->size++;

	//check the condition to rebuild the hashtable.
	if ( _hash->size >= _hash->threshold ) 
		rebuild_hash( _hash );
		
}

//check the existence of the mapping associated with the given key.
__EXTERN_API__ int hash_exist_mapping( friso_hash_t _hash, string key ) {
	
	uint_t bucket = ( key == NULL ) ? 0 : hash( key, _hash->length );
	hash_entry_t e;
	
	for ( e = *( _hash->table + bucket ); 
			e != NULL;
			e = e->_next ) {
		if ( key == e->_key
			|| ( key != NULL && e->_key != NULL && strcmp( key, e->_key ) == 0 )) {
			return 1;
		}
	}
	
	return 0;
}

//get the value associated with the given key.
__EXTERN_API__ void *hash_get_value( friso_hash_t _hash, string key ) {
	
	uint_t bucket = ( key == NULL ) ? 0 : hash( key, _hash->length );
	hash_entry_t e;
	
	for ( e = *( _hash->table + bucket ); 
			e != NULL;
			e = e->_next ) {
		if ( key == e->_key
			|| ( key != NULL && e->_key != NULL && strcmp( key, e->_key ) == 0 )) {
			return e->_val;
		}
	}
	
	return NULL;
}

//remove the mapping associated with the given key.
__EXTERN_API__ void hash_remove_mapping( friso_hash_t _hash, string key ) {
	
	uint_t bucket = ( key == NULL ) ? 0 : hash( key, _hash->length );
	hash_entry_t e, prev = NULL;
	hash_entry_t b;
	
	for ( e = *( _hash->table + bucket );
			e != NULL;
			prev = e, e = e->_next ) {
		if ( key == e->_key
			|| ( key != NULL && e->_key != NULL && strcmp( key, e->_key ) == 0 ) ) {
			b = e;
			//the node located at *( htable->table + bucket )
			if ( prev == NULL ) {
				_hash->table[bucket] = e->_next;
			} else {
				prev->_next = e->_next;
			}
			//printf("%s was removed\n", b->_key);
			_hash->size--;
			FRISO_FREE( b );
			break;
		}
	}
	
}

//count the size of the specified hashtable.
__EXTERN_API__ uint_t hash_get_size( friso_hash_t _hash ) {
	return _hash->size;
}
