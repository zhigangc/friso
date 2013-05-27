/*
 * main interface file for friso - free soul.
 * 		you could modify it and re-release it but never for commercial use.
 * 
 * @author	chenxin
 * @email	chenxin619315@gmail.com 
 */

#ifndef _friso_api_h
#define _friso_api_h

#include <stdio.h>
#include <stdlib.h>

/*platform shared library statement :: unix*/
#define __EXTERN_API__ extern
//#define __EXTERN_API__ extern __declspec(dllexport)
#define __STATIC_API__ static inline


#define ___ALLOCATION_ERROR___ 												\
	printf("Unable to do the memory allocation, program will now exit\n" );	\
	exit(1);

#define print(str) printf("%s", str )
#define println(str) printf("%s\n", str )

/*
 * memory allocation macro definition.
 * 		cause we should use emalloc,ecalloc .ege. in php.
 * so you could make it better apdat the php environment.
 */
#define FRISO_CALLOC(_bytes, _blocks) 	calloc(_bytes, _blocks)
#define FRISO_MALLOC(_bytes) 			malloc(_bytes)
#define FRISO_FREE( _ptr )				free( _ptr )

typedef unsigned short ushort_t;
typedef unsigned char uchar_t;
typedef unsigned int uint_t;
typedef char * string;





/* {{{ string handle interface define::start. */
#define __CHAR_BYTES__ 8
#define __BUFFER_DEFAULT_LENGTH__ 16

typedef struct {
	string buffer;
	size_t length;
	size_t allocs;
} string_buffer_entry;

typedef string_buffer_entry * string_buffer_t;

__EXTERN_API__ string_buffer_t new_string_buffer( void );
__EXTERN_API__ string_buffer_t new_string_buffer_with_opacity( size_t );
__EXTERN_API__ string_buffer_t new_string_buffer_with_string( string str );

/*
 * this function will copy the chars that the string pointed.
 *		to the buffer.
 * this may cause the resize action of the buffer.
 */
__EXTERN_API__ void string_buffer_append( string_buffer_t, string );

//insert the given string from the specified position.
__EXTERN_API__ void string_buffer_insert( string_buffer_t, size_t idx, string );

//remove the char in the specified position.
__EXTERN_API__ string string_buffer_remove( string_buffer_t, size_t idx, size_t );

/*
 * turn the string_buffer to a string.
 *		or return the buffer of the string_buffer.
 */
__EXTERN_API__ string_buffer_t string_buffer_trim( string_buffer_t );

/*
 * free the given string buffer.
 *		and this function will not free the allocations of the 
 *		the string_buffer_t->buffer, we return it to you, if there is
 * 	a necessary you could free it youself by calling free();
 */
__EXTERN_API__ string string_buffer_devote( string_buffer_t );

/*
 * clear the given string buffer.
 *		reset its buffer with 0 and reset its length to 0.
 */
__EXTERN_API__ void string_buffer_clear( string_buffer_t );

//free the string buffer include the buffer.
__EXTERN_API__ void free_string_buffer( string_buffer_t );

/* **********************************************
 *	utf-8 handle functions.						*
 ************************************************/

//print the given integer in a binary style.
__EXTERN_API__ void print_char_binary( char );

//get the bytes of a utf-8 char.
__EXTERN_API__ int get_utf8_bytes( char );

//return the unicode serial number of a given string.
__EXTERN_API__ int get_utf8_unicode( const string );

//turn the unicode serial to a utf-8 string.
__EXTERN_API__ int unicode_to_utf8( size_t, string );

//check if the given char is a CJK.
__EXTERN_API__ int utf8_cjk_string( size_t ) ;

/*check the given char is a Basic Latin letter or not.
 * 		include all the letters and english puntuations.*/
__EXTERN_API__ int utf8_halfwidth_letter_digit( size_t );

/*
 * check the given char is a full-width latain or not.
 *		include the full-width arabic numeber, letters.
 *		but not the full-width puntuations.
 */
