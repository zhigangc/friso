/*
 * friso test program.
 *
 * @author	chenxin
 * @email	chenxin619315@gmail.com
 */
#include "friso_API.h"
#include "friso.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * File Explain.
 * 
 * @author	chenxin
 * @see		http://www.webssky.com 
 */

#define __LENGTH__ 15
#define __INPUT_LENGTH__ 1024
#define ___EXIT_INFO___																\
 		println("Thanks for trying friso.");										\
 		break;


/*static void printcode( string str ) {
	int i,length;
	length = strlen( str );
	printf("str:length=%d\n", length );
	for ( i = 0; i < length; i++ ) {
		printf("%d ", str[i] );
	}
	putchar('\n');
}*/

int main(int argc, char **argv) {

	clock_t s_time, e_time;
	char line[__INPUT_LENGTH__] =  "我爱北京天安门";
	int i;
	string __path__ = NULL;

	friso_t friso;
	friso_task_t task;

	//get the lexicon directory
	for ( i = 0; i < argc; i++ ) {
		if ( strcasecmp( "-init", argv[i] ) == 0 ) {
			__path__ = argv[i+1];
		}
	}
	if ( __path__ == NULL ) {
		println("Usage: fmem -init lexicon path");
		exit(0);
	}

	s_time = clock();

	friso = friso_new_from_ifile(__path__);

	e_time = clock();

	printf("friso initialized in %fsec\n", (double) ( e_time - s_time ) / CLOCKS_PER_SEC );
	task = friso_new_task();
		
	i = 0;
	while ( i < 4000000 ) {
		i++;
		//set the task text.
		friso_set_text( task, line );
		println("分词结果:");
		
			s_time = clock();
		while ( ( friso_next( friso, friso->mode, task ) ) != NULL ) {
			//printf("%s[%d,%d]/ ", task->hits->word, task->hits->type, task->hits->offset );
			printf("%s/ ", task->hits->word );
			if ( task->hits->type == __FRISO_NEW_WORDS__ ) {
				FRISO_FREE( task->hits->word );
			}
		}
		e_time = clock();
		printf("\nDone, cost < %fsec\n", ( (double)(e_time - s_time) ) / CLOCKS_PER_SEC );
		
	}
	friso_free_task( task );
	friso_free(friso);
	
	
	return 0;
}
