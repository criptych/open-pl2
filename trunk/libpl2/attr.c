// XXX: This is a disgusting way of doing things, but i really don't care since we'll usually only have one attr set loaded at a time
#include <libpl2.h>

pl2Attr *pl2Attr_load(void *buf, u32 size) {
	if (buf==NULL || size==0)
		return NULL;
	char *end = (char*)buf+size;
	char *str = (char*)buf;
	int count=0;
	while(str<end) {
		char *wstr = (char*)memchr(str, '\n', end-str);
		if (wstr==NULL)
			break;
		count++;
		str = wstr+1;
	}


	size_t attr_size = sizeof(pl2Attr)+(sizeof(pl2AttrEntry)*count);
	pl2Attr *attr = (pl2Attr*)malloc(attr_size);
	if (attr==NULL)
		return NULL;
	memset(attr, 0, attr_size);

	int cur = 0;

	str = (char*)buf;

#ifdef WITH_ICONV
	iconv_t sjis = iconv_open("UTF-8", "shift-jis");
	if (sjis==(iconv_t)-1)
#endif
		output_message("Not converting shift-jis to UTF-8, expect weird character\n");

	while (str<end) {
		if (cur>=count) {
			output_message("Count(%d) exceeded!\n", count);
			break;
		}
		strskip(str, end);
		if (str==end)
			break;

		if (*str=='\n') {
			str++;
			continue;
		}
		

		char *nstr = (char*)memchr(str, '\n', end-str);
		if (nstr==NULL)
			break;
		*nstr = '\0';

		char *wstr = (char*)memchr(str, '\r', end-str);
		if (wstr!=NULL)
			*wstr = '\0';

		if (*str=='#') {
			str = nstr+1;
			continue;			
		}

		char *estr = (char*)memchr(str, '=', nstr-str);
		if (estr==NULL) {
			str = nstr+1;
			continue;
		}
		*(estr++) = '\0';
		strskip(estr, end);
	
		strncpy(attr->Entries[cur].Name, str, sizeof(attr->Entries[cur].Name));
		strterm(attr->Entries[cur].Name);


		size_t rem = sizeof(attr->Entries[cur].Text)-1;
		#ifdef WITH_ICONV
		char *out = attr->Entries[cur].Text;
		while (strlen(estr)) {
			size_t in = strlen(estr);
			size_t ret = iconv(sjis, &estr, &in, &out, &rem);
			if (ret==-1) {
				*out = '\0';
				break;
			}
			if (ret==0 || in==0)
				break;
		}
		#else
		strncpy(attr->Entries[cur].Text, estr, rem);
		#endif
		strterm(attr->Entries[cur].Text);

		cur++;
		str = nstr+1;
	}

	attr->EntryCount = cur;

#ifdef WITH_ICONV
	iconv_close(sjis);
#endif

	return attr;
}

const char *pl2Attr_get(pl2Attr *attr, const char *name, int id) {
	int cur=0;
	if (attr==NULL)
		return NULL;

	for (u32 ent=0;ent<attr->EntryCount;ent++) {
		if (!strcmp(attr->Entries[ent].Name, name)) {
			if (cur==id)
				return attr->Entries[ent].Text;
			cur++;
		}
	}
	return NULL;
}

void pl2Attr_destroy(pl2Attr *attr) {
	free(attr);
}


