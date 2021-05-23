// License - WTFPL
// latest code revision at 2021.05.23
// painstakingly deduced precious model and texture data brought to you by Spectre & HaDDayn
// to contact, use either of those:
// https://forum.xentax.com/memberlist.php?mode=viewprofile&u=57990
// https://github.com/Spectre-srs/RoadkillParser



//----------------------------------------------------------
#if defined(_WIN32) || defined(_WINDOWS)
#include <Windows.h>
#define concolor( x ) SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE), x )
#define grey    concolor( 0x0008 )
#define bright  concolor( 0x0007 )
#define white   concolor( 0x0007 | 0x0008 )
#define yellow  concolor( 0x0002 | 0x0004 | 0x0008 )
#define red     concolor( 0x0004 | 0x0008 )
const char PAUSE[] = "pause";
const char CLS[]   = "cls";
#elif defined(__unix__) || defined(__unix)
#include <unistd.h> // isatty()
#include <stdlib.h> // system() atoi()
#include <cstring>
#include <cstdio>
#define atty        isatty(fileno(stdout))
#define ttyprint(x) if( atty ) printf(x)
#define system(x)   if( atty ) system(x)
#define grey        ttyprint("\033[0m\033[2m")
#define bright      ttyprint("\033[0m\033[37m")
#define white       ttyprint("\033[0m\033[1;37m")
#define yellow      ttyprint("\033[0m\033[1;33m")
#define red         ttyprint("\033[0m\033[1;31m")
const char PAUSE[] = "read -p \"Press [Enter] key to continue...\"";
const char CLS[]   = "clear";
#else
#define grey
#define bright
#define white
#define yellow
#define red
#define system(x)
#endif



//----------------------------------------------------------
#include "RoadkillFormat.h"
using namespace Roadkill;
#include <string>
#include "PrintInfo.h"
#include <fstream>
#include <sstream>  // for easy obj output, temporary data holding
#include <iostream> // getline for manual interface
#include <cctype>   // toupper



//----------------------------------------------------------
std::string MakeUppercase( std::string str )
{
	for( uint32 i = 0, e = str.length(); i < e; ++i )
		str[i] = std::toupper(str[i]);
	return str;
}



//----------------------------------------------------------
// thank you kindly https://github.com/neko68k/rtftool/blob/master/RTFTool/rtfview/p6t_v2.cpp
void P6T_ProcessImg8( int w, int h, const uint8* inpixels, uint8* outpixels )
{
	int i = 0;

	for( int y = 0; y < h; ++y )
	{
		for( int x = 0; x < w; ++x )
		{
			int block_location = (y & (~0xf)) * w + (x & (~0xf)) * 2;
			int swap_selector = (((y + 2) >> 2) & 0x1) * 4;
			int posY = (((y & (~3)) >> 1) + (y & 1)) & 0x7;
			int column_location = posY * w * 2 + ((x + swap_selector) & 0x7) * 4;

			int byte_num = ((y >> 1) & 1) + ((x >> 2) & 2); // 0,1,2,3

			outpixels[(y * w) + x] = inpixels[block_location + column_location + byte_num];
		}
	}
}
void P6T_ProcessImg4( int w, int h, const uint8* inpixels, uint8* outpixels )
{
	uint8 entry;

	for( int y = 0; y < h; ++y )
	{
		for( int x = 0; x < w; ++x )
		{
			// get the pen
			int index = (y * w) + x;

			// swizzle
			int pageX = x &(~0x7f);
			int pageY = y &(~0x7f);
			int pages_horz = (w + 127) / 128;
			int pages_vert = (h + 127) / 128;

			int page_number = (pageY / 128) * pages_horz + (pageX / 128);
			int page32Y = (page_number / pages_vert) * 32;
			int page32X = (page_number % pages_vert) * 64;

			int page_location = page32Y * h * 2 + page32X * 4;
			int locX = x & 0x7f;
			int locY = y & 0x7f;

			int block_location = ((locX & (~0x1f)) >> 1) * h + (locY & (~0xf)) * 2;
			int swap_selector = (((y + 2) >> 2) & 0x1) * 4;
			int posY = (((y & (~3)) >> 1) + (y & 1)) & 0x7;
			int column_location = posY * h * 2 + ((x + swap_selector) & 0x7) * 4;
			int byte_num = (x >> 3) & 3; // 0,1,2,3

			entry = inpixels[page_location + block_location + column_location + byte_num];
			entry = (uint8)((entry >> ((y >> 1) & 0x01) * 4) & 0x0F);
			outpixels[index] = entry;
		}
	}
}



