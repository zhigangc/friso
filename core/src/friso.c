/*
 * friso main file implemented the friso main functions.
 * 		starts with friso_ in the friso header file "friso.h";
 * 
 * @author	chenxin
 * @email	chenxin619315@gmail.com
 */
#include "friso_API.h"
#include "friso.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//create a new friso configuration variable.
__EXTERN_API__ friso_t friso_new( void ) {
	friso_t e = ( friso_t ) FRISO_MALLOC( sizeof( friso_entry ) );
	if ( e == NULL ) {
		___ALLOCATION_ERROR___
	} 

	//initialize the entry
	e->max_len 		= DEFAULT_SEGMENT_LENGTH;
	e->r_name 		= 1;
	e->mix_len 		= DEFAULT_MIX_LENGTH;
	e->lna_len 		= DEFAULT_LNA_LENGTH;
	e->nthreshold 	= DEFAULT_NTHRESHOLD;
	e->mode 		= ( friso_mode_t ) DEFAULT_SEGMENT_MODE;
	e->dic 			= NULL;

	return e;
}

//creat a new friso with initialize item from a configuration file.
__EXTERN_API__ friso_t friso_new_from_ifile( string __ifile ) {
	
	FILE *__stream;
	char __chars__[256], __key__[128], *__line__, __lexi__[128];
	uint_t i, t, __hit__ = 0, __length__;
	friso_t e = friso_new();

	if ( ( __stream = fopen( __ifile, "rb" ) ) != NULL ) {

		//initialize the entry with the value from the ifile.
		while ( ( __line__ = file_get_line( __chars__, __stream ) ) != NULL ) {
			//comments filter.
			if ( __line__[0] == '#' ) continue;
			if ( __line__[0] == '\t' ) continue; 
			if ( __line__[0] == ' ' || __line__[0] == '\0' ) continue;

			__length__ = strlen( __line__ );
			for ( i = 0; i < __length__; i++ ) {
				if ( __line__[i] == ' ' || __line__[i] == '\t' || __line__[i] == '=' ) break;
				__key__[i] = __line__[i];
			}
			__key__[i] = '\0';

			//position the euqals char '='.
			if ( __line__[i] == ' ' || __line__[i] == '\t' ) {
				for ( i++ ; i < __length__; i++ ) 
					if ( __line__[i] == '=' ) break; 
			} 

			//clear the left whitespace of the value.
			for ( i++; i < __length__ 
						&& ( __line__[i] == ' ' || __line__[i] == '\t' ); i++ );
			for ( t = 0; i < __length__; i++, t++ ) {
				if ( __line__[i] == ' ' || __line__[i] == '\t' ) break;
				__line__[t] = __line__[i]; 
			} 
			__line__[t] = '\0';

			//printf("key=%s, value=%s\n", __key__, __line__ );
			if ( strcmp( __key__, "friso.lex_dir" ) == 0 ) {
				/*
				 * here copy the value of the lex_dir.
				 *		cause we need the value of friso.max_len to finish all
				 *	the work when we call function friso_dic_load_from_ifile to
				 *	initiliaze the friso dictionary.
				 */
				if ( __hit__ == 0 ) {
					__hit__ = t;
					for ( t = 0; t < __hit__; t++ ) {
						__lexi__[t] = __line__[t];
					}
					__lexi__[t] = '\0';
				} 
			} else if ( strcmp( __key__, "friso.max_len" ) == 0 ) {
				e->max_len = atoi( __line__ );
			} else if ( strcmp( __key__, "friso.r_name" ) == 0 ) {
				e->r_name = atoi( __line__ );
			} else if ( strcmp( __key__, "friso.mix_len" ) == 0 ) {
				e->mix_len = atoi( __line__ );
			} else if ( strcmp( __key__, "friso.lna_len" ) == 0 ) {
				e->lna_len = atoi( __line__ );
			} else if ( strcmp( __key__, "friso.nthreshold" ) == 0 ) {
				e->nthreshold = atoi( __line__ );
			} else if ( strcmp( __key__, "friso.mode" ) == 0 ) {
				e->mode = ( friso_mode_t ) atoi( __line__ );
			}
		}

		/*
		 * intialize the friso dictionary here.
		 *		use the setting from the ifile parse above.
		 *	we copied the value in the __lexi__.
		 */
		if ( __hit__ != 0 ) {
			e->dic = friso_dic_new();
			friso_dic_load_from_ifile( e->dic, __lexi__, e->max_len * 3 );
		}

		fclose( __stream );
	}

	return e;
}

