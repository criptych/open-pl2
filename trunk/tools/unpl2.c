#include <libpl2.h>


int main(int argc, char *argv[]) {
	if (argc<2) {
		output_message("Usage: %s file.pl2\n", argv[0]);
		return 1;
	}

	char *name = argv[1];

	pl2Pak *myPak = pl2Pak_load(name);
	if (myPak==NULL) {
		return 1;
	}

	for (u32 i=0;i<myPak->EntryCount;i++) {
		int size;
		

		void *dat = pl2Pak_unpackID(myPak, i, &size);
		if (dat==NULL) {
			output_error("%d: %s <FAILED>\n", i, myPak->Entries[i].Name);
			continue;
		}	
		FILE *f = fopen(myPak->Entries[i].Name, "wb");
		if (f==NULL) {
			output_error("%s: Failed to open file (%s)\n", myPak->Entries[i].Name, strerror(errno));
			continue;
		}
		fwrite(dat, 1, size, f);
		fclose(f);
	}

	return 0;
}
