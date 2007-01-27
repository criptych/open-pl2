#include <libpl2.h>

#define PL2TMB_MAGIC 0x30424d54


void pl2TMB_destroy(pl2TMB *tmb) {
	if (tmb==NULL)
		return;

	if (tmb->Textures!=NULL)
		free(tmb->Textures);

	if (tmb->Meshes!=NULL)
		free(tmb->Meshes);

	if (tmb->Flags&1)
		free(tmb->Data);

	free(tmb);
}

#define REMAINING(_cur) \
	(((uptr)tmb->Data+tmb->Size)-(uptr)(_cur))

#define CHECK_SIZE( \
	_name, \
	_cur, \
	_req \
) \
do { \
	if (((uptr)_cur+_req)>((uptr)tmb->Data+tmb->Size)) { \
		output_error("#%s:%s# Truncated TMB?!? %p>%p\n", tmb->Name, _name, ((uptr)_cur+_req), ((uptr)tmb->Data+tmb->Size)); \
		return NULL; \
	} \
} \
while (0) 


///////////////////////////////////////
// Texture section
///////////////////////////////////////
static u8 *pl2TMB_textureStart(pl2TMB *tmb, u8 *cur, u32 count) {
	tmb->TextureCount = count;
	tmb->Textures = (pl2TMB_Texture**)malloc(sizeof(pl2TMB_Texture*)*tmb->TextureCount);
	if (tmb->Textures==NULL) {
		output_error("#%s:Texture# Failed to allocate helper memory\n", tmb->Name);
		return NULL;
	}
	return cur;
}

static u8 *pl2TMB_textureEntry(pl2TMB *tmb, u8 *cur, u32 id) {
	CHECK_SIZE("Texture", cur, sizeof(pl2TMB_Texture));
	tmb->Textures[id] = (pl2TMB_Texture*)cur;
	tmb->Textures[id]->Name[31] = '\0';

	if (tmb->Textures[id]->Width>=4096 || tmb->Textures[id]->Height>=4096) {
		output_error("#%s:Texture%d:%s# Huge texture size! (%d,%d)\n", tmb->Name, id, tmb->Textures[id]->Name, tmb->Textures[id]->Width, tmb->Textures[id]->Height);
		return NULL;
	}
	cur += sizeof(pl2TMB_Texture);

	cur += tmb->Textures[id]->Width*tmb->Textures[id]->Height*4;
	CHECK_SIZE("Texture", cur, 0);
	return cur;
}

static u8 *pl2TMB_textureEnd(pl2TMB *tmb, u8 *cur) {
	return cur;
}

///////////////////////////////////////
// Material section
///////////////////////////////////////
static u8 *pl2TMB_materialStart(pl2TMB *tmb, u8 *cur, u32 count) {
	tmb->MaterialCount = count;
	tmb->Materials = (pl2TMB_Material*)cur;
	return cur;
}

static u8 *pl2TMB_materialEntry(pl2TMB *tmb, u8 *cur, u32 id) {
	CHECK_SIZE("Material", cur, sizeof(pl2TMB_Material));
	cur += sizeof(pl2TMB_Material);
	return cur;
}

static u8 *pl2TMB_materialEnd(pl2TMB *tmb, u8 *cur) {
	return cur;
}

///////////////////////////////////////
// Mesh section
///////////////////////////////////////
static u8 *pl2TMB_meshStart(pl2TMB *tmb, u8 *cur, u32 count) {
	tmb->MeshCount = count;
	tmb->Meshes = malloc(sizeof(pl2TMB_Mesh)*tmb->MeshCount);
	if (tmb->Meshes==NULL) {
		output_error("#%s:Mesh# Failed to allocate helper memory\n", tmb->Name);
		return NULL;
	}

	return cur;
}