//----------------------------------------------------------
std::string OutputFormat( const rTextureFormat& ) { return ".tga"; }
int OutputTraits( const rTextureFormat& ) { return std::ios_base::binary; }
void ConvertAndSave( const rTextureFormat& tex, std::ostream &tga )
{
	// tga header
	{
		uint32 typewriter = 0;
		tga.write( (char*)&typewriter, 2 );
		typewriter = 2;
		tga.write( (char*)&typewriter, 1 ); // uncompressed RGB
		typewriter = 0;
		tga.write( (char*)&typewriter, 4 );
		tga.write( (char*)&typewriter, 1 );
		tga.write( (char*)&typewriter, 4 );
		tga.write( (char*)&tex.header.imgWidth, 2 );
		tga.write( (char*)&tex.header.imgHeight, 2 );
		typewriter = 32;
		tga.write( (char*)&typewriter, 1 ); // 32 bit per pixel
		typewriter = 40;
		tga.write( (char*)&typewriter, 1 ); // 8 for alpha, | 32 for y axis
	}

	const uint32 pixelsAmount = tex.header.imgWidth * tex.header.imgHeight;
	const uint8* pixels = &tex.header.pixels[0];

	uint8block restored;
	if( tex.header.format != 0x09 )
	{
		restored.resize( pixelsAmount );

		if( tex.header.format == 0x06 )
			P6T_ProcessImg8( tex.header.imgWidth, tex.header.imgHeight, pixels, &restored[0] );
		else if( tex.header.format == 0x0A )
			P6T_ProcessImg4( tex.header.imgWidth, tex.header.imgHeight, pixels, &restored[0] );
		// else throw some kind of unrecognized format error

		pixels = &restored[0];
	}

	union {
		uint8 hex[4];
		uint32 val;
	} src, out;
	uint8 idx;
	const uint32* pallete = &tex.header.pallete[0];

	for( uint32 i = 0; i < pixelsAmount; ++i )
	{
		idx = pixels[i];
		if( tex.header.format != 0x0A )
		{
			// each fourth and fifth bit of the index are swapped,
			// protection against what i did i suppose?
			uint8 bit1 = (idx >> 3) & 1;//  1000
			uint8 bit2 = (idx >> 4) & 1;// 10000
			uint8 diff = bit1 ^ bit2;
			idx ^= (diff << 3) | (diff << 4);
		}

		// RGBA > BGRA conversion
		// tga stores them as int32 ARGB, which makes them BGRA in little endian
		src.val = pallete[idx];
		out.hex[0] = src.hex[2];
		out.hex[1] = src.hex[1];
		out.hex[2] = src.hex[0];
		// alpha represented as 0x00-0x80 range, tga expects 0x00-0xFF
		out.hex[3] = (src.hex[3] == 0x80) ? 0xFF : src.hex[3] * 2;

		tga.write( (char*)&out, 4 );
	}
}



//----------------------------------------------------------
inline void PutVertex( std::stringstream& buff, float x, float y, float z )
{
	buff << "v  " << x << " " << y << " " << z << "\n";
}
inline void PutVertex( std::stringstream& buff, float x, float y, float z, float r, float g, float b )
{
	// non standard obj extension, but eh, better than nothing at all
	buff << "v  " << x << " " << y << " " << z << " "
		<< r << " " << g << " " << b << "\n";
}
inline void PutNormal( std::stringstream& buff, float x, float y, float z )
{
	buff << "vn  " << x << " " << y << " " << z << "\n";
}
inline void PutUv( std::stringstream& buff, float u, float v )
{
	buff << "vt  " << u << " " << v << "\n";
}
inline void PutFace1( std::stringstream& buff, int a, int b, int c )
{
	buff << "f  " << a << " " << b << " " << c  << "\n";
}
inline void PutFace2( std::stringstream& buff, int a, int b, int c )
{
	buff << "f  " // v/vt
		<< a << "/" << a << " "
		<< b << "/" << b << " "
		<< c << "/" << c << "\n";
}
inline void PutFace3( std::stringstream& buff, int a, int b, int c )
{
	buff << "f  " // v/vt/vn
		<< a << "/" << a << "/" << a << " "
		<< b << "/" << b << "/" << b << " "
		<< c << "/" << c << "/" << c << "\n";
}



