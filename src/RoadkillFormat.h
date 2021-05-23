#ifndef h_RoadkillFormat
#define h_RoadkillFormat

// License - WTFPL
// latest code revision at 2021.05.23
// painstakingly deduced precious model and texture data brought to you by Spectre & HaDDayn
// to contact, use either of those:
// https://forum.xentax.com/memberlist.php?mode=viewprofile&u=57990
// https://github.com/Spectre-srs/RoadkillParser

// roadkill uses x+ for west, y+ for up, and z+ for south(forward), feet for spatial units,
// and stores vertex color as BGRA, while texture color is RGBA, alpha value only goes up to x80 in both
// all variables are laid out exactly how they are stored in the file // except for _debug
// all data is in little endian, and written as so - exactly how it is stored in the file
// % indicates how much of the section is fully known the purpose of
// _x variables have the exact same value across every single file, written as hex in its name
// _const variables are the same, but their meaning is somewhat deducible
// _unk variables have values that do change, but their meaning wasn't identified
// _ver variables are dependent on the file format version and may be absent in another
// _debug variables are not part of the file format, and hold arbitrary data about the parsing process

#include <istream>
#include <vector>

namespace Roadkill
{

//----------------------------------------------------------
typedef char int8;
typedef short int16;
typedef int int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef std::vector<int8> int8block;
typedef std::vector<int16> int16block;
typedef std::vector<int32> int32block;
typedef std::vector<uint8> uint8block;
typedef std::vector<uint16> uint16block;
typedef std::vector<uint32> uint32block;
typedef std::vector<float> floatblock;



//----------------------------------------------------------
template<typename T, uint32 N>
inline uint32 arrsize(T const (&array) [N])
{
	return N;
}



//----------------------------------------------------------
template<typename T>
inline void Read( std::istream& from, std::vector<T>& to, uint32 size )
{
	to.resize( size );
	if( size != 0 )
	{
		void* data = &to[0];
		from.read( static_cast<char*>(data), sizeof(T) * size );
	}
}



//----------------------------------------------------------
template<typename T>
inline void ReadPadded( std::istream& from, std::vector<T>& to, uint32 size, uint32 multipleOf = 4 )
{
	// some 1-byte and 2-bytes data is still stored in blocks that are multiples of 4 bytes
	uint32 bytes = sizeof(T) * size;
	// so if the data is not a multiple of 4 bytes, we still should read a round size
	uint32 padBytes = multipleOf - ( bytes % multipleOf ); // 1 2 3 4
	padBytes %= multipleOf;                                // 1 2 3 0
	// we read the missing bytes as an additional imaginary value to the data size
	uint32 imaginaryValues = padBytes / sizeof(T);

	Read( from, to, size + imaginaryValues );
}



//----------------------------------------------------------
template<typename T>
inline void Read( std::istream& from, T& to )
{
	void* data = &to;
	from.read( static_cast<char*>(data), sizeof(T) );
}



//----------------------------------------------------------
inline uint32 CurrentPosition( std::istream& file )
{
	return static_cast<uint32>(file.tellg());
}



//======================================================



//----------------------------------------------------------
struct rTextureHeader //75%
{
	uint32 _x03000000;
	uint32 format;              // mostly 09(8bit index), sometimes 0A(4bit index), or even 06 (swizzled 8bit index)
	uint32 imgWidth;            // image horizontal dimension
	uint32 imgHeight;           // image vertical dimension
	uint32 _x00000000;
	uint32 _unkFlag;            // 0 three-quarters of the time, 2 the last quarter, can't see the meaning
	uint32 _x00000000_[4];
	uint32 _unkFive;            // mostly 0x41000050, x05000050 in 4bit
	uint32 _unkFour;            // mostly 0x40000000, x04000000 in 4bit
	uint32 _x00000008;
	uint32 _x00000000__[2];
	uint32block pallete;        // bytes stored as RGBA, alpha is in 0-80 range, 16 or 256 colors depending on index format

	uint32 _x00000000___[3];
	uint16 _unkSomeSize1;       // always _unkSomeSize+1
	uint16 _x0050;
	uint32 _unkSomeSize;        // always width*height divided by 16 if 8bit index, and divided by 32 if 4bit
	uint32 _x00000008_;
	uint32 _x00000000____[2];
	uint8block pixels;          // width*height indices into the color pallete, or only half as much with 4bit indices

