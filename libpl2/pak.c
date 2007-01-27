#include <libpl2.h>

pl2Pak *pl2Pak_load(const char *file) {
	FILE *f = fopen(file, "rb");
	if (f==NULL) {
		output_error("#%s# Failed to open (%s)\n", file, strerror(errno));
		return 0;
	}
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);

	void *data = malloc(size);
	if (data==NULL) {
		output_error("#%s# Failed to allocate %d bytes of data (%s)\n", file, size, strerror(errno));
		fclose(f);
		return 0;
	}

	if (fread(data, 1, size, f)!=size) {
		output_error("#%s# Failed to read %d bytes of data (%s)\n", file, size, strerror(errno));
		free(data);
		fclose(f);
		return 0;
	}
	fclose(f);

	pl2Pak *pak = pl2Pak_create(data, size, file);
	if (pak==NULL) {
		free(data);
		return NULL;
	}

	pak->Flags |= 1; // Say 'data' should be freed

	return pak;
}

pl2Pak *pl2Pak_create(void *buf, u32 size, const char *name) {
	u8 *cur = (u8*)buf;

	for (u32 i=0;i<4;i++) {
		u32 magic = *(u32*)cur;
		if (magic)
			output_warning("#%s# Magic%d is %08x\n", name, i, magic);
		cur+=4;
	}

	pl2Pak *pak = malloc(sizeof(pl2Pak));
	if (pak==NULL) {
		output_error("#%s# Failed to allocate helper (%s)\n", name, strerror(errno));
		return NULL;
	}
	strfilename(pak->Name, sizeof(pak->Name), name);
	pak->Data = buf;
	pak->Size = size;
	pak->Flags = 0;
	pak->EntryCount = 0;
	pak->Entries = (pl2PakEntry*)cur;

	u32 min = 0xffffffff;
	while(((uptr)cur-(uptr)buf)<min) {
		u32 id = pak->EntryCount;
		u32 end = pak->Entries[id].Offset + pak->Entries[id].PackedSize;

		pak->Entries[id].Name[31] = '\0';

		if (end > pak->Size) {
			output_error("#%s:%s# Data overruns pak file! (%d > %d)\n", pak->Name, pak->Entries[id].Name, end, pak->Size);
			free(pak);
			return NULL;
		}

		if (pak->Entries[id].Offset<min)
			min = pak->Entries[id].Offset;

		pak->EntryCount++;
		cur += sizeof(pl2PakEntry);
	}
	return pak;
}

void *pl2Pak_unpack(pl2Pak *pak, const char *name, u32 *size) {
	for (u32 id=0;id<pak->EntryCount;id++) {
		if (!strcmp(name, pak->Entries[id].Name))
			return pl2Pak_unpackID(pak, id, size);
	}
	output_warning("#%s:%s# Can't find resource\n", pak->Name, name);
	return NULL;
}

void *pl2Pak_unpackID(pl2Pak *pak, u32 id, u32 *size) {
	int currupt = 0;
	u8  *inData, *outData;
	u8  *inCur,  *outCur;
	u32  inSize,  outSize;

	if (size!=NULL)
		*size = 0;

	if (id>=pak->EntryCount) {
		output_error("#%s:%d# ID out of range! (%d)\n", pak->Name, id, pak->EntryCount);
		return NULL;
	}

	inSize = pak->Entries[id].PackedSize; 
	inCur = inData = pak->Entries[id].Offset + pak->Data;
	if ((inData+inSize) > (pak->Data+pak->Size)) {
		output_error("#%s:%s# Data overruns pak file!\n", pak->Name, pak->Entries[id].Name);
		return NULL;
	}

	outSize = pak->Entries[id].Size;
	outCur = outData = malloc(outSize);
	if (outData==NULL) {
		output_error("#%s:%s# Failed to allocate output memory (%d)\n", pak->Name, pak->Entries[id].Name, outSize);
		return NULL;
	}

	while (inCur<(inData+inSize)) {
		int exit=0;
		u8 flag = *(inCur++);

		for (u32 curByte=0;curByte<8;curByte++) {
			if ((inCur-inData)>=inSize) {
				exit=1;
				break;
			}

			if ((outCur-outData)>=outSize){
				output_error("#%s:%s# output buffer overrun\n", pak->Name, pak->Entries[id].Name);
				currupt=1;
				exit=1;
				break;
			}

			if (flag&(1<<curByte)) {
				*(outCur++) = *(inCur++);
				continue;
			}

			if (inSize-(inCur-inData)<2){
				output_error("#%s:%s# input buffer overrun\n", pak->Name, pak->Entries[id].Name);
				currupt=1;
				exit=1;
				break;
			}
	

			//TODO: Decompression routine
			currupt = 1; // XXX: We don't know how to do this properly yet
			u32 offset=(inCur[0])|(inCur[1]&0xf0)<<4;
			u32 length=(inCur[1]&0xf)+3;
			inCur+=2;

			if (outSize-(outCur-outData)<length){
				output_error("#%s:%s# output buffer overrun\n", pak->Name, pak->Entries[id].Name);
				currupt=1;
				exit=1;
				break;
			}


			for (int i=0;i<length;i++) {
				*(outCur++) = 0;
			}

		}
		if (exit)
		break;
	}
	
	if (currupt)
		output_error("#%s:%s# Don't know how to decompress yet!\n", pak->Name, pak->Entries[id].Name);

	if ((outCur-outData)<outSize) {
		currupt = 1;
		output_error("#%s:%s# %d output bytes short!\n", pak->Name, pak->Entries[id].Name, outSize-(outCur-outData));
	}

	if ((inCur-inData)<inSize) {
		currupt = 1;
		output_error("#%s:%s# %d input bytes unused!\n", pak->Name, pak->Entries[id].Name, inSize-(inCur-inData));
	}

	if (currupt) {
		free(outData);
		return NULL;
	}

	if (size!=NULL)
		*size = outSize;

	return outData;
}

void pl2Pak_destroy(pl2Pak *pak) {
	if (pak->Flags&1)
		free(pak->Data);
	free(pak);
}