static u8 *pl2TMB_meshEntry(pl2TMB *tmb, u8 *cur, u32 id) {
	CHECK_SIZE("Mesh", cur, 108);

	tmb->Meshes[id].Name = cur;
	tmb->Meshes[id].Name[31] = '\0';
	cur+=32;

	tmb->Meshes[id].Matrix = (float*)cur;
	cur+=sizeof(float)*16;
	
	tmb->Meshes[id].TriangleCount = *(u32*)cur;
	if (tmb->Meshes[id].TriangleCount>=5000000) {
		output_error("#%s:Mesh%d:%s# Huge triangle count! (%d)\n", tmb->Name, id, tmb->Meshes[id].Name, tmb->Meshes[id].TriangleCount);
		return NULL;
	}
	cur+=4;

	tmb->Meshes[id].Unknown = *(u32*)cur;
	
	if (tmb->Meshes[id].Unknown)
		output_debug("#%s:Mesh%d:%s# Unknown value = %d (0x%08x)\n", tmb->Name, id, tmb->Meshes[id].Name, tmb->Meshes[id].Unknown, tmb->Meshes[id].Unknown);
	cur+=4;


	tmb->Meshes[id].BindCount = *(u32*)cur;
	if (tmb->Meshes[id].BindCount>=64) {
		output_error("#%s:Mesh%d:%s# Huge bind count! (%d)\n", tmb->Name, id, tmb->Meshes[id].Name, tmb->Meshes[id].BindCount);
		return NULL;
	}
	cur+=4;

	tmb->Meshes[id].Binds = (pl2TMB_Bind*)cur;
	cur+=sizeof(pl2TMB_Bind)*tmb->Meshes[id].BindCount;
	CHECK_SIZE("Mesh", cur, 0);
	// XXX should check that binds are valid here

	tmb->Meshes[id].Vertices = (pl2TMB_Vertex*)cur;
	cur+=sizeof(pl2TMB_Vertex)*tmb->Meshes[id].TriangleCount*3;
	CHECK_SIZE("Mesh", cur, 0);
	// XXX should check that vertices are valid here

	return cur;
}

static u8 *pl2TMB_meshEnd(pl2TMB *tmb, u8 *cur) {
	return cur;
}

///////////////////////////////////////
// Bone section
///////////////////////////////////////
static u8 *pl2TMB_boneStart(pl2TMB *tmb, u8 *cur, u32 count) {
	tmb->BoneCount = count;
	tmb->Bones = (float**)malloc(sizeof(float*)*tmb->BoneCount);
	if (tmb->Bones==NULL) {
		output_error("#%s:Bone# Failed to allocate helper memory\n", tmb->Name);
		return NULL;
	}
	return cur;
}

static u8 *pl2TMB_boneEntry(pl2TMB *tmb, u8 *cur, u32 id) {
	tmb->Bones[id] = (float*)cur;
	cur += sizeof(float)*16;
	return cur;
}

static u8 *pl2TMB_boneEnd(pl2TMB *tmb, u8 *cur) {
	return cur;
}

///////////////////////////////////////
// Unknown section
///////////////////////////////////////
static u8 *pl2TMB_unknownStart(pl2TMB *tmb, u8 *cur, u32 count) {
	tmb->UnknownCount = count;
	return cur;
}

static u8 *pl2TMB_unknownEntry(pl2TMB *tmb, u8 *cur, u32 id) {
	cur += 10*4;
	return cur;
}

static u8 *pl2TMB_unknownEnd(pl2TMB *tmb, u8 *cur) {
	for (u32 id=0;id<tmb->UnknownCount;id++) {
		cur += strlen((char*)cur)+1;
	}
	return cur;
}



typedef struct {
	u32 Flags;
	const char *Name;
	u8 *
	(*Start)(
		pl2TMB *tmb,
		u8 *cur,
		u32 count
	);
	u8 *
	(*Entry)(
		pl2TMB *tmb,
		u8 *cur,
		u32 id
	);
	u8 *
	(*End)(
		pl2TMB *tmb,
		u8 *cur
	);
} pl2TMB_section;

