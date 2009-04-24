#ifndef OTHER_GETOPT_H_
#define OTHER_GETOPT_H_

struct GetOptContext
{
	char* optarg;
	int   optind;
	int   optopt;
	int   opterr;
	int   optreset;
};

void init_getopt_context( GetOptContext *ctx );

int getopt( int argc, char * const argv[], const char *ostr, GetOptContext *ctx );

#endif /* OTHER_GETOPT_H_ */