/*
 * friso free functions.
 * here we have to free its dictionary.
 */
__EXTERN_API__ void friso_free( friso_t friso ) {
	//free the dictionary
	if ( friso->dic != NULL ) {
		friso_dic_free( friso->dic );
	}
	FRISO_FREE( friso );
}

__EXTERN_API__ void friso_set_dic( friso_t friso, friso_dic_t dic ) {
	friso->dic = dic;
}

__EXTERN_API__ void friso_set_mode( friso_t friso, friso_mode_t mode ) {
	friso->mode = mode;
}

//create a new segment item.
__EXTERN_API__ friso_task_t friso_new_task() {
	
	friso_task_t task = ( friso_task_t ) FRISO_MALLOC( sizeof( friso_task_entry ) );
	if ( task == NULL ) {
		___ALLOCATION_ERROR___
	}

	//initliaze the segment.
	task->text 	= NULL;
	task->idx 	= 0;
	task->length = 0;
	task->bytes = 0;
	task->unicode = 0;
	task->poll	= new_link_list();
	task->hits = friso_new_hits();

	return task;
}

__EXTERN_API__ void friso_free_task( friso_task_t task ) {

	//free the allocation of the poll link list.
	if ( task->poll != NULL ) {
		free_link_list( task->poll );
	}

	//free the allocations of the hits.
	if ( task->hits != NULL ) {
		friso_free_hits( task->hits );
	}

	FRISO_FREE( task );
}

//create a new friso hits
__EXTERN_API__ friso_hits_t friso_new_hits( void ) {

	friso_hits_t hits = ( friso_hits_t ) FRISO_MALLOC( sizeof( friso_hits_term ) );
	if ( hits == NULL ) {
		___ALLOCATION_ERROR___
	}

	//initialize
	hits->word = NULL;
	hits->offset = -1;
	hits->type = 0;

	return hits;
}

//free the allocations of the given friso hits
__EXTERN_API__ void friso_free_hits( friso_hits_t hits ) {
	FRISO_FREE( hits );
}


/*
 * set the text of the current segmentation.
 *		that means we could re-use the segment.
 *	also we have to reset the idx and the length of the segmentation.
 		and the most important one - clear the poll link list.
 *		
 */
__EXTERN_API__ void friso_set_text( friso_task_t task, string text ) {
	task->text = text;
	task->idx = 0;										//reset the index
	task->length = strlen( text );
	task->poll = link_list_clear( task->poll );			//clear the word poll
}

/* **************************************************************
 * the static functions:										*
 * 		to assist the friso_next finish the work.				*
 ****************************************************************/
__STATIC_API__ size_t read_next_word( friso_task_t task, uint_t * idx, string __word ) {
	
	register uint_t t;

	if ( *idx >= task->length ) {
		return 0;
	}
	
	task->bytes = get_utf8_bytes( task->text[ *idx ] );

	for ( t = 0; t < task->bytes; t++ ) {
		__word[t] = task->text[ (*idx)++ ];
	}
	__word[t] = '\0';

	return task->bytes;
}

//get the next cjk word from the current position, with simple mode.
__STATIC_API__ friso_hits_t next_simple_cjk( friso_t friso, friso_task_t task ) {

	uint_t t, idx = task->idx, __length__;
	string_buffer_t sb = new_string_buffer_with_string( task->buffer );
	lex_entry_t e = friso_dic_get( friso->dic, __LEX_CJK_WORDS__, sb->buffer );
	/*
	 * here bak the e->length in the task->hits->type.
	 *		we will use it to count the task->idx.
	 * for the sake of use less variable.
	 */
	__length__ = e->length;

	for ( t = 1; 
		t < friso->max_len 
		&& ( task->bytes = read_next_word( task, &idx, task->buffer ) ) != 0; t++ ) {
		
		task->unicode = get_utf8_unicode( task->buffer );

		if ( utf8_whitespace( task->unicode ) ) break;
		if ( ! utf8_cjk_string( task->unicode ) ) break;

		string_buffer_append( sb, task->buffer );

		//check the existence of the word by search the dictionary.
		if ( friso_dic_match( friso->dic, __LEX_CJK_WORDS__, sb->buffer ) ) {
			e = friso_dic_get( friso->dic, __LEX_CJK_WORDS__, sb->buffer );
		}
	}

	//correct the offset of the segment.
	task->idx += ( e->length - __length__ );
	free_string_buffer( sb );							//free the buffer

	//reset the hits.
	task->hits->word = e->word;
	task->hits->type = __FRISO_SYS_WORDS__;

	return task->hits;
}