	uint8block _unkTrailing;    // some files have unknown data of undetermined size at the end, I see patterns
	                                // in it, but I can't even begin to guess what does it mean, quite a possibility
	                                // it's just padding, but blotches of data can be a bit too big
	                                // mip maps? no, can't be

	void Fill( std::istream& file )
	{
		Read( file, _x03000000 );
		Read( file, format );
		Read( file, imgWidth );
		Read( file, imgHeight );
		Read( file, _x00000000 );
		Read( file, _unkFlag );
		Read( file, _x00000000_ );
		Read( file, _unkFive );
		Read( file, _unkFour );
		Read( file, _x00000008 );
		Read( file, _x00000000__ );

		const uint32 amount = (format == 0x0A) ? 16 : 256;
		Read( file, pallete, amount );

		Read( file, _x00000000___ );
		Read( file, _unkSomeSize1 );
		Read( file, _x0050 );
		Read( file, _unkSomeSize );
		Read( file, _x00000008_ );
		Read( file, _x00000000____ );

		uint32 resolution = imgWidth * imgHeight;
		if( format == 0x0A ) resolution /= 2;
		Read( file, pixels, resolution );

		uint8 byte;
		while( file.peek() != std::istream::traits_type::eof() )
		{
			Read( file, byte );
			_unkTrailing.push_back( byte );
		}
	};
};



//----------------------------------------------------------
class rTextureFormat
{
public:
	rTextureHeader header;

	void Load( std::istream &file )
	{
		header.Fill( file );
	};
};



//======================================================



//----------------------------------------------------------
struct rModelBoundBox //100%
{
	float bboxMin[3];           // boundbox dimensions, XYZ min XYZ max
	float bboxMax[3];

	void Fill( std::istream& file )
	{
		Read( file, bboxMin );
		Read( file, bboxMax );
	};
};



//----------------------------------------------------------
struct rChunkPadding //100%?
{
	uint32 _x00000017;
	uint32block _x00000000;     // chunk files have a trailing padding of 4 byte zeros of 0-3
	                                // amount and it has nothing to do with 16bit alignment,
	                                // because chunks themselves don't align to 16 in their size,
	                                // I dunno if the padding pattern is important and what dictates it

	void Fill( std::istream& file )
	{
		Read( file, _x00000017 );

		uint32 zero;
		Read( file, zero );
		while( zero == 0 )
		{
			if( file.eof() ) return;
			_x00000000.push_back( zero );
			Read( file, zero );
		}
		file.seekg( -static_cast<int>(sizeof(zero)), std::ios::cur );
	};

	uint32 Size()
	{
		return sizeof(_x00000017) + _x00000000.size() * sizeof(_x00000000[0]);
	}
};



//----------------------------------------------------------
struct rModelHeader //80%
{
	uint32 version;             // model version, 4 for beta, 6 for release, influences subtle format differences
	uint32 meshCount;           // count of rModelDescriptor structs
	uint32 _ver6hasCollision;   // 1 if has rModelCollision struct, 0 otherwise //maybe count too? never seen >1
	                                // version 4 models always contain rModelCollision, even if it has 0 vertices
	uint32 texturesCount;       // count of rModelTexture structs
	uint32 _ver6_unkNotOne;     // almost always 1, 0 in TRFICLGHT.SMB and PLATFORMA.SMB release models

	void Fill( std::istream& file )
	{
		Read( file, version );
		Read( file, meshCount );

		if( version == 6 )
		{
			Read( file, _ver6hasCollision );
			Read( file, texturesCount );
			Read( file, _ver6_unkNotOne );
		}
		else
		{
			_ver6hasCollision = 1;
			Read( file, texturesCount );
		}
	};
};



//----------------------------------------------------------
struct rModelTexture //80%
{
	uint32 _x06000000;
	uint32 flags;               // mostly 0, x08 in models with light glow sprites and hand held weapons,
	                                // x01 in PGULAGPOLE2.SMB and CO1DROP.SMB (and DEFTREE.SMB v4 model)
	                                // x08 might be an alpha channel, judging by rArenaTexture counterpart
	uint32 _x00000000[2];
	char texName[216];          // giant name of the used texture, rest is filled with zeros

