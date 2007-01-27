#include <libpl2.h>

// TODO Change for windows 
#define DIRECTORY_SEPARATOR '/'
#define EXTENSION_SEPARATOR '.'

int strfilename(char *dst, int sz, const char *src) {
	if (dst==NULL || src==NULL)
		return -1;

	*dst = '\0';
	if (*src=='\0')
		return -1;

	const char *filename = strrchr(src, DIRECTORY_SEPARATOR);
	if (filename==NULL)
		filename=src;

	if (*filename=='\0')
		return -1;

	while (*filename==DIRECTORY_SEPARATOR)
		filename++;

	const char *end = strrchr(filename, EXTENSION_SEPARATOR);

	int len;
	if (end==NULL)
		len = strlen(filename);
	else
		len = end-filename;

	if (len>=sz)
		len = sz-1;

	strncpy(dst, filename, len);
	dst[len] = '\0';

	return len;
}

int strdirectory(char *dst, int sz, const char *src) {
	if (dst==NULL || src==NULL)
		return -1;

	*dst = '\0';
	if (*src=='\0')
		return -1;

	const char *end = strrchr(src, DIRECTORY_SEPARATOR);
	if (end==NULL)
		return -1;

	int len = end-src;
	if (len>=sz)
		len = sz-1;
	len++;

	strncpy(dst, src, len);
	dst[len] = '\0';

	return len;
}



int strendcmp(const char *big, const char *small) {
	if (strlen(big)<strlen(small))
		return 1;
	
	int idx = strlen(big)-strlen(small);
	return strcmp(big+idx, small);
}

int strendcasecmp(const char *big, const char *small) {
	if (strlen(big)<strlen(small))
		return 1;
	
	int idx = strlen(big)-strlen(small);
	return strcasecmp(big+idx, small);
}

int dirsweep_recursive(const char *name, void *user) {
	output_error("I should never be called!\n");
	return -1;
}

int dirsweep(const char *name, int (*f_file)(const char *name, void *user), int (*f_dir)(const char *name, void *user), void *user) {
	DIR *dir = opendir(name);
	char buf[4096];
	u32 count=0;
	if (dir==NULL)
		return 0;

	while (1) {
		struct dirent *dire = readdir(dir);
		if (dire==NULL)
			break;
		if (dire->d_name[0]=='.')
			continue;

		snprintf(buf, sizeof(buf), "%s/%s", name, dire->d_name);
		struct stat s;
		if (stat(buf, &s)<0)
			continue;

		int ret = 0;

		if (S_ISREG(s.st_mode) && f_file!=NULL)
			ret = f_file(buf, user);
		else if (S_ISDIR(s.st_mode) && f_dir!=NULL) {
			if (f_dir==dirsweep_recursive)
				ret = dirsweep(buf, f_file, f_dir, user);
			else
				ret = f_dir(buf, user);
		}
		if (ret<0)
			return ret;

		count += ret;
	}

	closedir(dir);
	return count;
}