/* {{{ basic latin segment*/
//basic latin full-width change and upper lower case change quick accessor.
#define ___LATAIN_FULL_UPPER_CHECK___								\
		if ( utf8_fullwidth_letter_digit( task->unicode ) ) {			\
			task->unicode -= 65248;									\
			__convert = 1;											\
			memset( task->buffer, 0, 7 );							\
		}															\
		if ( utf8_uppercase_letter( task->unicode ) ) {				\
			/*reset the buffer*/									\
			task->unicode += 32;									\
			__convert = 1;											\
		}															\
		if ( __convert == 1 ) {										\
			unicode_to_utf8( task->unicode, task->buffer );			\
			__convert = 0;											\
		}															\


//get the next latin word from the current position.
__STATIC_API__ friso_hits_t next_basic_latin( friso_t friso, friso_task_t task ) {

	char __convert = 0, t = 0;
	string_buffer_t sb, temp;
	lex_entry_t e = NULL;

	//full-half width and upper-lower case exchange.
	task->unicode = get_utf8_unicode( task->buffer );
	___LATAIN_FULL_UPPER_CHECK___

	//creat a new string buffer and append the task->buffer insite.
	sb = new_string_buffer_with_string( task->buffer );


	//segmentation.
	while ( ( task->bytes = read_next_word( task, &task->idx, task->buffer ) ) != 0 ) {

		task->unicode = get_utf8_unicode( task->buffer );

		if ( utf8_whitespace( task->unicode ) ) break;
		if ( utf8_en_punctuation( task->unicode ) && ! utf8_keep_punctuation( task->buffer ) ) break; 
		if ( ! ( utf8_halfwidth_letter_digit( task->unicode ) || utf8_fullwidth_letter_digit( task->unicode ) ) ) {
			task->idx -= task->bytes;
			t = 1;
			break;
		}

		//full-half width and upper-lower case convert
		___LATAIN_FULL_UPPER_CHECK___

		//append the word the buffer.
		string_buffer_append( sb, task->buffer );
	}

	/*clear the useless english punctuation from
		the end of the buffer.*/
	for ( ; sb->length > 0 
			&& sb->buffer[ sb->length - 1 ] != '%' 
			&& is_en_punctuation( sb->buffer[ sb->length - 1 ] ); ) {
		sb->buffer[ --sb->length ] = '\0';
	}

	/*
	 * find the chinese or english mixed word.
	 * 		or single chinese units.*/
	if ( t == 1 ) {
		if ( utf8_cjk_string( task->unicode ) ) {
			//temp string buffer.
			temp = new_string_buffer_with_string( sb->buffer );

			for ( t = 0; 
				t < friso->mix_len 
				&& ( task->bytes = read_next_word( task , &task->idx, task->buffer ) ) != 0; t++ ) {
				
				task->unicode = get_utf8_unicode( task->buffer );

				if ( ! utf8_cjk_string( task->unicode ) ) {
					task->idx -= task->bytes;
					break;
				}

				string_buffer_append( temp, task->buffer );

				//check the mixed word dictionary.
				if ( friso_dic_match( friso->dic, __LEX_MIX_WORDS__, temp->buffer ) ) {
					__convert = 1;
					//get the lexicon entry from the dictionary.
					e = friso_dic_get( friso->dic, __LEX_MIX_WORDS__, temp->buffer );
				}
			}

			//correct the segmentation offset.
			task->idx -= ( temp->length - ( e == NULL ? sb->length : e->length ) );
			free_string_buffer( temp );

			//no match for mix word, try to find a single chinese unit.
			if ( __convert == 0 ) {
				//check if it is string made up with numeric
				if ( utf8_numeric_string( sb->buffer ) 
						&& ( task->bytes = read_next_word( task, &task->idx, task->buffer ) ) != 0 ) {

					//check the single chinese units dictionary.
					if ( friso_dic_match( friso->dic, __LEX_CJK_UNITS__, task->buffer ) ) {
						string_buffer_append( sb, task->buffer );
					} else {
						task->idx -= task->bytes;
					}
				}
			}	//end convert condition

		}
	}


	if ( __convert == 1 ) {
		free_string_buffer( sb );
		task->hits->word = e->word;
		task->hits->type = __FRISO_SYS_WORDS__;
	} else {
		/*
		 * adjust the string buffer.
		 *		here we do not trim the buffer cause its allocations will be free
		 *	after the call of friso_next - sooner or later it will be released.
		 *	if your memory almost run out, you should call string_buffer_trim.
		 *	or we save the time to do the allocations and copy the buffer insite.
		 */
		//string_buffer_trim( sb );
		task->hits->word = string_buffer_devote( sb );
		task->hits->type = __FRISO_NEW_WORDS__;
	}

	return task->hits;
}
/* }}} */