	void Fill( std::istream& file )
	{
		Read( file, _x06000000 );
		Read( file, flags );
		Read( file, _x00000000 );
		Read( file, texName );
	};
};



//----------------------------------------------------------
struct rModelCollision //100%
{
	char _ver6_constLegacy[23]; // "theLegacyCollisionPart", null terminated
	char _ver6_unkNotLegacy[9]; // junk most possibly, considering it's just a rest of 32-byte long name
	uint32 _x01000000;
	uint32 vertexCount;         // vertex count
	uint32 trianglesCount;      // triangles count

	floatblock vertexData;      // XYZ triples for vertex positions, [vertexCount*3]
	uint16block triangleData;   // triples of parent vertex ids for triangles, [trianglesCount*3]

	void Fill( std::istream& file, uint32 modelVersion )
	{
		if( modelVersion == 6 )
		{
			Read( file, _ver6_constLegacy );
			Read( file, _ver6_unkNotLegacy );
		}
		else
		{
			strcpy(_ver6_constLegacy, "theLegacyCollisionPart" );
		}

		Read( file, _x01000000 );
		Read( file, vertexCount );
		Read( file, trianglesCount );
		Read( file, vertexData, vertexCount * 3 );
		Read( file, triangleData, trianglesCount * 3 );
	};
};



//----------------------------------------------------------
struct rModelDescriptor //90%
{
	char meshName[32];          // mesh model name, zero terminated string with random junk after the end
	uint16 textureId;           // id of the rModelTexture struct to use material from
	uint32 _x02000000;
	rModelBoundBox bbox;        // min and max coordinates for the object bound box
	uint32 _x02000000_;
	uint32 chunksSize;          // size in bytes of model geometry chunks in which this mesh is described
	uint32 _x06000000;
	uint32 vertexCountRelated;  // amount of vertices after welding?
	                                // only shows real count if mesh has only one chunk, else - less
	uint32 trianglesCount;      // amount of triangles in the mesh
	uint32 _unkMoosor;          // seems to be junk // confirmed for junk 100%, seen even bits of strings

	void Fill( std::istream& file )
	{
		Read( file, meshName );
		Read( file, textureId );
		Read( file, _x02000000 );
		bbox.Fill( file );
		Read( file, _x02000000_ );
		Read( file, chunksSize );
		Read( file, _x06000000 );
		Read( file, vertexCountRelated );
		Read( file, trianglesCount );
		Read( file, _unkMoosor );
	};
};



//----------------------------------------------------------
struct rModelGeometryChunk //100%
{
	uint32 _x0080016D;
	uint16 headerVertexCount;   // amount of vertices, same as another one
	uint16 headerQuadsCount;    // amount of quads, same as another one
	uint16 headerVertex3Count;  // amount of vertex pos floats plus one, VertexCount*3+1, same as another one

	uint32 _xE300030100010180[2]; // note 0180 at the end
	uint8 vertexCount;          // amount of vertex position triplets
	uint8 vertexResolution;     // if 69, vertexData16 is used, if 68 - vertexData32
	int16block vertexData16;    // XYZ triples for vertex positions, [vertexCount*3], 16bit fixed precision / 256.0f
	int32block vertexData32;    // XYZ triples for vertex positions, [vertexCount*3], 32bit fixed precision / 256.0f

	uint16 _x0280;              // note 0280
	uint8 normalsCount;         // amount of vertex normals triplets
	uint8 _x6A;
	int8block normalsData;      // XYZ triplets of vertex normals, [normalsCount*3], fixed precision /128.0f

	uint16 _x0380;
	uint8 uvCount;              // amount of UV position pairs
	uint8 _x65;
	int16block uvData;          // UV coordinate pairs for textures, [uvCount*2], fixed precision / 4096.0f

