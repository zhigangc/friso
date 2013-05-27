/*
 * main interface file for friso - free soul.
 * 		you could modify it and re-release it but never for commercial use.
 * 
 * @author	chenxin
 * @email	chenxin619315@gmail.com 
 */
#ifndef _friso_h
#define _friso_h

#include "friso_API.h"
#include <stdio.h>

/* {{{ lexicon interface define :: start*/

/*
 * Type: friso_lex_t
 * -----------
 * This type used to represent the type of the lexicon. 
 */
typedef enum {
	__LEX_CJK_WORDS__ = 0,
	__LEX_CJK_UNITS__ = 1,
	__LEX_MIX_WORDS__ = 2,
	__LEX_CN_LNAME__ = 3,
	__LEX_CN_SNAME__ = 4,
	__LEX_CN_DNAME1__ = 5,
	__LEX_CN_DNAME2__ = 6,
	__LEX_CN_LNA__ = 7,
	__LEX_OTHER_WORDS__ = 8
} friso_lex_t;

typedef friso_hash_t * friso_dic_t;
#define __FRISO_LEXICON_LENGTH__ 8

/*
 * Type: lex_entry_cdt
 * -------------------
 * This type used to represent the lexicon entry struct. 
 */
typedef struct {
	string word;
	string syn;
	uint_t fre;
	/*
	 * the type of the lexicon item.
	 * available value is all the elements in friso_lex_t enum.
	 *	and if it is __LEX_OTHER_WORDS__, we need to free it after use it.
	 *	here use char as its data type not int.
	 */
	char length;
	char type;					
} lex_entry_cdt;

typedef lex_entry_cdt * lex_entry_t;

/*
 * Function: friso_dic_new
 * Usage: dic = friso_new_dic();
 * -----------------------------
 * This function used to create a new dictionary.(memory allocation).
 */
__EXTERN_API__ friso_dic_t friso_dic_new( void );

__EXTERN_API__ string file_get_line( string, FILE * );

/*
 * Function: friso_dic_free
 * Usage: friso_dic_free( void );
 * ------------------------------
 * This function is used to free all the allocation of friso_dic_new. 
 */
__EXTERN_API__ void friso_dic_free( friso_dic_t );

//create a new lexicon entry.
__EXTERN_API__ lex_entry_t new_lex_entry( string, string, uint_t, char, char );
//free the given lexicon entry.
__EXTERN_API__ void free_lex_entry( lex_entry_t );

/*
 * Function: friso_dic_load
 * Usage: friso_dic_load( dic, friso_lex_t, path, length ); 
 * --------------------------------------------------
 * This function is used to load dictionary from a given path.
 * 		no length limit when length less than 0.
 */
__EXTERN_API__ void friso_dic_load( friso_dic_t, friso_lex_t, string, uint_t );

/*
 * load the lexicon configuration file.
 *	and load all the valid lexicon from the conf file.
 */
__EXTERN_API__ void friso_dic_load_from_ifile( friso_dic_t, string, size_t );

/*
 * Function: friso_dic_match
 * Usage: friso_dic_add( dic, friso_lex_t, word, syn );
 * ----------------------------------------------
 * This function used to put new word into the dictionary.
 */
__EXTERN_API__ void friso_dic_add( friso_dic_t, friso_lex_t, string, string );

/*
 * Function: friso_dic_add_with_fre
 * Usage: friso_dic_add_with_fre( dic, friso_lex_t, word, value, syn, fre );
 * -------------------------------------------------------------------
 * This function used to put new word width frequency into the dictionary.
 */
__EXTERN_API__ void friso_dic_add_with_fre( friso_dic_t, friso_lex_t, string, string, uint_t );

/*
 * Function: friso_dic_match
 * Usage: result = friso_dic_match( dic, friso_lex_t, word );
 * ----------------------------------------------------
 * This function is used to check the given word is in the dictionary or not. 
 */
__EXTERN_API__ int friso_dic_match( friso_dic_t, friso_lex_t, string );

/*
 * Function: friso_dic_get
 * Usage: friso_dic_get( dic, friso_lex_t, word );
 * -----------------------------------------
 * This function is used to search the specified lex_entry_t.
 */
__EXTERN_API__ lex_entry_t friso_dic_get( friso_dic_t, friso_lex_t, string );

/*
 * Function: friso_spec_dic_size
 * Usage: friso_spec_dic_size( dic, friso_lex_t )
 * This function is used to get the size of the dictionary with a specified type. 
 */