/* **************************************************************
 * 	mmseg algorithm implemented functions :: start  			*
 ****************************************************************/

/*
 * get the next match from the current position,
 *		throught the dictionary.
 *	this will return all the matchs.
 *
 * @return friso_array_t that contains all the matchs.
 */
__STATIC_API__ friso_array_t get_next_match( friso_t friso, friso_task_t task, uint_t idx ) {

	register uint_t t;
	string_buffer_t sb = new_string_buffer_with_string( task->buffer );

	//create a match dynamic array.
	friso_array_t match = new_array_list_with_opacity( friso->max_len );
	array_list_add( match, friso_dic_get( friso->dic, __LEX_CJK_WORDS__, task->buffer ) );

	for ( t = 1; t < friso->max_len
			&& ( task->bytes = read_next_word( task, &idx, task->buffer ) ) != 0; t++ ) {

		task->unicode = get_utf8_unicode( task->buffer );
		if ( utf8_whitespace( task->unicode ) ) 	break;
		if ( ! utf8_cjk_string( task->unicode ) ) break;

		//append the task->buffer to the buffer.
		string_buffer_append( sb, task->buffer );

		//check the CJK dictionary.
		if ( friso_dic_match( friso->dic, __LEX_CJK_WORDS__, sb->buffer ) ) {
			/*
			 * add the lex_entry_t insite.
			 * here is a key point:
			 *		we use friso_dic_get function to get the address of the lex_entry_cdt
			 *		that store in the dictionary, not create a new lex_entry_cdt.
			 * so :
			 *		1.we will not bother to the allocations of the newly created lex_entry_cdt.
			 *		2.more efficient of course.
			 */
			array_list_add( match, friso_dic_get( friso->dic, __LEX_CJK_WORDS__, sb->buffer ) );
		}
	}

	/*buffer allocations clear*/
	free_string_buffer( sb );
	//array_list_trim( match );
	
	return match;
}



/*chunk for mmseg defines and functions to handle them.*/
typedef struct {
	friso_array_t words;
	uint_t length;
	float average_word_length;
	float word_length_variance;
	float single_word_dmf;
} friso_chunk_entry;

typedef friso_chunk_entry * friso_chunk_t;

/*create a new chunks*/
__STATIC_API__ friso_chunk_t new_chunk( friso_array_t words, size_t length ) {

	friso_chunk_t chunk = ( friso_chunk_t ) FRISO_MALLOC( sizeof( friso_chunk_entry ) );
	if ( chunk == NULL ) {
		___ALLOCATION_ERROR___
	}

	chunk->words = words;
	chunk->length = length;
	chunk->average_word_length = -1;
	chunk->word_length_variance = -1;
	chunk->single_word_dmf = -1;

	return chunk;
}

//free the given chunk
__STATIC_API__ void free_chunk( friso_chunk_t chunk ) {
	FRISO_FREE( chunk );
}

/*
 * a static function to count the average word length
 *		of the given chunk.
 */
__STATIC_API__ float count_chunk_avl( friso_chunk_t chunk ) {
	chunk->average_word_length = (float) chunk->length / chunk->words->length;
	return chunk->average_word_length;
}

/*
 * a static function to count the word length variance
 *		of the given chunk.
 */