	uint32 _x04040001;
	uint8 vertex3Count;         // amount of vertex pos floats plus one, VertexCount*3+1
	uint8 _xC0;
	uint8 quadsCount;           // amount of triangle pair vertex ids quadruplets
	uint8 _x6E;
	uint8block quadsData;       // [quadsCount*4], quads are made of paired triangles,
	                                // first uses 012 ids for the triangle, second 023
	                                // quad ids begin with 1 and point directly into the vertex data,
	                                // counting vertex id from beginning and skipping by 3

	rChunkPadding padding;

	uint32 _debugChunkSize;     // size of data in this chunk, including padding

	void Fill( std::istream& file )
	{
		_debugChunkSize = CurrentPosition(file);

		Read( file, _x0080016D );
		Read( file, headerVertexCount );
		Read( file, headerQuadsCount );
		Read( file, headerVertex3Count );

		Read( file, _xE300030100010180 );
		Read( file, vertexCount );
		Read( file, vertexResolution );

		if( vertexResolution == 0x68 )
			Read( file, vertexData32, vertexCount * 3 );
		else
			ReadPadded( file, vertexData16, vertexCount * 3 );

		Read( file, _x0280 );
		Read( file, normalsCount );
		Read( file, _x6A );
		ReadPadded( file, normalsData, normalsCount * 3 );

		Read( file, _x0380 );
		Read( file, uvCount );
		Read( file, _x65 );
		Read( file, uvData, uvCount * 2 );

		Read( file, _x04040001 );
		Read( file, vertex3Count );
		Read( file, _xC0 );
		Read( file, quadsCount );
		Read( file, _x6E );
		Read( file, quadsData, quadsCount * 4 );

		_debugChunkSize = CurrentPosition(file) - _debugChunkSize;
		padding.Fill( file );
		_debugChunkSize += padding.Size();
	};
};



//----------------------------------------------------------
class rModelFormat
{
public:
	rModelHeader header;
	std::vector<rModelTexture> textures;
	std::vector<rModelCollision> collision;
	rModelBoundBox boundbox;
	std::vector<rModelDescriptor> descriptors;
	std::vector<rModelGeometryChunk> chunks;

	void Load( std::istream &file )
	{
		header.Fill( file );

		textures.resize( header.texturesCount );
		for( uint32 i = 0; i < header.texturesCount; ++i )
		{
			textures[i].Fill( file );
		}

		if( header.version == 4 || header._ver6hasCollision == 1 )
		{
			collision.push_back( rModelCollision() );
			collision.back().Fill( file, header.version );
		}

		boundbox.Fill( file );

		uint32 sizeToRead = 0;
		descriptors.resize( header.meshCount );
		for( uint32 i = 0; i < header.meshCount; ++i )
		{
			descriptors[i].Fill( file );
			sizeToRead += descriptors[i].chunksSize;
		}

		uint32 sizeRead = 0;
		while( sizeRead < sizeToRead )
		{
			chunks.push_back( rModelGeometryChunk() );
			chunks.back().Fill( file );
			sizeRead += chunks.back()._debugChunkSize;
		}

		//if( sizeRead != sizeToRead ) throw some warning;
	};
};



//======================================================



//----------------------------------------------------------
struct rArenaHeader //90-100%
{
	uint32 _x05000000;
	float sunDir[3];            // unit length XYZ vector, supposedly light direction?
	float sunColor[3];          // 0-255 RGB value, supposedly sun or light color?
	float fogColor[3];          // 0-255 RGB value, color of fog
	float fogDistance;          // something in the 0.3-0.61 range, I think it corresponds with
	                                // a view distance for fog? 0.3 being on a smallest night arena
	                            // I'm sure I'm done with this struct, but I can't test my theories,
	                            // unless I'll modify game data to see if it changes in predictable way
	uint32 _x06000000;
	uint32 texturesCount;       // amount of texture names to be followed

	void Fill( std::istream& file )
	{
		Read( file, *this );
	};
};



//----------------------------------------------------------
struct rArenaTexture //100%
{
	uint32 flags;               // x08 is probably alpha channel, x00 otherwise, but it's unreliable,
	                                // PYTHO.BQS has none of them set to 8, but uses alpha textures anyway
	uint32 _x00000000[2];
	char texName[216];          // a null-filled texture name string