__EXTERN_API__ int utf8_fullwidth_letter_digit( size_t );

//check the given char is a upper case char or not.
__EXTERN_API__ int utf8_uppercase_letter( size_t );

//check the given char is a lower case char or not.
__EXTERN_API__ int utf8_lowercase_letter( size_t );

//check the given char is a numeric
__EXTERN_API__ int utf8_numeric_letter( size_t );

/*
 * check if the given string is make up with numeric chars.
 		both full-width,half-width numeric is ok.
 */
__EXTERN_API__ int utf8_numeric_string( const string );

//check the given char is a whitespace or not.
__EXTERN_API__ int utf8_whitespace( size_t );

/* check if the given char is a letter number 
 *		like 'ⅠⅡ'
 */
__EXTERN_API__ int utf8_letter_number( size_t );

/*
 * check if the given char is a other number
 *		like '①⑩⑽㈩'
 */
__EXTERN_API__ int utf8_other_number( size_t );

//check the given char is a english punctuation or not.
__EXTERN_API__ int utf8_en_punctuation( size_t ) ;
__EXTERN_API__ int is_en_punctuation( char );
__EXTERN_API__ int utf8_keep_punctuation( string );

//check the given english char is a full-width char or not.
__EXTERN_API__ int utf8_fullwidth_char( size_t ) ;
/* }}} string interface define::end*/




/* {{{ dynamaic array interface define::start*/
#define __DEFAULT_ARRAY_LIST_OPACITY__ 10

/*friso array list entry struct*/
typedef struct {
	void **items;
	size_t allocs;
	size_t length;
} friso_array_entry;

typedef friso_array_entry * friso_array_t;

//create a new friso dynamic array.
__EXTERN_API__ friso_array_t new_array_list( void );
//create a new friso dynamic array with the given opacity
__EXTERN_API__ friso_array_t new_array_list_with_opacity( size_t );

/*
 * free the given friso array.
 *	include the friso_array_entry's allocations
 *		and all the allocations that each one of the pointer array pointed.
 */
__EXTERN_API__ void free_array_list( friso_array_t );

//add a new item to the array.
__EXTERN_API__ void array_list_add( friso_array_t, void * );

//insert a new item at a specifed position.
__EXTERN_API__ void array_list_insert( friso_array_t, size_t, void * );

//get a item at a specified position.
__EXTERN_API__ void *array_list_get( friso_array_t, size_t );

/*
 * set the item at a specified position.
 * 		this will return the old value.
 */
__EXTERN_API__ void *array_list_set( friso_array_t, size_t, void * );

/*
 * remove the given item at a specified position.
 *		this will return the value of the removed item.
 */
__EXTERN_API__ void *array_list_remove( friso_array_t, size_t );

/*trim the array list for final use.*/
__EXTERN_API__ friso_array_t array_list_trim( friso_array_t );

/*
 * clear the array list.
 * 	this function will free all the allocations that the pointer pointed.
 *		but will not free the point array allocations,
 *		and will reset the length of it.
 */
__EXTERN_API__ friso_array_t array_list_clear( friso_array_t );

//return the size of the array.
__EXTERN_API__ size_t array_list_size( friso_array_t );

//return the allocations of the array.
__EXTERN_API__ size_t array_list_allocs( friso_array_t );

//check if the array is empty.
__EXTERN_API__ int array_list_empty( friso_array_t );
/* }}} dynamaic array interface define::end*/




/* {{{ link list interface define::start*/
struct friso_link_node {
	void * value;
	struct friso_link_node *prev;
	struct friso_link_node *next;
};
typedef struct friso_link_node link_node_entry;
typedef link_node_entry * link_node_t;

/*
 * link list adt
 */
typedef struct {
	link_node_t head;
	link_node_t tail;
	uint_t size;
} friso_link_entry;

typedef friso_link_entry * friso_link_t;

//create a new link list
__EXTERN_API__ friso_link_t new_link_list( void );