static pl2TMB_section pl2TMB_sectionInfo[] = {
	{0x0000, "Texture",  pl2TMB_textureStart,  pl2TMB_textureEntry,  pl2TMB_textureEnd},
	{0x0000, "Material", pl2TMB_materialStart, pl2TMB_materialEntry, pl2TMB_materialEnd},
	{0x0000, "Mesh",     pl2TMB_meshStart,     pl2TMB_meshEntry,     pl2TMB_meshEnd},
	{0x0000, "Bone",     pl2TMB_boneStart,     pl2TMB_boneEntry,     pl2TMB_boneEnd},
	{0x0000, "Unknown",  pl2TMB_unknownStart,  pl2TMB_unknownEntry,  pl2TMB_unknownEnd},
	{0x0000, NULL,       NULL,                 NULL,                 NULL},
};


pl2TMB *pl2TMB_load(const char *file) {
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

	pl2TMB *tmb = pl2TMB_create(data, size, file);
	if (tmb==NULL) {
		free(data);
		return NULL;
	}

	tmb->Flags |= 1; // Say 'data' should be freed

	return tmb;
}

pl2TMB *pl2TMB_create(void *buf, u32 size, const char *name) {
	u8 *cur = (u8*)buf;
	if (size<8)
		return NULL;

	u32 magic = endian_u32(*(u32*)cur);
	if (magic!=PL2TMB_MAGIC) {
		output_error("#%s# Unknown magic! (%08x)\n", name, magic);
		return NULL;
	}
	cur+=4;

	pl2TMB *tmb = (pl2TMB*)malloc(sizeof(pl2TMB));
	if (tmb==NULL) {
		output_error("#%s# Failed to allocate helper memory\n", name);
		return NULL;
	}

	tmb->Data = buf;
	tmb->Size = size;
	tmb->Flags=0;
	strfilename(tmb->Name, sizeof(tmb->Name), name);
	tmb->TextureCount=0;
	tmb->Textures=NULL;
	tmb->MaterialCount=0;
	tmb->Materials=NULL;
	tmb->MeshCount=0;
	tmb->Meshes=NULL;
	tmb->BoneCount=0;
	tmb->Bones=NULL;
	tmb->UnknownCount=0;

	for (u32 sectionID=0;pl2TMB_sectionInfo[sectionID].Name!=NULL;sectionID++) {
		pl2TMB_section *section = &pl2TMB_sectionInfo[sectionID];

		if (REMAINING(cur)<4) {
			output_error("#%s:%s# EOF hit while reading count\n", tmb->Name, section->Name);
			pl2TMB_destroy(tmb);
			return NULL;
		}

		u32 count = endian_u32(*(u32*)cur);
		if (count>0x1000) {
			output_error("#%s:%s# Huge count! (%u)\n", tmb->Name, section->Name, count);
			pl2TMB_destroy(tmb);
			return NULL;
		}
		cur += 4;
	
		if (section->Start!=NULL) {
			void *ret = section->Start(tmb, cur, count);
			if (ret==NULL) {
				output_error("#%s:%s# Start failed\n", tmb->Name, section->Name);
				pl2TMB_destroy(tmb);
			}
			cur = ret;
		}

		if (section->Entry!=NULL) {
			for (u32 entryID=0;entryID<count;entryID++) {
				void *ret = section->Entry(tmb, cur, entryID);
				if (ret==NULL) {
					output_error("#%s:%s# Entry%d failed\n", tmb->Name, section->Name, entryID);
					pl2TMB_destroy(tmb);
				}
				cur = ret;
			}
		}

		if (section->End!=NULL) {
			void *ret = section->End(tmb, cur);
			if (ret==NULL) {
				output_error("#%s:%s# End failed\n", tmb->Name, section->Name);
				pl2TMB_destroy(tmb);
			}
			cur = ret;
		}

	}

	if (REMAINING(cur)) {
		output_error("#%s# %d bytes of trash at end of file?\n", tmb->Name, REMAINING(cur));
		pl2TMB_destroy(tmb);
		return NULL;
	}

	return tmb;
}

