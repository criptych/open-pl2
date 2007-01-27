#include <libpl2.h>


int main(int argc, char *argv[]) {
	if (argc<2) {
		output_message("Usage: %s file.pl2\n", argv[0]);
		return 1;
	}

	char *name = argv[1];

	pl2TMB *myTMB = pl2TMB_load(name);
	if (myTMB==NULL) {
		return 1;
	}

	return 0;
}