//----------------------------------------------------------
std::string OutputFormat( const rModelFormat& ) { return ".obj"; }
int OutputTraits( const rModelFormat& ) { return 0; }
void ConvertAndSave( const rModelFormat& smb, std::ostream &obj )
{
	std::stringstream vertices;
	std::stringstream uvvertices;
	std::stringstream vertexnormals;
	std::stringstream faces;
	uint32 objVertexId = 1;
	obj << "mtllib roadkill.mtl\n\n";

	uint32 descriptor = 0;
	uint32 chunksSize = 0;

	for( uint32 _chunk = 0, _chunks = smb.chunks.size(); _chunk < _chunks; ++_chunk )
	{
		const rModelGeometryChunk& chunk = smb.chunks[_chunk];

		if( !chunk.vertexData16.empty() )
		for( uint32 i = 0, e = chunk.vertexCount; i < e; ++i )
		{
			float v0 = chunk.vertexData16[i*3+0] /256.f;
			float v1 = chunk.vertexData16[i*3+1] /256.f;
			float v2 = chunk.vertexData16[i*3+2] /256.f;
			// x axis is inverted in obj
			PutVertex( vertices, -v0, v1, v2 );
		}
		else if( !chunk.vertexData32.empty() )
		for( uint32 i = 0, e = chunk.vertexCount; i < e; ++i )
		{
			float v0 = chunk.vertexData32[i*3+0] /256.f;
			float v1 = chunk.vertexData32[i*3+1] /256.f;
			float v2 = chunk.vertexData32[i*3+2] /256.f;
			// x axis is inverted in obj
			PutVertex( vertices, -v0, v1, v2 );
		}

		for( uint32 i = 0, e = chunk.normalsCount; i < e; ++i )
		{
			float vn0 = chunk.normalsData[i*3+0] /128.f;
			float vn1 = chunk.normalsData[i*3+1] /128.f;
			float vn2 = chunk.normalsData[i*3+2] /128.f;
			// faces are flipped due to -x axis mirror, fix normals accordingly
			PutNormal( vertexnormals, -vn0, vn1, vn2 );
		}

		for( uint32 i = 0, e = chunk.uvCount; i < e; ++i )
		{
			float vt0 = chunk.uvData[i*2+0] /4096.f;
			float vt1 = chunk.uvData[i*2+1] /4096.f;
			// vertical axis is inverted in obj
			PutUv( uvvertices, vt0, 1.f - vt1 );
		}

		for( uint32 i = 0, e = chunk.quadsCount; i < e; ++i )
		{
			int f0 = chunk.quadsData[i*4+0] /3 +objVertexId;
			int f1 = chunk.quadsData[i*4+1] /3 +objVertexId;
			int f2 = chunk.quadsData[i*4+2] /3 +objVertexId;
			int f3 = chunk.quadsData[i*4+3] /3 +objVertexId;
			// faces are flipped due to -x axis mirror, inverse winding order
			PutFace3( faces, f0, f2, f1 );
			if( f2 != f3 )
				PutFace3( faces, f0, f3, f2 );
		}

		objVertexId += chunk.headerVertexCount;
		chunksSize += chunk._debugChunkSize;

		const rModelDescriptor& desc = smb.descriptors[descriptor];
		if( desc.chunksSize == chunksSize )
		{
			obj << "\ng " << desc.meshName << "\n";
			// obj material import is case sensitive, while roadkill engine is not
			obj << "usemtl " << MakeUppercase( smb.textures[desc.textureId].texName ) << "\n";
			obj << vertices.rdbuf();
			obj << vertexnormals.rdbuf();
			obj << uvvertices.rdbuf();
			obj << faces.rdbuf();
			vertices.clear();
			vertexnormals.clear();
			uvvertices.clear();
			faces.clear();
			++descriptor;
			chunksSize = 0;
		}
	}

	if( !smb.collision.empty() )
	{
		const rModelCollision& c = smb.collision[0];

		for( uint32 i = 0, e = c.vertexCount; i < e; ++i )
		{
			float v0 = c.vertexData[i*3+0];
			float v1 = c.vertexData[i*3+1];
			float v2 = c.vertexData[i*3+2];
			// x axis is inverted in obj
			PutVertex( vertices, -v0, v1, v2 );
		}

		for( uint32 i = 0, e = c.trianglesCount; i < e; ++i )
		{
			int f0 = c.triangleData[i*3+0] +objVertexId;
			int f1 = c.triangleData[i*3+1] +objVertexId;
			int f2 = c.triangleData[i*3+2] +objVertexId;
			// faces are flipped due to -x axis mirror, inverse winding order
			PutFace1( faces, f0, f2, f1 );
		}

		obj << "\ng theLegacyCollisionPart\n";
		obj << "usemtl theLegacyCollisionPart\n";
		obj << vertices.rdbuf();
		obj << faces.rdbuf();
		vertices.clear();
		faces.clear();
		objVertexId += c.vertexCount;
	}

	red;
	if( descriptor < smb.descriptors.size() )
		printf( ">OUTPUT ERROR - CHUNK AMOUNT MISMATCH, yes that's my error since I wont see it\n" );
}