	void Fill( std::istream& file )
	{
		Read( file, flags );
		Read( file, _x00000000 );
		Read( file, texName );
	};
};



//----------------------------------------------------------
struct rArenaDescriptor //90% // practically identical to rModelDescriptor
{
	uint16 textureId;           // id of the rArenaTexture struct to use material from
	uint16 _unkSomeFlag;        // it's mostly x00, sometimes x01, rarely x18, and even rarer x03(in three cities)
	uint32 _x02000000;
	rModelBoundBox bbox;        // min and max coordinates for the object bound box
	uint32 _x02000000_;
	uint32 chunksSize;          // size in bytes of model geometry chunks in which this mesh is described
	uint32 _x06000000;
	uint32 vertexCountRelated;  // unreliable amount of vertices, after welding? only shows real count if
	                                // mesh has only one chunk, else - less, sometimes - more
	uint32 trianglesCount;      // unreliable amount of triangles in the mesh, though sometimes shows
	                                // more than chunks have, when it does vertex count also may be
	                                // bigger than in the chunks, what is this inconsistency about?
	uint32 _unkJunk;            // seems to be junk // confirmed for junk 100%, seen even bits of strings

	void Fill( std::istream& file )
	{
		Read( file, textureId );
		Read( file, _unkSomeFlag );
		Read( file, _x02000000 );
		bbox.Fill( file );
		Read( file, _x02000000_ );
		Read( file, chunksSize );
		Read( file, _x06000000 );
		Read( file, vertexCountRelated );
		Read( file, trianglesCount );
		Read( file, _unkJunk );
	};
};



//----------------------------------------------------------
struct rArenaCollision //100%
{
	uint32 vertexCount;         // vertex count, was seen up to 4163
	uint32 trianglesCount;      // triangles count, was seen up to 5958

	floatblock vertexData;      // XYZ triples for vertex positions, [vertexCount*3]
	uint16block triangleData;   // triplets of parent vertex ids for triangles, [trianglesCount*3]

	uint32 collisionGroup;      // material type, and I believe it's one byte long, there in the middle
/* known values
 * note: names are arbitrary, collgroups are used pretty vaguely and inconsistently,
 * not even mentioning misplaced ones, and demo maps have every single collgroup set to 0
00000000 - CONCRETE     // pretty random details of buildings, pavements, bridges, some walls
00000200 - BLACKTAR     // roads and most of buildingtops
00000300 - SIDEWALK     // sidewalks, tilesquares
00000400
00000500 - PAVEMENT     // most of the driveable concrete, bridges
00000600 - DIRT         // also known as sand in paradise city
00000800 - GRASS
00000900 - ROUGH        // rubbish, train tracks, few roofs
00000A00 - ROCK         // wallrock in lava falls
00000B00 - METALTOP     // metal trampolines, metal roofs, some fences
00000C00 - a single quad of ROUGH in blister canyon
00000D00 - some metal pipes in blister canyon
00000F00 - BEACH        // aka SAND
00001200 - LINOLEUM     // ??? driveable parts of WOOD-structures, wooden trampolines
00001300 - BRICKS       // buildings and pillars
00001400 - CLOTH        // woody's tent-roof, and slide-roofs of stalls in blister canyon
00001500 - BUILDING     // aka GENERIC GEOMETRY, most of the buildings and concrete things
00001700 - GLASS        // bug: some blow johnson airport pillars too
00001800 - METAL
00001900 - single patch of billboards in blister canyon
00001A00 - WALLROCK     // woody's cave attraction, wallrock in blister canyon
00001B00 - TREES        // used as woodsticks too
00001C00 - WOOD         // bug: entire woody's entrance is considered wood when it's not
00001D00 - SKYBOX
00001E00 - RAILING      // paling, railing, fences, see-through metal with holes // is it bullet-penetrable?
*/