//free the specified link list
__EXTERN_API__ void free_link_list( friso_link_t );

//return the size of the current link list.
__EXTERN_API__ uint_t link_list_size( friso_link_t );

//check the given link is empty or not.
__EXTERN_API__ int link_list_empty( friso_link_t );

//clear all the nodes in the link list( except the head and the tail ).
__EXTERN_API__ friso_link_t link_list_clear( friso_link_t link );

//add a new node to the link list.(append from the tail)
__EXTERN_API__ void link_list_add( friso_link_t, void * );

//add a new node before the specified node
__EXTERN_API__ void link_list_insert_before( friso_link_t, uint_t, void * );

//get the node in the current index.
__EXTERN_API__ void *link_list_get( friso_link_t, uint_t );

//modify the node in the current index.
__EXTERN_API__ void *link_list_set( friso_link_t, uint_t, void * );

//remove the specified link node
__EXTERN_API__ void *link_list_remove( friso_link_t, uint_t );

//remove the given node
__EXTERN_API__ void *link_list_remove_node( friso_link_t, link_node_t );

//remove the node from the frist.
__EXTERN_API__ void *link_list_remove_first( friso_link_t );

//remove the last node from the link list
__EXTERN_API__ void *link_list_remove_last( friso_link_t );

//append a node from the end.
__EXTERN_API__ void link_list_add_last( friso_link_t, void * );

//add a node at the begining of the link list.
__EXTERN_API__ void link_list_add_first( friso_link_t, void * );
/* }}} link list interface define::end*/




/* {{{ hashtable interface define :: start*/
struct hash_entry {
	string _key;					//the node key
	void * _val;					//the node value
	struct hash_entry * _next;
};
typedef struct hash_entry friso_hash_entry;
typedef friso_hash_entry * hash_entry_t;


typedef struct {
	uint_t length;
	uint_t size;
	float factor;
	uint_t threshold;
	hash_entry_t *table;
} friso_hash_cdt;

typedef friso_hash_cdt * friso_hash_t;

//default value for friso_hash_cdt
#define DEFAULT_LENGTH 	31
#define DEFAULT_FACTOR	0.85f

/*
 * Function: new_hash_table
 * Usage: table = new_hash_table();
 * -------------------------------- 
 * this function allocates a new symbol table with no entries.
 */
__EXTERN_API__ friso_hash_t new_hash_table( void );

/*
 * Function: free_hash_table
 * Usage: free_hash_table( table, bool );
 * --------------------------------------
 * this function will free all the allocation for memory.
 */
__EXTERN_API__ void free_hash_table( friso_hash_t, uint_t );

/*
 * Function: put_new_mapping
 * Usage: put_mapping( table, key, value );
 * ----------------------------------------
 * the function associates the specified key with the given value.
 */
__EXTERN_API__ void hash_put_mapping( friso_hash_t, string, void * );

/*
 * Function: is_mapping_exists
 * Usage: bool = is_mapping_exists( table, key );
 * ----------------------------------------------
 * this function check the given key mapping is exists or not.
 */
__EXTERN_API__ int hash_exist_mapping( friso_hash_t, string );

/*
 * Function: get_mapping_value
 * Usage: value = get_mapping_value( table, key );
 * -----------------------------------------------
 * this function return the value associated with the given key.
 * 		UNDEFINED will be return if the mapping is not exists.
 */
__EXTERN_API__ void *hash_get_value( friso_hash_t, string );

/*
 * Function: remove_mapping
 * Usage: remove_mapping( table, key );
 * ------------------------------------
 * This function is used to remove the mapping associated with the given key.
 */
__EXTERN_API__ void hash_remove_mapping( friso_hash_t, string );

/*
 * Function: get_table_size
 * Usage: size = get_table_size( table );
 * --------------------------------------
 * This function is used to count the size of the specified table. 
 */
__EXTERN_API__ uint_t hash_get_size( friso_hash_t );
/* }}} hashtable interface define :: end*/




#endif /*end ifndef*/
