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

#define ___ABOUT___																	\
 		println("+-----------------------------------------------------------+");	\
 		println("| friso - a chinese word segmentation writen by c.          |");	\
 		println("| bug report email - chenxin619315@gmail.com.               |");	\
 		println("| or: visit http://code.google.com/p/friso.                 |");	\
 		println("|     java edition for http://code.google.com/p/jcseg       |");	\
 		println("| type 'quit' to exit the program.                          |");	\
 		println("+-----------------------------------------------------------+");

//read a line from a command line.
static string getLine( FILE *fp, string __dst ) {
	register int c;
	register string cs;

	cs = __dst;
	while ( ( c = getc( fp ) ) != EOF ) {
		if ( c == '\n' ) break;
		*cs++ = c; 
	}
	*cs = '\0';

	return ( c == EOF && cs == __dst ) ? NULL : __dst;
}

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
	char line[__INPUT_LENGTH__];
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
		println("Usage: friso -init lexicon path");
		exit(0);
	}

	s_time = clock();
	
	//initialize
/*	friso_t friso = friso_new();
	friso_dic_t dic = friso_dic_new();

	friso_dic_load_from_conf( dic, __path__, __LENGTH__ );
	friso_set_dic( friso, dic );
	friso_set_mode( friso, __FRISO_COMPLEX_MODE__ );*/
	friso = friso_new_from_ifile(__path__);

	e_time = clock();

	printf("friso initialized in %fsec\n", (double) ( e_time - s_time ) / CLOCKS_PER_SEC );
	___ABOUT___

	//set the task.
	task = friso_new_task();

	while ( 1 ) {
		print("friso>> ");
		getLine( stdin, line );
		//exit the programe
		if ( strcasecmp( line, "quit" ) == 0 ) {
			___EXIT_INFO___
		}

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
	
	return 0;
}