	void Fill( std::istream& file )
	{
		Read( file, vertexCount );
		Read( file, trianglesCount );
		Read( file, vertexData, vertexCount * 3 );
		Read( file, triangleData, trianglesCount * 3 );
		Read( file, collisionGroup );
	};
};



//----------------------------------------------------------
struct rMapTile //100%
{
	uint32 arenaDescriptorsCount; // amount of rArenaDescriptors to follow
	uint32 collisionChunksCount;  // amount of rArenaCollisions to follow, after rArenaDescriptors if any
	std::vector<rArenaDescriptor> descriptors;
	std::vector<rArenaCollision> collision;
	uint32 geometryDataAddress;   // address of where this tile's mesh data (if any) begins in the file
	uint32 geometryDataSize;      // size in bytes of that data (if any), including location and geometry chunks
	                                  // note: both are unreliable, NC.BQS has its id718 tile size miscalculated,
	                                  // showing 16 bytes more than it actually is, shifting every address in the
	                                  // subsequent tiles by 16, making reading file via those impossible

	void Fill( std::istream& file )
	{
		Read( file, arenaDescriptorsCount );
		Read( file, collisionChunksCount );

		descriptors.resize( arenaDescriptorsCount );
		for( uint32 i = 0; i < arenaDescriptorsCount; ++i )
		{
			descriptors[i].Fill( file );
		}

		collision.resize( collisionChunksCount );
		for( uint32 i = 0; i < collisionChunksCount; ++i )
		{
			collision[i].Fill( file );
		}

		Read( file, geometryDataAddress );
		Read( file, geometryDataSize );
	};
};



//----------------------------------------------------------
struct rArenaLocation //50%
{
	uint32 _x0080026C;
	uint32 _x00000000[4];
	float meshPos[3];           // XYZ position of mesh in the world
	float _constMeshScale;      // wild guess, never seen it to diverge from 1.f (0000803F)

	uint16 _x0280;
	uint8 _const_unkDataCount;  // amount of unknown data to follow, always 8
	uint8 _x6D;
	uint16block _unkData;       // [_const_unkDataCount*4] quads of shorts, they have peculiar pattern,
	                                // as if trying to list coordinates of every corner of AABB box:
	                                //  A, B, C, 0
	                                //  X, B, C, 0          // unfortunately, their
	                                //  A, Y, C, 0          // meaning is lost on me
	                                //  X, Y, C, 0
	                                //  A, B, Z, 0
	                                //  X, B, Z, 0
	                                //  A, Y, Z, 0
	                                //  X, Y, Z, 0

	rChunkPadding padding;

	uint32 _debugChunkSize;     // size of data in this chunk, including padding // always 108 in this struct
	uint32 _debugChunkAddress;  // offset from beginning of the file at which this chunk was found

	void Fill( std::istream& file )
	{
		_debugChunkSize = _debugChunkAddress = CurrentPosition(file);

		Read( file, _x0080026C );
		Read( file, _x00000000 );
		Read( file, meshPos );
		Read( file, _constMeshScale );

		Read( file, _x0280 );
		Read( file, _const_unkDataCount );
		Read( file, _x6D );
		Read( file, _unkData, _const_unkDataCount * 4 );

		_debugChunkSize = CurrentPosition(file) - _debugChunkSize;
		padding.Fill( file );
		_debugChunkSize += padding.Size();
	};
};



//----------------------------------------------------------
struct rArenaGeometryChunk //100% // partially identical to rModelGeometryChunk
{
	uint32 _x0080016D;
	uint16 headerVertexCount;   // amount of vertices, same as another one
	uint16 headerQuadsCount;    // amount of quads, same as another one
	uint16 headerVertex4Count;  // amount of ... err, VertexCount*4+1, same as another one

	uint32 _xE300040100010180[2];// note 0180 at the end // number 4 differs from rModelGeometryChunk
	uint8 vertexCount;          // amount of vertex position triplets
	uint8 vertexResolution;     // if 69, vertexData16 is used, if 68 - vertexData32 // _const, always 69 in maps
	int16block vertexData16;    // XYZ triples for vertex positions, [vertexCount*3], 16bit fixed precision / 32.0f
//	int32block vertexData32;    // XYZ triples for vertex positions, [vertexCount*3], 32bit fixed precision / 32.0f
	                                // actually, 32 is never been used in maps, I wonder if it's even allowed

	uint16 _x02C0;
	uint8 vcolorsCount;         // amount of vertex colors
	uint8 _x6E;
	uint32block vcolorsData;    // uint8-quads of vertex colors, as-is order is BGRA, alpha is in 0-7F range