//----------------------------------------------------------
std::string OutputFormat( const rArenaFormat& ) { return ".obj"; }
int OutputTraits( const rArenaFormat& ) { return 0; }
void ConvertAndSave( const rArenaFormat& bqs, std::ostream &obj )
{
	std::stringstream vertices;
	std::stringstream uvvertices;
	std::stringstream faces;
	uint32 objVertexId = 1;
	obj << "mtllib roadkill.mtl\n\n";

	uint32 cell = 0;
	while( bqs.cells[cell].arenaDescriptorsCount == 0 ) ++cell; // skip empty cells
	uint32 descriptor = 0;
	uint32 location = 0;
	uint32 chunksSize = 0;

	for( uint32 _chunk = 0, _amount = bqs.geometry.size(); _chunk < _amount; ++_chunk )
	{
		const rArenaGeometryChunk& chunk = bqs.geometry[_chunk];
		const rArenaLocation& loc = bqs.locations[location];

//		if( !chunk.vertexData16.empty() )
		for( uint32 i = 0, e = chunk.vertexCount; i < e; ++i )
		{
			float v0 = chunk.vertexData16[i*3+0] /32.f +loc.meshPos[0];
			float v1 = chunk.vertexData16[i*3+1] /32.f +loc.meshPos[1];
			float v2 = chunk.vertexData16[i*3+2] /32.f +loc.meshPos[2];
			// unpopular unofficial obj vertex color extension,
			// that will either do nothing or preserve the vertex lighting
			const uint8* bgra = static_cast<const uint8*>(static_cast<const void*>(&chunk.vcolorsData[i]));
			float r = bgra[2] / 255.f;
			float g = bgra[1] / 255.f;
			float b = bgra[0] / 255.f;
			// x axis is inverted in obj
			PutVertex( vertices, -v0, v1, v2, r, g, b );
		}
/*
		else if( !chunk.vertexData32.empty() )
		for( uint32 i = 0, e = chunk.vertexCount; i < e; ++i )
		{
			float v0 = chunk.vertexData32[i*3+0] /32.f +loc.meshPos[0];
			float v1 = chunk.vertexData32[i*3+1] /32.f +loc.meshPos[1];
			float v2 = chunk.vertexData32[i*3+2] /32.f +loc.meshPos[2];
			// unpopular unofficial obj vertex color extension,
			// that will either do nothing or preserve the vertex lighting
			const uint8* bgra = static_cast<const uint8*>(static_cast<const void*>(&chunk.vcolorsData[i]));
			float r = bgra[2] / 255.f;
			float g = bgra[1] / 255.f;
			float b = bgra[0] / 255.f;
			// x axis is inverted in obj
			PutVertex( vertices, -v0, v1, v2, r, g, b );
		}
*/
		for( uint32 i = 0, e = chunk.uvCount; i < e; ++i )
		{
			float vt0 = chunk.uvData[i*2+0] /4096.f;
			float vt1 = chunk.uvData[i*2+1] /4096.f;
			// vertical axis is inverted in obj
			PutUv( uvvertices, vt0, 1.f - vt1 );
		}

		for( uint32 i = 0, e = chunk.quadsCount; i < e; ++i )
		{
			int f0 = chunk.quadsData[i*4+0] /4 +objVertexId;
			int f1 = chunk.quadsData[i*4+1] /4 +objVertexId;
			int f2 = chunk.quadsData[i*4+2] /4 +objVertexId;
			int f3 = chunk.quadsData[i*4+3] /4 +objVertexId;
			// faces are flipped due to -x axis mirror, inverse winding order
			PutFace2( faces, f0, f2, f1 );
			if( f2 != f3 )
				PutFace2( faces, f0, f3, f2 );
		}
		objVertexId += chunk.headerVertexCount;
		chunksSize += chunk._debugChunkSize;

		const rArenaDescriptor& desc = bqs.cells[cell].descriptors[descriptor];
		if( desc.chunksSize == chunksSize + loc._debugChunkSize )
		{
			obj << "\ng " << cell << "\n";
			// obj material import is case sensitive, while roadkill engine is not
			obj << "usemtl " << MakeUppercase( bqs.textures[desc.textureId].texName ) << "\n";
			obj << vertices.rdbuf();
			obj << uvvertices.rdbuf();
			obj << faces.rdbuf();
			vertices.clear();
			uvvertices.clear();
			faces.clear();
			chunksSize = 0;
			++location;
			if( ++descriptor >= bqs.cells[cell].descriptors.size() )
			{
				do {
					++cell; // skip empty cells
				}
				while( cell < arrsize(bqs.cells) && bqs.cells[cell].arenaDescriptorsCount == 0 );
				descriptor = 0;
			}
		}
	}

	obj << "\n\ns off";

	for( uint32 _cell = 0, _cells = arrsize(bqs.cells); _cell < _cells; ++_cell )
	{
		const std::vector<rArenaCollision>& collisions = bqs.cells[_cell].collision;

		for( uint32 _coll = 0, _colls = collisions.size(); _coll < _colls; ++_coll )
		{
			const rArenaCollision& c = collisions[_coll];

			for( uint32 i = 0, e = c.vertexCount; i < e; ++i )
			{
				float v0 = c.vertexData[i*3+0];
				float v1 = c.vertexData[i*3+1];
				float v2 = c.vertexData[i*3+2];
				// x axis is inverted in obj
				PutVertex( vertices, -v0, v1, v2 );
			}

			for( uint32 i = 0, e = c.trianglesCount; i < e; ++i )
			{
				int f0 = c.triangleData[i*3+0] +objVertexId;
				int f1 = c.triangleData[i*3+1] +objVertexId;
				int f2 = c.triangleData[i*3+2] +objVertexId;
				// faces are flipped due to -x axis mirror, inverse winding order
				PutFace1( faces, f0, f2, f1 );
			}

			objVertexId += c.vertexCount;

			const char symbol[] = "0123456789ABCDEF";
			uint8 cg = c.collisionGroup >> 16;
			obj << "\ng collgroup_" << symbol[cg/16] << symbol[cg%16] << "\n";
			obj << "usemtl collgroup_" << symbol[cg/16] << symbol[cg%16] << "\n";
			obj << vertices.rdbuf();
			obj << faces.rdbuf();
			vertices.clear();
			faces.clear();
		}
	}

	red;
	if( cell < arrsize(bqs.cells) )
		printf( ">OUTPUT ERROR - CHUNK AMOUNT MISMATCH, yes that's my error since I wont see it\n" );
}