__EXTERN_API__ uint_t friso_spec_dic_size( friso_dic_t, friso_lex_t );
__EXTERN_API__ uint_t friso_all_dic_size( friso_dic_t );
/* }}} lexicon interface define :: end*/




/* {{{ friso main interface define :: start*/
#define DEFAULT_SEGMENT_LENGTH 	5
#define DEFAULT_MIX_LENGTH 		2
#define DEFAULT_LNA_LENGTH 		1
#define DEFAULT_NTHRESHOLD 		1000000
#define DEFAULT_SEGMENT_MODE 	2

/*
 * Type: friso_mode_t
 * ------------------
 * use to identidy the mode that the friso use. 
 */
typedef enum {
	__FRISO_SIMPLE_MODE__ 	= 1,
	__FRISO_COMPLEX_MODE__ 	= 2
} friso_mode_t;

/*
 * Type: friso_vars_entry
 * -----------------
 * This type is used to set the configuration of friso. 
 */
typedef struct {
	ushort_t max_len;				//the max match length (4 - 7).
	ushort_t r_name;				//1 for open chinese name recognition 0 for close it.
	ushort_t mix_len;				//the max length for the CJK words in a mix string.
	ushort_t lna_len;				//the max length for the chinese last name adron.
	uint_t nthreshold;				//the threshold value for a char to make up a chinese name.
	friso_mode_t mode;				//Complex mode or simple mode
	friso_dic_t dic;				//friso dictionary
} friso_entry;

typedef friso_entry * friso_t;


//the type of the returned friso_task_t.
typedef enum {
	__FRISO_SYS_WORDS__ = 0,
	__FRISO_NEW_WORDS__ = 1
} hits_word_t;

/*the segmentation term*/
typedef struct {
	string word;
	int offset;
	hits_word_t type;
} friso_hits_term;

typedef friso_hits_term * friso_hits_t;

/*
 * Type: friso_segment
 * This type used to represent the current segmentation content.
 * 		like the text to split, and the current index. 
 */
typedef struct {
	string text;
	uint_t idx;
	uint_t length;
	uint_t bytes;
	uint_t unicode;
	friso_link_t poll;
	char buffer[7];
	friso_hits_t hits;
} friso_task_entry;

typedef friso_task_entry * friso_task_t;


/*
 * Function: friso_new;
 * Usage: vars = friso_new( void );
 * --------------------------------
 * This function used to create a new empty friso friso_t; 
 *		with default value.
 */
__EXTERN_API__ friso_t friso_new( void );

//creat a friso entry with a default value from a configuratile file.
__EXTERN_API__ friso_t friso_new_from_ifile( string );

/*
 * Function: friso_free_vars;
 * Usage: friso_free( vars );
 * --------------------------
 * This function is used to free the allocation of the given vars. 
 */
__EXTERN_API__ void friso_free( friso_t );

/*
 * Function: friso_set_dic
 * Usage: dic = friso_set_dic( vars, dic );
 * ----------------------------------------
 * This function is used to set the dictionary for friso. 
 * 		and firso_dic_t is the pointer of a hash table array.
 */
__EXTERN_API__ void friso_set_dic( friso_t, friso_dic_t );

/*
 * Function: friso_set_mode
 * Usage: friso_set_mode( vars, mode );
 * ------------------------------------
 * This function is used to set the mode(complex or simple) that you want to friso to use.
 */
__EXTERN_API__ void friso_set_mode( friso_t, friso_mode_t );

/*
 * Function: friso_new_task;
 * Usage: segment = friso_new_task( void );
 * ----------------------------------------
 * This function is used to create a new friso segment type; 
 */
__EXTERN_API__ friso_task_t friso_new_task( void );

/*
 * Function: friso_free_task;
 * Usage: friso_free_task( task ); 
 * -------------------------------
 * This function is used to free the allocation of function friso_new_segment();
 */
__EXTERN_API__ void friso_free_task( friso_task_t );

//create a new friso hits
__EXTERN_API__ friso_hits_t friso_new_hits( void );

//free the given friso hits
__EXTERN_API__ void friso_free_hits( friso_hits_t );

/*
 * Function: friso_set_text
 * Usage: friso_set_text( task, text );
 * ------------------------------------
 * This function is used to set the text that is going to segment. 
 */
__EXTERN_API__ void friso_set_text( friso_task_t, string );

/*
 * Function: friso_next
 * Usage: word = friso_next( vars, seg );
 * --------------------------------------
 * This function is used to get next word that friso segmented. 
 */
__EXTERN_API__ friso_hits_t friso_next( friso_t, friso_mode_t, friso_task_t );
/* }}} friso main interface define :: end*/




#endif /*end ifndef*/