	uint16 _x0380;
	uint8 uvCount;              // amount of UV position pairs
	uint8 _x65;
	int16block uvData;          // UV coordinate pairs for textures, [uvCount*2], fixed precision / 4096.0f

	uint32 _x04040001;
	uint8 vertex4Count;         // amount of ... errr, VertexCount*4+1
	uint8 _xC0;
	uint8 quadsCount;           // amount of triangle pair vertex ids quadruplets
	uint8 _x6E_;
	uint8block quadsData;       // [quadsCount*4], quads are made of paired triangles, first uses 012 ids for triangle,
	                                // second 023, vertex ids begin with 1 and *not* point directly into the vertex data,
	                                // counting ids from beginning and skipping by *4* somewhy

	rChunkPadding padding;

	uint32 _debugChunkSize;     // size of data in this chunk, including padding

	void Fill( std::istream& file )
	{
		_debugChunkSize = CurrentPosition(file);

		Read( file, _x0080016D );
		Read( file, headerVertexCount );
		Read( file, headerQuadsCount );
		Read( file, headerVertex4Count );

		Read( file, _xE300040100010180 );
		Read( file, vertexCount );
		Read( file, vertexResolution );

//		if( vertexResolution == 0x68 )
//			Read( file, vertexData32, vertexCount * 3 );
//		else
			ReadPadded( file, vertexData16, vertexCount * 3 );

		Read( file, _x02C0 );
		Read( file, vcolorsCount );
		Read( file, _x6E );
		Read( file, vcolorsData, vcolorsCount );

		Read( file, _x0380 );
		Read( file, uvCount );
		Read( file, _x65 );
		Read( file, uvData, uvCount * 2 );

		Read( file, _x04040001 );
		Read( file, vertex4Count );
		Read( file, _xC0 );
		Read( file, quadsCount );
		Read( file, _x6E_ );
		Read( file, quadsData, quadsCount * 4 );

		_debugChunkSize = CurrentPosition(file) - _debugChunkSize;
		padding.Fill( file );
		_debugChunkSize += padding.Size();
	};
};



//----------------------------------------------------------
class rArenaFormat
{
public:
	rArenaHeader header;
	std::vector<rArenaTexture> textures;
	float _unkNegative;         // some float in -877 -221 range, no idea
	uint32 _constGridSize;      // always 1024, which corresponds with cells amount, is it dynamic?
	rMapTile cells[1024];       // 32*32 grid of geometry pieces, grid goes east-to-west north-to-south,
	                                // but it is nowhere enforced, since every cell's rArenaLocation
	                                // has its own position variable
	struct FloatPair {
		float min;
		float max;
	} heights[1024];            // upper and lower coordinate bounds of visual geometry in corresponding cell,
	                                // holds positive and negative 1.0e+30 for cells with no mesh chunks

	// these two fill rest of the file interchangingly, rArenaLocation goes first,
	// then its corresponding geometry chunks, then next location again
	// edit: technically, geometry chunks go into location, I'm sorry I've tired
	std::vector<rArenaLocation> locations;
	std::vector<rArenaGeometryChunk> geometry;

	void Load( std::istream &file )
	{
		header.Fill( file );

		textures.resize( header.texturesCount );
		for( uint32 i = 0; i < header.texturesCount; ++i )
		{
			textures[i].Fill( file );
		}

		Read( file, _unkNegative );
		Read( file, _constGridSize );

		for( uint32 i = 0; i < arrsize(cells); ++i )
		{
			cells[i].Fill( file );
		}

		Read( file, heights );

		while( !file.eof() )
		{
			uint32 header;
			Read( file, header );
			file.seekg( -static_cast<int>(sizeof(header)), std::ios::cur );

			if( header == 0x6C028000 )
			{
				locations.push_back( rArenaLocation() );
				locations.back().Fill( file );
			}
			else if( header == 0x6D018000 )
			{
				geometry.push_back( rArenaGeometryChunk() );
				geometry.back().Fill( file );
			}
			else
			{
				uint32 pos = CurrentPosition(file);
				printf( "\nbroke at %x\n", pos );
				break;
			}
		}
	}
};

} //namespace



#endif //h_RoadkillFormat