//----------------------------------------------------------
template<typename format>
void ParseAndConvert( const std::string& filePath, int infolevel = 0, bool junk = false, bool onlyread = false )
{
	std::ifstream input( filePath.c_str(), std::ios_base::binary );
	if( !input.good() )
	{
		red;
		printf( "> can't open file! make sure path to it is latin\n" );
		return;
	}

	format obj;
	obj.Load( input );
	PrintInfo( obj, infolevel, junk );

	if( input.peek() != std::ifstream::traits_type::eof() )
	{
		yellow;
		printf( "> not whole file is parsed! unknown data at the end!\n" );
	}

	if( onlyread ) return;

	std::ofstream output( (filePath + OutputFormat(obj)).c_str(), OutputTraits(obj) );
	if( !output.good() )
	{
		red;
		printf( "> can't write converted file! make sure path to it is latin\n" );
		return;
	}
	ConvertAndSave( obj, output );
}



//----------------------------------------------------------
void Interface()
{
	const char* help = "\
Spectre: holy crap, I hacked Roadkill\n\
HaDDayn: I think I contributed enough to call it \"we\"\n\
Spectre: of course, but still, holy shit\n\
\n\
to contact us, use either of those:\n\
https://forum.xentax.com/memberlist.php?mode=viewprofile&u=57990\n\
https://github.com/Spectre-srs/RoadkillParser\n\
\n\
info x - set amount of info to show, can be 0, 1, or 2 (def 0)\n\
junk x - set to show unknown junk from the file, can be 0 or 1 (def 0)\n\
write x - set to write converted data, 0 or 1 (def 1)\n\
smb x - filepath to the roadkill model file, converts into .obj\n\
tex x - filepath to the roadkill texture file, converts into .tga\n\
bqs x - filepath to the roadkill city/arena file, converts into .obj\n\
help - show more hints\n\
\n";
	const char* help2 = "\
those files can be extracted from roadkill archives via RavioliGameTools_v2.8,\n\
filepath can be both local or global,\n\
output is saved next to the input file with new extension appended to the name,\n\
convert can be done automatically by drag'n'dropping files onto the executable,\n\
but don't drop too many files at once, or commandline will choke\n\
\n\
roadkill.mtl can be used to automatically apply converted textures to the obj\n\
if arena is too big for your 3d editor, try importing it by small chunks\n\
vertex color is output via non-standard but existing obj extension:\n\
v X Y Z R G B\n\
//by the way arenas from roadkill demo really have no collision group info\n\
\n\
if you see any red text, that means you're reading a wrong or broken file\n\
and if you see yellow, that means there is still left something to investigate,\n\
some flags and little data chunks whose purpose wasn't deduced, the tool\n\
outputs them automatically for maybe to pick your interest for solving\n\
\n\
also, you can't quit this app, it will work on you forever\n";

	printf( help );
	unsigned int infolevel = 0; // unsigned to disallow negative values by definition
	bool junk = false;
	bool onlyread = false;

	while( true )
	{
		std::string cmd, param;
		{
			bright;
			std::string command;
			getline( std::cin, command );

			int idx = command.find_first_of( ' ' );
			cmd = command.substr( 0, idx );
			cmd = MakeUppercase( cmd );
			param = command.substr( idx + 1 );
		}

		if( cmd == "HELP" )
		{
			printf( help );
			printf( help2 );
		}
		else if( cmd == "QUIT" )
		{
			printf( "welp,\n" );
			break;
		}
		else if( cmd == "INFO" )
		{
			infolevel = atoi( param.c_str() );
			if( infolevel > 2 ) infolevel = 2;
			const char* msg[3] = { "no info beside errors", "only useful info", "all the info" };
			white;
			printf( "> set to show %s\n", msg[infolevel] );
		}
		else if( cmd == "WRITE" )
		{
			onlyread = !atoi( param.c_str() );
			white;
			printf( "> set to %s\n", onlyread ? "only read" : "save converted file" );
		}
		else if( cmd == "JUNK" )
		{
			junk = !!atoi( param.c_str() );
			white;
			printf( "> set to %s unknown junk\n", junk ? "show" : "hide" );
		}
		else if( cmd == "TEX" )
		{
			ParseAndConvert<rTextureFormat>( param, infolevel, junk, onlyread );
			white;
			printf( "> done\n" );
		}
		else if( cmd == "SMB" )
		{
			ParseAndConvert<rModelFormat>( param, infolevel, junk, onlyread );
			white;
			printf( "> done\n" );
		}
		else if( cmd == "BQS" )
		{
			ParseAndConvert<rArenaFormat>( param, infolevel, junk, onlyread );
			white;
			printf( "> done\n" );
		}
		else
		{
			white;
			printf( "> type HELP if you forgot how to use this program\n" );
		}
	}
};



//----------------------------------------------------------
int main( int argc, char* argv[] )
{
	if( argc > 1 )
	{
		// drag n drop converter mode
		for( int i = 1; i < argc; ++i )
		{
			uint32 len = strlen( argv[i] );
			if( len < 4 ) continue;

			std::string ext( argv[i] + (len - 3), 3 );
			ext = MakeUppercase( ext );

			white;
			if( ext == "TEX" )
			{
				printf( "%s\n", argv[i] );
				ParseAndConvert<rTextureFormat>( argv[i] );
			}
			else if( ext == "SMB" )
			{
				printf( "%s\n", argv[i] );
				ParseAndConvert<rModelFormat>( argv[i] );
			}
			else if( ext == "BQS" )
			{
				printf( "%s\n", argv[i] );
				ParseAndConvert<rArenaFormat>( argv[i] );
			}
		}
		white;
		printf( "> done\n" );
	}
	else
	{
		// manual explore and diag mode
		Interface();
	}

	system(PAUSE);
	return 0;
}
