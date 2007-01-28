#include <libpl2.h>

static pl2_outputComplexFunc _chook=NULL;
static pl2_outputFunc _hook=NULL;
static void *_hookData, *_chookData;

static const char _lvlchrs[] = "EWID";


#define CACHE_SIZE 32
static u32 _cacheSize=0, _cachePos=0;
static char _cache[CACHE_SIZE][512];

void pl2_output(u32 level, const char *function, int line, const char *format, ...) {
	char strbuf[512];
	va_list ap;

	va_start(ap, format);
	vsnprintf(strbuf, sizeof(strbuf), format, ap);
	va_end(ap);

	if (_chook!=NULL)
		_chook(level, function, line, strbuf, _chookData);

	snprintf(_cache[_cachePos], sizeof(_cache[_cachePos]), "%c> %s", level>=sizeof(_lvlchrs)?'-':_lvlchrs[level], strbuf);

	if (_hook!=NULL)
		_hook(_cache[_cachePos], _hookData);

	printf("%s", _cache[_cachePos]);


	_cachePos=(_cachePos+1)&(CACHE_SIZE-1);
	_cacheSize++;
	if (_cacheSize>CACHE_SIZE)
		_cacheSize=CACHE_SIZE;
}


void pl2_outputHookComplex(pl2_outputComplexFunc func, void *user) {
	_chook = func;
	_chookData = user;
}

void pl2_outputHook(pl2_outputFunc func, void *user) {
	_hook = func;
	_hookData = user;
}

const char *pl2_outputGet(u32 id) {
	if (id>=_cacheSize)
		return NULL;
	u32 nid = (CACHE_SIZE+_cachePos-_cacheSize+id)&(CACHE_SIZE-1);
	if (nid>=_cacheSize)
		return NULL;

	return _cache[nid];
}