__STATIC_API__ float count_chunk_var( friso_chunk_t chunk ) {

	float __var__ = 0, __tmp__;			//snapshot
	register uint_t t;
	lex_entry_t e;

	for ( t = 0; t < chunk->words->length; t++ ) {
		e = ( lex_entry_t ) chunk->words->items[t];
		__tmp__ = e->length - chunk->average_word_length;
		__var__ += __tmp__ * __tmp__;
	}
	chunk->word_length_variance = __var__ / chunk->words->length;

	return chunk->word_length_variance;
}

/*
 * a static function to count the single word morpheme degree of freedom
 *		of the given chunk.
 */
__STATIC_API__ float count_chunk_mdf( friso_chunk_t chunk ) {

	float __mdf__ = 0;
	register uint_t t;
	lex_entry_t e;

	for ( t = 0; t < chunk->words->length; t++ ) {
		e = ( lex_entry_t ) chunk->words->items[t] ;
		if ( e->length == 3 ) {		//single cjk word
			__mdf__ += e->fre;
		}
	}
	chunk->single_word_dmf = ( float ) log( __mdf__ );

	return chunk->single_word_dmf;
}

/*chunk pinter - use for for debug*/
#define ___CHUNK_PRINTER___( _chunks_ )											\
	for ( t = 0; t < _chunks_->length; t++ ) {									\
		__tmp__ = (( friso_chunk_t ) _chunks_->items[t])->words;				\
		for ( j = 0; j < __tmp__->length; j++ ) {								\
			printf("%s/ ", ( ( lex_entry_t ) __tmp__->items[j] )->word );		\
		}																		\
		putchar('\n');															\
	}																			\
	putchar('\n');																\

/*
 * here,
 * we use four rules to filter all the chunks to get the best chunk.
 *		and this is the core of the mmseg alogrithm.
 * 1. maximum match word length.
 * 2. larget average word length.
 * 3. smallest word length variance.
 * 4. largest single word morpheme degrees of freedom.
 */
__STATIC_API__ friso_chunk_t mmseg_core_invoke( friso_array_t chunks ) {
	
	register uint_t t/*, j*/;
	float max;
	friso_chunk_t e;
	friso_array_t __res__, __tmp__;
	__res__ = new_array_list_with_opacity( chunks->length );


	//1.get the maximum matched chunks.
		//count the maximum length
	max = ( float ) ( ( friso_chunk_t ) chunks->items[0] )->length;
	for ( t = 1; t < chunks->length; t++ ) {
		e = ( friso_chunk_t ) chunks->items[t];
		if ( e->length > max )
			max = ( float ) e->length;
	}
		//get the chunk items that owns the maximum length.
	for ( t = 0; t < chunks->length; t++ ) {
		e = ( friso_chunk_t ) chunks->items[t];
		if ( e->length >= max ) {
			array_list_add( __res__, e );
		} else {
			free_array_list( e->words );
			free_chunk( e );
		}
	}
		//check the left chunks
	if ( __res__->length == 1 ) {
		e = ( friso_chunk_t ) __res__->items[0];
		free_array_list( __res__ );
		free_array_list( chunks );
		return e;
	} else {
		__tmp__ = array_list_clear( chunks );
		chunks = __res__;
		__res__ = __tmp__;
	}


	//2.get the largest average word length chunks.
		//count the maximum average word length.
	max = count_chunk_avl( ( friso_chunk_t ) chunks->items[0] );
	for ( t = 1; t < chunks->length; t++ ) {
		e = ( friso_chunk_t ) chunks->items[t];
		if ( count_chunk_avl( e ) > max ) {
			max = e->average_word_length;
		}
	}
		//get the chunks items that own the largest average word length.
	for ( t = 0; t < chunks->length; t++ ) {
		e = ( friso_chunk_t ) chunks->items[t];
		if ( e->average_word_length >= max ) {
			array_list_add( __res__, e );
		} else {
			free_array_list( e->words );
			free_chunk( e );
		}
	}
		//check the left chunks
	if ( __res__->length == 1 ) {
		e = ( friso_chunk_t ) __res__->items[0];
		free_array_list( chunks );
		free_array_list( __res__ );
		return e;
	} else {
		__tmp__ = array_list_clear( chunks );
		chunks = __res__;
		__res__ = __tmp__;
	}


	//3.get the smallest word length variance chunks
		//count the smallest word length variance
	max = count_chunk_var( ( friso_chunk_t ) chunks->items[0] );
	for ( t = 1; t < chunks->length; t++ ) {
		e = ( friso_chunk_t ) chunks->items[t];
		if ( count_chunk_var( e ) < max ) {
			max = e->word_length_variance;
		}
	}
		//get the chunks that own the smallest word length variance.
	for ( t = 0; t < chunks->length; t++ ) {
		e = ( friso_chunk_t ) chunks->items[t];
		if ( e->word_length_variance <= max ) {
			array_list_add( __res__, e );
		} else {
			free_array_list( e->words );
			free_chunk( e );
		}
	}
		//check the left chunks
	if ( __res__->length == 1 ) {
		e = ( friso_chunk_t ) __res__->items[0];
		free_array_list( chunks );
		free_array_list( __res__ );
		return e;
	} else {
		__tmp__ = array_list_clear( chunks );
		chunks = __res__;
		__res__ = __tmp__;
	}

	//4.get the largest single word morpheme degrees of freedom.
		//count the maximum single word morpheme degreees of freedom
	max = count_chunk_mdf( ( friso_chunk_t ) chunks->items[0] );
	for ( t = 1; t < chunks->length; t++ ) {
		e = ( friso_chunk_t ) chunks->items[t];
		if ( count_chunk_mdf( e ) > max ) {
			max = e->single_word_dmf;
		}
	} 
		//get the chunks that own the largest single word word morpheme degrees of freedom.
	for ( t = 0; t < chunks->length; t++ ) {
		e = ( friso_chunk_t ) chunks->items[t];
		if ( e->single_word_dmf >= max ) {
			array_list_add( __res__, e );
		} else {
			free_array_list( e->words );
			free_chunk( e );
		}
	}

	/*
	 * there is still more than one chunks?
	 *		well, this rarely happen but still got a chance.
	 * here we simple return the first chunk as the final result.
	 */
	for ( t = 1; t < __res__->length; t++ ) {
		e = ( friso_chunk_t ) __res__->items[t];
		free_array_list( e->words );
		free_chunk( e );
	}

	e = ( friso_chunk_t ) __res__->items[0];
	free_array_list( chunks );
	free_array_list( __res__ );

	return e;
}


/*
 * get the next cjk word from the current position, with complex mode.
 *	this is the core of the mmseg chinese word segemetation algorithm.
 *	we use four rules to filter the matched chunks and get the best one
 *		as the final result.
 *
 * @see mmseg_core_invoke( chunks );
 */
__STATIC_API__ friso_hits_t next_complex_cjk( friso_t friso, friso_task_t task ) {

	register uint_t x, y, z;
	/*bakup the task->bytes here*/
	uint_t __idx__ = task->bytes;
	lex_entry_t fe, se, te;
	friso_chunk_t e;
	friso_array_t words, chunks;
	friso_array_t smatch, tmatch, fmatch = get_next_match( friso, task, task->idx );

	/*
	 * here:
	 *		if the length of the fmatch is 1, mean we don't have to
	 *	continue the following work. ( no matter what we get the same result. )
	 */
	if ( fmatch->length == 1 ) {
		task->hits->word =  ( ( lex_entry_t ) fmatch->items[0] )->word;
		task->hits->type = __FRISO_SYS_WORDS__;
		free_array_list( fmatch );
		
		return task->hits;
	}

	chunks = new_array_list();
	task->idx -= __idx__;
	

	for ( x = 0; x < fmatch->length; x++ ) 
	{
		/*get the word and try the second layer match*/
		fe = ( lex_entry_t ) array_list_get( fmatch, x );
		__idx__ = task->idx + fe->length;
		read_next_word( task, &__idx__, task->buffer );

		if ( task->bytes != 0 
				&& utf8_cjk_string( get_utf8_unicode( task->buffer ) ) 
				&& friso_dic_match( friso->dic, __LEX_CJK_WORDS__, task->buffer ) ) {

			//get the next matchs
			smatch = get_next_match( friso, task, __idx__ );
			for ( y = 0; y < smatch->length; y++ ) 
			{
				/*get the word and try the third layer match*/
				se = ( lex_entry_t ) array_list_get( smatch, y );
				__idx__ = task->idx + fe->length + se->length;
				read_next_word( task, &__idx__, task->buffer );

				if ( task->bytes != 0 
						&& utf8_cjk_string( get_utf8_unicode( task->buffer ) )
						&& friso_dic_match( friso->dic, __LEX_CJK_WORDS__, task->buffer ) ) {

					//get the matchs.
					tmatch = get_next_match( friso, task, __idx__ );
					for ( z = 0; z < tmatch->length; z++ ) 
					{
						te = ( lex_entry_t ) array_list_get( tmatch, z );
						words = new_array_list_with_opacity(3);
						array_list_add( words, fe );
						array_list_add( words, se );
						array_list_add( words, te );
						array_list_add( chunks, 
								new_chunk( words, fe->length + se->length + te->length ) );
					}
					free_array_list( tmatch );
				} else {
					words = new_array_list_with_opacity(2);
					array_list_add( words, fe );
					array_list_add( words, se );
					//add the chunk
					array_list_add( chunks,
							new_chunk( words, fe->length + se->length ) );
				}
			}
			free_array_list( smatch );
		} else {
			words = new_array_list_with_opacity(1);
			array_list_add( words, fe );
			array_list_add( chunks, new_chunk( words, fe->length ) );
		}
	}
	free_array_list( fmatch );

	/*
	 * filter the chunks with the four rules of the mmseg algorithm
	 *		and get best chunk as the final result.
	 * @see mmseg_core_invoke( chunks );
	 * @date 2012-12-13
	 */
	if ( chunks->length > 1 ) {
		e = mmseg_core_invoke( chunks );
	} else {
		e = ( friso_chunk_t ) chunks->items[0];
	}
	fe = ( lex_entry_t ) e->words->items[0];
	task->hits->word = fe->word;
	task->hits->type = __FRISO_SYS_WORDS__;
	task->idx += fe->length;						//reset the idx of the task.
	free_chunk( e );

	return task->hits;
}

/* **********************************************************
 * mmseg algorithm implemented functions : end 				*
 ************************************************************/


/*
 * get the next segmentation.
 * 		and also this is the friso enterface function.
 *
 * @param friso.
 * @param segment.
 * @return string.
 */
__EXTERN_API__ friso_hits_t friso_next( friso_t friso, friso_mode_t _mode, friso_task_t task ) {

	lex_entry_t lex = NULL;

	if ( ! link_list_empty( task->poll ) ) {
		/*
		 * load word from the word poll if it is not empty.
		 * 		this will make the next word more convenient and efficient.
		 * 	often synonyms, newly created word will be stored in the poll.
		 */
		lex = ( lex_entry_t ) link_list_remove_first( task->poll );

		task->hits->word = lex->word;
		task->hits->offset = task->idx;
		task->hits->type = 				\
				lex->type ==	__LEX_OTHER_WORDS__  ? __FRISO_NEW_WORDS__ : __FRISO_SYS_WORDS__;

		//free the allocations of the lexicon entry if its type is other words.
		if ( lex->type == __LEX_OTHER_WORDS__ ) {
			free_lex_entry( lex );
		}

		return task->hits;
	}

	while ( task->idx < task->length ) {
		//read the next word from the current position.
		task->bytes = read_next_word( task, &task->idx, task->buffer );
		if ( task->bytes == 0 ) break;

		task->unicode = get_utf8_unicode( task->buffer );
		if ( utf8_whitespace( task->unicode ) ) continue;
		task->hits->offset = task->idx - task->bytes;

		//start the segmentation.
		if ( utf8_cjk_string( task->unicode ) 
						&& friso_dic_match( friso->dic, __LEX_CJK_WORDS__, task->buffer) ) {

			//complex mode.
			if ( _mode == __FRISO_COMPLEX_MODE__ ) {
				return next_complex_cjk( friso, task );
			} else {
				return next_simple_cjk( friso, task );
			}
		} 
		else if ( utf8_halfwidth_letter_digit( task->unicode ) 
						|| utf8_fullwidth_letter_digit( task->unicode ) ) 
		{
			if ( utf8_en_punctuation( task->unicode ) ) continue; 
			//get the next basic latin word.
			return next_basic_latin( friso, task );
		} 
		else if ( utf8_letter_number( task->unicode ) ) {

		} 
		else if ( utf8_other_number( task->unicode ) ) {

		}
	}
	
	return NULL;
}

