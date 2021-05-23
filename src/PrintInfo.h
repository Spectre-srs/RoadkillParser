



//----------------------------------------------------------
#define BYTESTRING( param ) BytesToString(&param, sizeof(param)).c_str()
std::string BytesToString( const void* from, uint32 bytes, bool spaces = false )
{
	const char symbol[] = "0123456789ABCDEF";
	const uint8 mod = arrsize(symbol);
	const uint8* data = static_cast<const uint8*>(from);

	std::string string;
	for( uint32 i = 0; i < bytes; ++i )
	{
		string += symbol[ data[i] / mod ];
		string += symbol[ data[i] % mod ];
		if( spaces ) string += ' ';
	}
	return string;
}



//----------------------------------------------------------
std::string BytesToBinary( const void* from, uint32 bytes, bool spaces = false )
{
	const uint8* data = static_cast<const uint8*>(from);

	std::string string;
	for( uint32 i = 0; i < bytes; ++i )
	{
		const uint8 bit = data[i];
		for( uint8 b = 0; b < 8; ++b )
		{
			string += (bit & (128>>b)) ? '1' : '0';
		}
		if( spaces ) string += ' ';
	}
	return string;
}



//======================================================



//----------------------------------------------------------
void PrintInfo( const rTextureHeader& obj, int infolevel, bool junk )
{
	//if any of those true, loading is screwed up most possibly
	{
		red;
		if( obj._x03000000 != 0x03 )
			printf( ">rTextureHeader._x03000000 = x%s / %u\n", BYTESTRING(obj._x03000000), obj._x03000000 );

		if( obj._x00000000 != 0 )
			printf( ">rTextureHeader._x00000000 = x%s / %u\n", BYTESTRING(obj._x00000000), obj._x00000000 );

		if( obj._x00000000_[0] != 0 )
			printf( ">rTextureHeader._x00000000_[0] = x%s / %u\n", BYTESTRING(obj._x00000000_[0]), obj._x00000000_[0] );
		if( obj._x00000000_[1] != 0 )
			printf( ">rTextureHeader._x00000000_[1] = x%s / %u\n", BYTESTRING(obj._x00000000_[1]), obj._x00000000_[1] );
		if( obj._x00000000_[2] != 0 )
			printf( ">rTextureHeader._x00000000_[2] = x%s / %u\n", BYTESTRING(obj._x00000000_[2]), obj._x00000000_[2] );
		if( obj._x00000000_[3] != 0 )
			printf( ">rTextureHeader._x00000000_[3] = x%s / %u\n", BYTESTRING(obj._x00000000_[3]), obj._x00000000_[3] );

		if( obj._x00000008 != 0x08000000 )
			printf( ">rTextureHeader._x00000008 = x%s / %u\n", BYTESTRING(obj._x00000008), obj._x00000008 );

		if( obj._x00000000__[0] != 0 )
			printf( ">rTextureHeader._x00000000__[0] = x%s / %u\n", BYTESTRING(obj._x00000000__[0]), obj._x00000000__[0] );
		if( obj._x00000000__[1] != 0 )
			printf( ">rTextureHeader._x00000000__[1] = x%s / %u\n", BYTESTRING(obj._x00000000__[1]), obj._x00000000__[1] );

		if( obj._x00000000___[0] != 0 )
			printf( ">rTextureHeader._x00000000___[0] = x%s / %u\n", BYTESTRING(obj._x00000000___[0]), obj._x00000000___[0] );
		if( obj._x00000000___[1] != 0 )
			printf( ">rTextureHeader._x00000000___[1] = x%s / %u\n", BYTESTRING(obj._x00000000___[1]), obj._x00000000___[1] );
		if( obj._x00000000___[2] != 0 )
			printf( ">rTextureHeader._x00000000___[2] = x%s / %u\n", BYTESTRING(obj._x00000000___[2]), obj._x00000000___[2] );

		if( obj._x0050 != 0x5000 )
			printf( ">rTextureHeader._x0050 = x%s / %hu\n", BYTESTRING(obj._x0050), obj._x0050 );

		if( obj._x00000008_ != 0x8000000 )
			printf( ">rTextureHeader._x00000008_ = x%s / %u\n", BYTESTRING(obj._x00000008_), obj._x00000008_ );

		if( obj._x00000000____[0] != 0 )
			printf( ">rTextureHeader._x00000000____[0] = x%s / %u\n", BYTESTRING(obj._x00000000____[0]), obj._x00000000____[0] );
		if( obj._x00000000____[1] != 0 )
			printf( ">rTextureHeader._x00000000____[1] = x%s / %u\n", BYTESTRING(obj._x00000000____[1]), obj._x00000000____[1] );
	}

	//if any of those true, they should be investigated
	{
		yellow;
		if( obj.format == 0x0A )
		{
			if( obj._unkFive != 0x50000005 )
				printf( ">rTextureHeader._unkFive = x%s / %u\n", BYTESTRING(obj._unkFive), obj._unkFive );

			if( obj._unkFour != 0x04 )
				printf( ">rTextureHeader._unkFour = x%s / %u\n", BYTESTRING(obj._unkFour), obj._unkFour );
		}
		else
		{
			if( obj._unkFive != 0x50000041 )
				printf( ">rTextureHeader._unkFive = x%s / %u\n", BYTESTRING(obj._unkFive), obj._unkFive );

			if( obj._unkFour != 0x40 )
				printf( ">rTextureHeader._unkFour = x%s / %u\n", BYTESTRING(obj._unkFour), obj._unkFour );
		}

		if( obj._unkSomeSize1 != obj._unkSomeSize +1 )
			printf( ">rTextureHeader._unkSomeSize1 = x%s / %hu\n", BYTESTRING(obj._unkSomeSize1), obj._unkSomeSize1 );

		uint8 target = (obj.format == 0x0A) ? 32 : 16;
		if( obj.imgWidth * obj.imgHeight / obj._unkSomeSize != target )
			printf( ">rTextureHeader._unkSomeSize = x%s / %u\n", BYTESTRING(obj._unkSomeSize), obj._unkSomeSize );
	}

	bright;
	if( obj.format == 0x09 )
	{
		if( infolevel >= 1 )
			printf( "index = 8bit\n" );
	}
	else if( obj.format == 0x06 )
	{
		if( infolevel >= 1 )
			printf( "index = 8bit swizzled\n" );
	}
	else if( obj.format == 0x0A )
	{
		if( infolevel >= 1 )
			printf( "index = 4bit\n" );
	}
	else
	{
		red;
		printf( ">unknown index format = x%s / %u\n", BYTESTRING(obj.format), obj.format );
	}

	bright;
	if( infolevel >= 1 )
	{
		printf( "width  = %u\n", obj.imgWidth );
		printf( "height = %u\n", obj.imgHeight );
	}

	grey;
	if( junk )
		printf( "_unkFlag = x%s / %u\n", BYTESTRING(obj._unkFlag), obj._unkFlag );

	uint32 size = (obj.format == 0x0A) ? 16 : 256;
	if( infolevel >= 2 ) for( uint32 i = 0; i < size; ++i )
		printf( "pallete%3u = x%s\n", i, BYTESTRING(obj.pallete[i]) );
/*
	if( infolevel >= 2 ) {
		bright;
		system(PAUSE);
		grey;
		uint32 resolution = tex.imgWidth * tex.imgHeight;
		if( obj.format == 0x0A ) resolution /= 2;
		printf( "color indices = %s\n", BytesToString(obj.pixels, resolution, true).c_str() );
	}// */

	yellow;
	if( !obj._unkTrailing.empty() )
		printf( "_unkTrailing = x%s\n", BytesToString(&obj._unkTrailing[0], obj._unkTrailing.size() ).c_str() );
}



//----------------------------------------------------------
void PrintInfo( const rTextureFormat& tex, int infolevel, bool junk )
{
	PrintInfo( tex.header, infolevel, junk );
}



//======================================================



//----------------------------------------------------------
void PrintInfo( const rModelBoundBox& obj )
{
	bright;
	printf( "bboxMin %f, %f, %f\n", obj.bboxMin[0], obj.bboxMin[1], obj.bboxMin[2] );
	printf( "bboxMax %f, %f, %f\n", obj.bboxMax[0], obj.bboxMax[1], obj.bboxMax[2] );
}



//----------------------------------------------------------
void PrintInfo( const rChunkPadding& obj, int infolevel )
{
	//if any of those true, loading is screwed up most possibly
	{
		red;
		if( obj._x00000017 != 0x17000000 )
			printf( ">rModelGeometryChunk._x00000017 = x%s / %u\n", BYTESTRING(obj._x00000017), obj._x00000017 );
	}

	bright;
	if( infolevel >= 2 && !obj._x00000000.empty() )
		printf( "rChunkPadding = %u\n", obj._x00000000.size() * 4 );
}



//----------------------------------------------------------
void PrintInfo( const rModelHeader& obj, int infolevel, bool junk )
{
	//if any of those true, loading is screwed up most possibly
	{
		red;
		if( obj.version != 6 && obj.version != 4 )
			printf( ">rModelHeader.version = x%u, I only know of 4 and 6\n",  obj.version );

		//could be a COUNT of collision structs, I've yet to see such a model though
		if( obj.version == 6 && obj._ver6hasCollision > 1 )
			printf( ">rModelHeader._ver6hasCollision = x%s / %u\n", BYTESTRING(obj._ver6hasCollision), obj._ver6hasCollision );
	}

	if( obj.version == 6 )
	{
		if( obj._ver6_unkNotOne != 1 )
		{
			yellow;
			printf( ">rModelHeader._ver6_unkNotOne = x%s / %u\n", BYTESTRING(obj._ver6_unkNotOne), obj._ver6_unkNotOne );
		}
		else if( junk )
		{
			grey;
			printf( "_ver6_unkNotOne = x%s / %u\n", BYTESTRING(obj._ver6_unkNotOne), obj._ver6_unkNotOne );
		}
	}

	bright;
	if( infolevel >= 1 )
		printf( "meshCount = %u\n", obj.meshCount );

	grey;
	if( infolevel >= 2 )
		printf( "texturesCount = %u\n", obj.texturesCount );
}



//----------------------------------------------------------
void PrintInfo( const rModelTexture& obj, int infolevel, bool junk, uint32 id )
{
	//if any of those true, loading is screwed up most possibly
	{
		red;
		if( obj._x06000000 != 6 )
			printf( "%2u]>rModelTexture._x06000000 = x%s / %u\n", id, BYTESTRING(obj._x06000000), obj._x06000000 );
	}

	//if any of those true, they should be investigated
	{
		yellow;
		if( obj._x00000000[0] != 0 )
			printf( "%2u]>rModelTexture._x00000000[0] = x%s / %u\n", id, BYTESTRING(obj._x00000000[0]), obj._x00000000[0] );
		if( obj._x00000000[1] != 0 )
			printf( "%2u]>rModelTexture._x00000000[1] = x%s / %u\n", id, BYTESTRING(obj._x00000000[1]), obj._x00000000[1] );

		//rest of the space after texname filled with zeros, but it's just too long to make sense
		const char* _debugNotName = obj.texName + strlen(obj.texName);
		const uint32 lenght = sizeof(obj.texName) - strlen(obj.texName);
		const char zero[ sizeof(obj.texName) ] = {};
		if( memcmp(_debugNotName, zero, lenght) )
			printf( "%2u]>rModelTexture._debugNotName = x%s\n", id, BytesToString(_debugNotName, lenght).c_str() );
	}

	bright;
	if( infolevel >= 1 || junk )
		printf( "%2u]texName = %s\n", id, obj.texName );

	if( obj.flags != 0 )
	{
		yellow;
		printf( "%2u]rModelTexture[\"%s\"].flags = x%s / %u\n", id, obj.texName, BYTESTRING(obj.flags), obj.flags );
	}
	else if( junk )
	{
		grey;
		printf( "%2u]flags = x%s / %u\n", id, BYTESTRING(obj.flags), obj.flags );
	}
}



//----------------------------------------------------------
void PrintInfo( const rModelCollision& obj, int infolevel, bool junk, int version )
{
	//if any of those true, loading is screwed up most possibly
	{
		red;
		if( version == 6 && memcmp(obj._ver6_constLegacy, "theLegacyCollisionPart", sizeof(obj._ver6_constLegacy)) )
			printf( ">rModelCollision._ver6_constLegacy = x%s / %s\n", BYTESTRING(obj._ver6_constLegacy), obj._ver6_constLegacy );

		//could it be a number of models to follow? i've yet to see model with more than one
		if( obj._x01000000 != 1 )
			printf( ">rModelCollision._x01000000 = x%s / %u\n", BYTESTRING(obj._x01000000), obj._x01000000 );
	}

	grey;
	//rest of the name filled with random junk, though i see some patterns there sometimes, could be data
	if( version == 6 && junk )
		printf( "_ver6_unkNotLegacy = x%s / %s\n", BYTESTRING(obj._ver6_unkNotLegacy), obj._ver6_unkNotLegacy );

	bright;
	if( infolevel >= 1 )
		printf( "colmesh vertexCount = %u\n", obj.vertexCount );

	grey;
	if( infolevel >= 2 ) for( uint32 i = 0; i < obj.vertexCount; ++i )
		printf( "v%d, %f, %f, %f\n", i, obj.vertexData[i*3+0], obj.vertexData[i*3+1], obj.vertexData[i*3+2] );

	bright;
	if( infolevel >= 2 && obj.vertexCount + obj.trianglesCount > 250 )
		system(PAUSE);

	if( infolevel >= 1 )
		printf( "colmesh trianglesCount = %u\n", obj.trianglesCount );

	grey;
	if( infolevel >= 2 ) for( uint32 i = 0; i < obj.trianglesCount; ++i )
		printf( "t%d, %hd, %hd, %hd\n", i, obj.triangleData[i*3+0], obj.triangleData[i*3+1], obj.triangleData[i*3+2] );
}



//----------------------------------------------------------
void PrintInfo( const rModelDescriptor& obj, int infolevel, bool junk )
{
	//if any of those true, loading is screwed up most possibly
	{
		red;
		if( obj._x02000000 != 2 )
			printf( ">rModelDescriptor._x02000000 = x%s / %d\n", BYTESTRING(obj._x02000000), obj._x02000000 );

		if( obj._x02000000_ != 2 )
			printf( ">rModelDescriptor._x02000000_ = x%s / %d\n", BYTESTRING(obj._x02000000_), obj._x02000000_ );

		if( obj._x06000000 != 6 )
			printf( ">rModelDescriptor._x06000000 = x%s / %d\n", BYTESTRING(obj._x06000000), obj._x06000000 );
	}

	white;
	if( infolevel >= 1 || junk )
		printf( "\tmesh %s:\n", obj.meshName );

	if( junk )
	{
		grey;
		//I'm not sure, sometimes I think I see patterns there, could be some data at the end //yes, there was, now it's just junk
		const char* _debugNotName = obj.meshName + strlen(obj.meshName);
		const uint32 lenght = sizeof(obj.meshName) - strlen(obj.meshName);
		printf( "_notMeshName = x%s\n", BytesToString(_debugNotName, lenght).c_str() );
	}

	bright;
	if( infolevel >= 1 )
	{
		printf( "textureId: %hu\n", obj.textureId );

		if( infolevel >= 2 )
			PrintInfo( obj.bbox );

		printf( "vertexCountRelated = %u\n", obj.vertexCountRelated );

		printf( "trianglesCount = %u\n", obj.trianglesCount );

		printf( "chunksSize = %u\n", obj.chunksSize );
	}

	grey;
	if( junk )
		printf( "_unkMoosor = x%s / %u\n", BYTESTRING(obj._unkMoosor), obj._unkMoosor );
}



//----------------------------------------------------------
void PrintInfo( const rModelGeometryChunk& obj, int infolevel, bool junk )
{
	//if any of those true, loading is screwed up most possibly
	{
		red;
		if( obj._x0080016D != 0x6D018000 )
			printf( ">rModelGeometryChunk._x0080016D = x%s / %u\n", BYTESTRING(obj._x0080016D), obj._x0080016D );

		if( obj._xE300030100010180[0] != 0x010300e3 || obj._xE300030100010180[1] != 0x80010100 )
			printf( ">rModelGeometryChunk._xE300030100010180 = x%s\n", BYTESTRING(obj._xE300030100010180) );

		if( obj._x0280 != 0x8002 )
			printf( ">rModelGeometryChunk._x0280 = x%s / %hu\n", BYTESTRING(obj._x0280), obj._x0280 );

		if( obj._x6A != 0x6A )
			printf( ">rModelGeometryChunk._x6A = x%s / %hhu\n", BYTESTRING(obj._x6A), obj._x6A );

		if( obj._x0380 != 0x8003 )
			printf( ">rModelGeometryChunk._x0380 = x%s / %hu\n", BYTESTRING(obj._x0380), obj._x0380 );

		if( obj._x65 != 0x65 )
			printf( ">rModelGeometryChunk._x65 = x%s / %hhu\n", BYTESTRING(obj._x65), obj._x65 );

		if( obj._x04040001 != 0x01000404 )
			printf( ">rModelGeometryChunk._x04040001 = x%s / %u\n", BYTESTRING(obj._x04040001), obj._x04040001 );

		if( obj._xC0 != 0xC0 )
			printf( ">rModelGeometryChunk._xC0 = x%s / %hhu\n", BYTESTRING(obj._xC0), obj._xC0 );

		if( obj._x6E != 0x6E )
			printf( ">rModelGeometryChunk._x6E = x%s / %hhu\n", BYTESTRING(obj._x6E), obj._x6E );
	}

	//if any of those true, they should be investigated
	{
		yellow;
		if( obj.vertexResolution != 0x69 && obj.vertexResolution != 0x68 )
			printf( ">rModelGeometryChunk.vertexResolution = x%s / %hhu\n", BYTESTRING(obj.vertexResolution), obj.vertexResolution );

		//verts and quads do not exceed certain amount, when they do - they are chopped into next chunk
		if( obj.headerVertexCount > 63 )
			printf( ">rModelGeometryChunk.VertexCount = %hu - more than 63!\n", obj.headerVertexCount );

		if( obj.headerQuadsCount > 21 )
			printf( ">rModelGeometryChunk.QuadsCount = %hu - more than 21!\n", obj.headerQuadsCount );

		//all vertex data counts should equal each other
		if( obj.vertexCount != obj.headerVertexCount )
			printf( ">rModelGeometryChunk.vertexCount = %hhu, while headerVertexCount = %hu\n", obj.vertexCount, obj.headerVertexCount );

		if( obj.normalsCount != obj.headerVertexCount )
			printf( ">rModelGeometryChunk.normalsCount = %hhu, while headerVertexCount = %hu\n", obj.normalsCount, obj.headerVertexCount );

		if( obj.uvCount != obj.headerVertexCount )
			printf( ">rModelGeometryChunk.uvCount = %hhu, while headerVertexCount = %hu\n", obj.uvCount, obj.headerVertexCount );

		if( obj.quadsCount != obj.headerQuadsCount )
			printf( ">rModelGeometryChunk.quadsCount = %hhu while headerQuadsCount = %hu\n", obj.quadsCount, obj.headerQuadsCount );

		//i dont even understand why these exist
		if( obj.headerVertex3Count != obj.headerVertexCount *3 +1 )
			printf( ">rModelGeometryChunk.headerVertexCount = %hu while headerVertex3Count %hu\n", obj.headerVertexCount, obj.headerVertex3Count );

		if( obj.vertex3Count != obj.headerVertex3Count )
			printf( ">rModelGeometryChunk.vertex3Count = %hhu while headerVertex3Count %hu\n", obj.vertex3Count, obj.headerVertex3Count );
	}

	bright;
	if( infolevel >= 1 )
	{
		printf( "vertexResolution = %s\n", (obj.vertexResolution == 0x68) ? "32bit" : "16bit" );

		printf( "vertexCount = %hhu\n", obj.vertexCount );
	}

	if( infolevel >= 2 )
	{
		grey;

		if( !obj.vertexData16.empty() ) for( uint8 i = 0; i < obj.vertexCount; ++i )
			printf( "v%02d, %f, %f, %f\n", i, obj.vertexData16[i*3+0]/256.f, obj.vertexData16[i*3+1]/256.f, obj.vertexData16[i*3+2]/256.f );
		else
		if( !obj.vertexData32.empty() ) for( uint8 i = 0; i < obj.vertexCount; ++i )
			printf( "v%02d, %f, %f, %f\n", i, obj.vertexData32[i*3+0]/256.f, obj.vertexData32[i*3+1]/256.f, obj.vertexData32[i*3+2]/256.f );
		else
			printf( "unknown vertex format!\n", obj.vertexCount );

		bright;
		printf( "normalsCount = %hhu\n", obj.normalsCount );

		grey;
		for( uint8 i = 0; i < obj.normalsCount; ++i )
			printf( "vn%02d, %f, %f, %f\n", i, obj.normalsData[i*3+0]/128.f, obj.normalsData[i*3+1]/128.f, obj.normalsData[i*3+2]/128.f );

		bright;
		printf( "uvCount = %hhu\n", obj.uvCount );

		grey;
		for( uint8 i = 0; i < obj.uvCount; ++i )
			printf( "uv%02d, %f, %f\n", i, obj.uvData[i*2+0]/4096.f, obj.uvData[i*2+1]/4096.f );
	}

	bright;
	if( infolevel >= 1 )
		printf( "quadsCount = %hhu\n", obj.quadsCount );

	grey;
	if( infolevel >= 2 ) for( uint8 i = 0; i < obj.quadsCount; ++i )
		printf( "q%02d, %hhu, %hhu, %hhu, %hhu\n", i, obj.quadsData[i*4+0], obj.quadsData[i*4+1], obj.quadsData[i*4+2], obj.quadsData[i*4+3] );

	if( infolevel >= 1 )
		printf( "_debugChunkSize = %u\n", obj._debugChunkSize );

	PrintInfo( obj.padding, infolevel );
}



//----------------------------------------------------------
void PrintInfo( const rModelFormat& smb, int infolevel, bool junk )
{
	PrintInfo( smb.header, infolevel, junk );

	for( uint32 i = 0, e = smb.textures.size(); i < e; ++i )
		PrintInfo( smb.textures[i], infolevel, junk, i );

	for( uint32 i = 0, e = smb.collision.size(); i < e; ++i )
		PrintInfo( smb.collision[i], infolevel, junk, smb.header.version );

	if( infolevel >= 1 )
		PrintInfo( smb.boundbox );

	for( uint32 i = 0, e = smb.descriptors.size(); i < e; ++i )
		PrintInfo( smb.descriptors[i], infolevel, junk );

	uint32 descriptor = 0;
	uint32 size = 0;
	for( uint32 i = 0, e = smb.chunks.size(); i < e; ++i )
	{
		if( infolevel >= 1 )
		{
			if( infolevel >= 2 || size == 0 )
			{
				bright;
				system(PAUSE);
				if( infolevel >= 2 )
					system(CLS);
			}
			else
			{
				bright;
				printf( "----\n" );
			}
		}
		if( size == 0 && infolevel >= 1 )
		{
			white;
			printf( "\tmesh chunks for %s:\n", smb.descriptors[descriptor].meshName );
		}
		size += smb.chunks[i]._debugChunkSize;

		PrintInfo( smb.chunks[i], infolevel, junk );

		if( smb.descriptors[descriptor].chunksSize == size )
		{
			if( infolevel >= 2 )
			{
				white;
				printf( "----\n\tend of %s submesh\n\n", smb.descriptors[descriptor].meshName );
			}
			++descriptor;
			size = 0;
		}
	}

	red;
	if( descriptor != smb.descriptors.size() )
		printf( "mesh descriptors and mesh data amount mismatch!\n" );
	if( size != 0 )
		printf( "not all data fully forms their meshes!\n" );
}



//======================================================



//----------------------------------------------------------
void PrintInfo( const rArenaHeader& obj, int infolevel, bool junk )
{
	//if any of those true, loading is screwed up most possibly
	{
		red;
		if( obj._x05000000 != 5 )
			printf( ">rArenaHeader._x05000000 = x%s / %u\n", BYTESTRING(obj._x05000000), obj._x05000000 );

		if( obj._x06000000 != 6 )
			printf( ">rArenaHeader._x06000000 = x%s / %u\n", BYTESTRING(obj._x06000000), obj._x06000000 );
	}

	if( infolevel >= 1 )
	{
		bright;
		printf( "sunDir = %f, %f, %f\n", obj.sunDir[0], obj.sunDir[1], obj.sunDir[2] );

		printf( "sunColor = %f, %f, %f\n", obj.sunColor[0], obj.sunColor[1], obj.sunColor[2] );

		printf( "fogColor = %f, %f, %f\n", obj.fogColor[0], obj.fogColor[1], obj.fogColor[2] );

		printf( "fogDistance = %f\n", obj.fogDistance );

		printf( "texturesCount = %u\n", obj.texturesCount );
	}
}



//----------------------------------------------------------
void PrintInfo( const rArenaTexture& obj, int infolevel, bool junk, uint32 id )
{
	//if any of those true, loading is screwed up most possibly
	{
		red;
		if( obj._x00000000[0] != 0 )
			printf( "%2u] >rArenaTexture._x00000000[0] = x%s / %u\n", id, BYTESTRING(obj._x00000000[0]), obj._x00000000[0] );
		if( obj._x00000000[1] != 0 )
			printf( "%2u] >rArenaTexture._x00000000[1] = x%s / %u\n", id, BYTESTRING(obj._x00000000[1]), obj._x00000000[1] );
	}

	//if any of those true, they should be investigated
	{
		yellow;
		if( obj.flags != 0 && obj.flags != 8 )
			printf( "%2u] >rArenaTexture[\"%s\"].flags = x%s / %u\n", id, obj.texName, BYTESTRING(obj.flags), obj.flags );

		//rest of the space after texname filled with zeros, but it just too long to make sense
		const char* _debugNotName = obj.texName + strlen(obj.texName);
		const uint32 lenght = sizeof(obj.texName) - strlen(obj.texName);
		const char zero[ sizeof(obj.texName) ] = {};
		if( memcmp(_debugNotName, zero, lenght) )
			printf( "%2u] >rArenaTexture._debugNotName = x%s\n", id, BytesToString(_debugNotName, lenght).c_str() );
	}

	if( infolevel >= 1 )
	{
		bright;
		printf( "%2u] %s", id, obj.texName );
		white;
		printf( obj.flags == 8 ? ", has alpha channel\n" : "\n" );
	}
}



//----------------------------------------------------------
void PrintInfo( const rArenaDescriptor& obj, int infolevel, bool junk, uint32 id )
{
	//if any of those true, loading is screwed up most possibly
	{
		red;
		if( obj._x02000000 != 2 )
			printf( "%2u] >rArenaDescriptor._x02000000 = x%s / %d\n", id, BYTESTRING(obj._x02000000), obj._x02000000 );

		if( obj._x02000000_ != 2 )
			printf( "%2u] >rArenaDescriptor._x02000000_ = x%s / %d\n", id, BYTESTRING(obj._x02000000_), obj._x02000000_ );

		if( obj._x06000000 != 6 )
			printf( "%2u] >rArenaDescriptor._x06000000 = x%s / %d\n", id, BYTESTRING(obj._x06000000), obj._x06000000 );
	}

	if( infolevel == 0 && !junk )
		return;

	if( infolevel == 1 && !junk )
	{
		bright;
		printf( "%2u]textureId = %3u; chunksSize = %3u; trianglesCount = %3u\n",
			id, obj.textureId, obj.chunksSize, obj.trianglesCount );
		return;
	}

	white;
	printf( "%2u]     descriptor :\n", id );

	bright;
	if( infolevel >= 1 )
	{
		printf( "textureId: %u\n", obj.textureId );

		if( infolevel >= 2 )
			PrintInfo( obj.bbox );

		printf( "vertexCountRelated = %u\n", obj.vertexCountRelated );

		printf( "trianglesCount = %u\n", obj.trianglesCount );

		printf( "chunksSize = %u\n", obj.chunksSize );
	}

	grey;
	if( junk )
	{
		printf( "_unkSomeFlag = x%s / %s\n", id, BYTESTRING(obj._unkSomeFlag), BytesToBinary(&obj._unkSomeFlag, 1).c_str() );

		printf( "_unkJunk = x%s / %u\n", BYTESTRING(obj._unkJunk), obj._unkJunk );
	}
}



//----------------------------------------------------------
void PrintInfo( const rArenaCollision& obj, int infolevel, bool junk, uint32 id )
{
	//if any of those true, they should be investigated
	{
		yellow;
		if( (obj.collisionGroup & ~0x00FF0000) != 0 )
			printf( "%2u] >rArenaCollision.collisionGroup = x%s / %u\n", id, BYTESTRING(obj.collisionGroup), obj.collisionGroup );
	}

	const uint8 cg = obj.collisionGroup >> 16;

	if( infolevel == 0 )
		return;

	if( infolevel == 1 )
	{
		bright;
		printf( "%2u]vertexCount = %3u; trianglesCount = %3u; collGroup = 0x%s\n",
			id, obj.vertexCount, obj.trianglesCount, BYTESTRING(cg) );
		return;
	}

	white;
	printf( "%2u]     collision :\n", id );

	bright;
	printf( "vertexCount = %u\n", obj.vertexCount );

	grey;
	for( uint32 i = 0; i < obj.vertexCount; ++i )
		printf( "v%u, %f, %f, %f\n", i, obj.vertexData[i*3+0], obj.vertexData[i*3+1], obj.vertexData[i*3+2] );

	bright;
	if( obj.vertexCount + obj.trianglesCount > 250 )
		system(PAUSE);

	printf( "trianglesCount = %u\n", obj.trianglesCount );

	grey;
	for( uint32 i = 0; i < obj.trianglesCount; ++i )
		printf( "t%u, %hu, %hu, %hu\n", i, obj.triangleData[i*3+0], obj.triangleData[i*3+1], obj.triangleData[i*3+2] );

	bright;
	printf( "collisionGroup = 0x%s\n", BYTESTRING(cg) );
}



//----------------------------------------------------------
void PrintInfo( const rMapTile& obj, int infolevel, bool junk, uint32 id )
{
	bool hasData = (obj.arenaDescriptorsCount + obj.collisionChunksCount) > 0;

	if( junk || (hasData && infolevel >= 1) )
	{
		white;
		printf( "%3u]    cell pos: x%i y%i\n", id, (id%32), (id/32) );
	}

	bright;
	if( hasData && infolevel >= 1 )
		printf( "arenaDescriptorsCount = %u\n", obj.arenaDescriptorsCount );

	uint32 printLines = (infolevel >= 1 || junk) ? 20 : 0;
	for( uint32 i = 0, e = obj.descriptors.size(); i < e; ++i )
	{
		PrintInfo( obj.descriptors[i], infolevel, junk, i );

		if( printLines > 0 && i % printLines == printLines - 1 && i != e - 1 ) {
			bright;
			printf( "%u out of %u descriptors, ", i+1, e );
			system(PAUSE);
		}
	}

	bright;
	if( hasData && infolevel >= 1 )
	{
		if( obj.arenaDescriptorsCount > 0 )
			printf( "\n" );
		printf( "collisionChunksCount = %u\n", obj.collisionChunksCount );
	}

	printLines = 0;
	if( infolevel == 1 ) printLines = 250 + 48;
	if( infolevel >= 2 ) printLines = 1;
	for( uint32 i = 0, e = obj.collision.size(); i < e; ++i )
	{
		PrintInfo( obj.collision[i], infolevel, junk, i );

		if( printLines > 0 && i % printLines == printLines - 1 && i != e - 1 )
		{
			bright;
			printf( "%u out of %u collision chunks, ", i+1, e );
			system(PAUSE);
		}
	}

	if( infolevel >= 1 )
	{
		grey;
		printf( "geometryDataAddress = x%x\n", obj.geometryDataAddress );
		printf( "geometryDataSize = %u\n", obj.geometryDataSize );
	}

	white;
	if( hasData && (junk || infolevel >= 1) )
	{
		printf( "----\n\tend of cell %u\n", id );
		system(PAUSE);
		printf( "\n\n" );
	}
}



//----------------------------------------------------------
void PrintInfo( const rArenaLocation& obj, int infolevel, bool junk, uint32 id )
{
	//if any of those true, loading is screwed up most possibly
	{
		red;
		if( obj._x0080026C != 0x6C028000 )
			printf( "%3u]>rArenaLocation._x0080026C = x%s / %u\n", id, BYTESTRING(obj._x0080026C), obj._x0080026C );

		if( obj._x00000000[0] != 0 || obj._x00000000[1] != 0 || obj._x00000000[2] != 0 || obj._x00000000[3] != 0 )
			printf( "%3u]>rArenaLocation._x00000000 = x%s\n", id, BYTESTRING(obj._x00000000) );

		if( obj._x0280 != 0x8002 )
			printf( "%3u]>rArenaLocation._x0280 = x%s / %hu\n", id, BYTESTRING(obj._x0280), obj._x0280 );

		if( obj._x6D != 0x6d )
			printf( "%3u]>rArenaLocation._x6D = x%s / %hhu\n", id, BYTESTRING(obj._x6D), obj._x6D );
	}

	//if any of those true, they should be investigated
	{
		yellow;
		if( obj._constMeshScale != 1.0f )
			printf( "%3u]>rArenaLocation._constMeshScale = x%s / %f\n", id, BYTESTRING(obj._constMeshScale), obj._constMeshScale );

		if( obj._const_unkDataCount != 8 )
		{
			printf( "%3u]>rArenaLocation._const_unkDataCount = x%s / %f\n", id, BYTESTRING(obj._const_unkDataCount), obj._const_unkDataCount );
			junk = true;
		}

		struct lyambda98 {
			bool operator() ( uint16 a, uint16 b, uint16 c, uint16 d )
			{ return a == b && b == c && c == d; }
		} eq;
		if( !eq(obj._unkData[0*4+0], obj._unkData[2*4+0], obj._unkData[4*4+0], obj._unkData[6*4+0]) ||
			!eq(obj._unkData[1*4+0], obj._unkData[3*4+0], obj._unkData[5*4+0], obj._unkData[7*4+0]) ||
			!eq(obj._unkData[0*4+1], obj._unkData[1*4+1], obj._unkData[4*4+1], obj._unkData[5*4+1]) ||
			!eq(obj._unkData[2*4+1], obj._unkData[3*4+1], obj._unkData[6*4+1], obj._unkData[7*4+1]) ||
			!eq(obj._unkData[0*4+2], obj._unkData[1*4+2], obj._unkData[2*4+2], obj._unkData[3*4+2]) ||
			!eq(obj._unkData[4*4+2], obj._unkData[5*4+2], obj._unkData[6*4+2], obj._unkData[7*4+2]) ||
			!eq(obj._unkData[0*4+3], obj._unkData[1*4+3], obj._unkData[2*4+3], obj._unkData[3*4+3]) ||
			!eq(obj._unkData[4*4+3], obj._unkData[5*4+3], obj._unkData[6*4+3], obj._unkData[7*4+3]) ||
			obj._unkData[0*4+3] != obj._unkData[4*4+3] )
		{
			printf( "%3u]>rArenaLocation._unkData pattern mismatch!\n" );
			junk = true;
		}

		if( obj._debugChunkSize != 108 )
			printf( "%3u]>rArenaLocation._debugChunkSize = %u\n", obj._debugChunkSize );

		PrintInfo( obj.padding, infolevel );
	}

	bright;
	if( infolevel >= 2 )
		printf( "meshPos = %f, %f, %f\n", obj.meshPos[0], obj.meshPos[1], obj.meshPos[2] );

	if( junk )
	{
		printf( "_const_unkDataCount = %hhu\n", obj._const_unkDataCount );

		grey;
		for( int i = 0; i < obj._const_unkDataCount; ++i )
			printf( "%hu, %s, %s, %s, %s  /  %hi, %hi, %hi, %hi\n", i, BYTESTRING(obj._unkData[i*4+0]), BYTESTRING(obj._unkData[i*4+1]), BYTESTRING(obj._unkData[i*4+2]), BYTESTRING(obj._unkData[i*4+3]),
			obj._unkData[i*4+0], obj._unkData[i*4+1], obj._unkData[i*4+2], obj._unkData[i*4+3] );
	}
}



//----------------------------------------------------------
void PrintInfo( const rArenaGeometryChunk& obj, int infolevel, bool junk, uint32 id )
{
	//if any of those true, loading is screwed up most possibly
	{
		red;
		if( obj._x0080016D != 0x6D018000 )
			printf( "%2u] >rArenaGeometryChunk._x0080016D = x%s / %u\n", id, BYTESTRING(obj._x0080016D), obj._x0080016D );

		if( obj._xE300040100010180[0] != 0x010400e3 || obj._xE300040100010180[1] != 0x80010100 )
			printf( "%2u] >rArenaGeometryChunk._obogoid = x%s\n", id, BYTESTRING(obj._xE300040100010180) );

		if( obj._x02C0 != 0xc002 )
			printf( "%2u] >rArenaGeometryChunk._x02C0 = x%s / %hu\n", id, BYTESTRING(obj._x02C0), obj._x02C0 );

		if( obj._x6E != 0x6e )
			printf( "%2u] >rArenaGeometryChunk._x6E = x%s / %hhu\n", id, BYTESTRING(obj._x6E), obj._x6E );

		if( obj._x0380 != 0x8003 )
			printf( "%2u] >rArenaGeometryChunk._x0380 = x%s / %hu\n", id, BYTESTRING(obj._x0380), obj._x0380 );

		if( obj._x65 != 0x65 )
			printf( "%2u] >rArenaGeometryChunk._x65 = x%s / %hhu\n", id, BYTESTRING(obj._x65), obj._x65 );

		if( obj._x04040001 != 0x01000404 )
			printf( "%2u] >rArenaGeometryChunk._x04040001 = x%s / %u\n", id, BYTESTRING(obj._x04040001), obj._x04040001 );

		if( obj._xC0 != 0xC0 )
			printf( "%2u] >rArenaGeometryChunk._xC0 = x%s / %hhu\n", id, BYTESTRING(obj._xC0), obj._xC0 );

		if( obj._x6E_ != 0x6E )
			printf( "%2u] >rArenaGeometryChunk._x6E_ = x%s / %hhu\n", id, BYTESTRING(obj._x6E_), obj._x6E_ );
	}

	//if any of those true, they should be investigated
	{
		yellow;
		if( obj.vertexResolution != 0x69 && obj.vertexResolution != 0x68 )
			printf( "%2u] >rArenaGeometryChunk.vertexResolution = x%s / %hhu\n", id, BYTESTRING(obj.vertexResolution), obj.vertexResolution );

		//never seen an arena use full 32 bit, they all use 16 with /32.f precision
		if( obj.vertexResolution == 0x68 )
			printf( "%2u] >rArenaGeometryChunk.vertexResolution = 32 bit!" );

		//verts and quads do not exceed certain amount, when they do - they are chopped into next chunk
		if( obj.headerVertexCount > 63 )
			printf( "%2u] >rArenaGeometryChunk.VertexCount = %hu - more than 63!\n", id, obj.headerVertexCount );

		if( obj.headerQuadsCount > 21 )
			printf( "%2u] >rArenaGeometryChunk.QuadsCount = %hu - more than 21!\n", id, obj.headerQuadsCount );

		//all vertex data counts should equal each other
		if( obj.vertexCount != obj.headerVertexCount )
			printf( "%2u] >rArenaGeometryChunk.vertexCount = %hhu, while headerVertexCount = %hu\n", id, obj.vertexCount, obj.headerVertexCount );

		if( obj.vcolorsCount != obj.headerVertexCount )
			printf( "%2u] >rArenaGeometryChunk.vcolorsCount = %hhu, while headerVertexCount = %hu\n", id, obj.vcolorsCount, obj.headerVertexCount );

		if( obj.uvCount != obj.headerVertexCount )
			printf( "%2u] >rArenaGeometryChunk.uvCount = %hhu, while headerVertexCount = %hu\n", id, obj.uvCount, obj.headerVertexCount );

		if( obj.quadsCount != obj.headerQuadsCount )
			printf( "%2u] >rArenaGeometryChunk.quadsCount = %hhu while headerQuadsCount = %hu\n", id, obj.quadsCount, obj.headerQuadsCount );

		//i dont even understand why these exist
		if( obj.headerVertex4Count != obj.headerVertexCount *4 +1 )
			printf( "%2u] >rArenaGeometryChunk.headerVertexCount = %hu while headerVertex4Count %hu\n", id, obj.headerVertexCount, obj.headerVertex4Count );

		if( obj.vertex4Count != obj.headerVertex4Count )
			printf( "%2u] >rArenaGeometryChunk.vertex4Count = %hhu while headerVertex4Count %hu\n", id, obj.vertex4Count, obj.headerVertex4Count );
	}

	if( infolevel == 0 )
		return;

	if( infolevel == 1 )
	{
		bright;
		printf( "%3u]vertexCount = %2hhu; quadsCount = %2hhu; chunkSize = %u\n",
			id, obj.vertexCount, obj.quadsCount, obj._debugChunkSize );
		return;
	}

	bright;
	printf( "\n%3u] rArenaGeometryChunk:\n", id );

//	printf( "vertexResolution = %s\n", (obj.vertexResolution == 0x68) ? "32bit" : "16bit" );

	printf( "vertexCount = %hhu\n", obj.vertexCount );

	grey;
	if( !obj.vertexData16.empty() ) for( uint8 i = 0; i < obj.vertexCount; ++i )
		printf( "v%02d, %f, %f, %f\n", i, obj.vertexData16[i*3+0]/32.f, obj.vertexData16[i*3+1]/32.f, obj.vertexData16[i*3+2]/32.f );
//	else
//	if( !obj.vertexData32.empty() ) for( uint8 i = 0; i < obj.vertexCount; ++i )
//		printf( "v%02d, %f, %f, %f\n", i, obj.vertexData32[i*3+0]/32.f, obj.vertexData32[i*3+1]/32.f, obj.vertexData32[i*3+2]/32.f );
//	else
//		printf( "unknown vertex format!\n" );

	bright;
	printf( "vcolorsCount = %hhu\n", obj.vcolorsCount );

	grey;
	for( uint8 i = 0; i < obj.vcolorsCount; ++i )
		printf( "vc%02d, %s\n", i, BYTESTRING(obj.vcolorsData[i]) );

	bright;
	printf( "uvCount = %hhu\n", obj.uvCount );

	grey;
	for( uint8 i = 0; i < obj.uvCount; ++i )
		printf( "uv%02d, %f, %f\n", i, obj.uvData[i*2+0]/4096.f, obj.uvData[i*2+1]/4096.f );

	bright;
	printf( "quadsCount = %hhu\n", obj.quadsCount );

	grey;
	for( uint8 i = 0; i < obj.quadsCount; ++i )
	printf( "q%02d, %hhu, %hhu, %hhu, %hhu\n", i, obj.quadsData[i*4+0], obj.quadsData[i*4+1], obj.quadsData[i*4+2], obj.quadsData[i*4+3] );

	bright;
	printf( "_debugChunkSize = %u\n", obj._debugChunkSize );

	PrintInfo( obj.padding, infolevel );
}



//----------------------------------------------------------
void PrintInfo( rArenaFormat& bqs, int infolevel, bool junk )
{
	PrintInfo( bqs.header, infolevel, junk );

	uint32 printLines = 250;
	for( uint32 i = 0; i < bqs.header.texturesCount; ++i )
	{
		PrintInfo( bqs.textures[i], infolevel, junk, i );

		if( i == printLines && infolevel >= 1 )
		{
			bright;
			system(PAUSE);
		}
	}

	if( infolevel >= 1 || junk )
	{
		bright;
		printf( "\nmap cells next, " );
		system(PAUSE);
	}

	printLines = junk ? 32 : 0;
	for( uint32 i = 0, e = arrsize(bqs.cells); i < e; ++i )
	{
		PrintInfo( bqs.cells[i], infolevel, junk, i );

		if( printLines > 0 && i % printLines == printLines - 1 && i != e - 1 )
		{
			system(PAUSE);
			printf( "\n" );
		}
	}

	if( infolevel >= 1 || junk )
	{
		bright;
		printf( "\ngeometry chunks next, " );
		system(PAUSE);
	}

	printLines = (infolevel == 1) ? 250 : 0;
	uint32 lines = 0;
	for( uint32 i = 0, e = bqs.geometry.size(); i < e; ++i )
	{
		if( infolevel == 2 )
		{
			uint32 newLines = bqs.geometry[i].vertexCount *3 + bqs.geometry[i].quadsCount + 9;
			if( (lines += newLines) > 280 )
			{
				lines = newLines;
				white;
				printf( "\n%u out of %u model chunks, \n", i+1, e );
				system(PAUSE);
			}
		}

		PrintInfo( bqs.geometry[i], infolevel, junk, i );

		if( printLines > 0 && i % printLines == printLines - 1 && i != e - 1 )
		{
			white;
			printf( "\n%u out of %u model chunks, \n", i+1, e );
			system(PAUSE);
		}
	}

	//loops validating data coherence

	for( uint32 cellId = 0, locId = 0; cellId < arrsize(bqs.cells); ++cellId )
	{
		const rMapTile& cell = bqs.cells[cellId];

		if( cell.arenaDescriptorsCount == 0 )
			continue;

		if( locId >= bqs.locations.size() )
		{
			red;
			printf( "locId = %u, while locations.size = %u!\n", locId, bqs.locations.size() );
			break;
		}
		const uint32 dataStartAddress = bqs.locations[locId]._debugChunkAddress;

		uint32 desksize = 0;
		for( int i = 0, e = cell.arenaDescriptorsCount; i < e; ++i )
		{
			desksize += cell.descriptors[i].chunksSize;
			++locId;
		}

		yellow;
		if( cell.geometryDataAddress != dataStartAddress )
			printf( "> %u]cell.geometryDataAddress = x%x, but found at = x%x\n", 
				cellId, cell.geometryDataAddress, dataStartAddress );

		if( cell.geometryDataSize != desksize )
			printf( "> %u]cell.geometryDataSize = %u, while actual data size was = %u\n", cellId, cell.geometryDataSize, desksize );

		if( (infolevel >= 1 || junk) && locId%250 == 0 )
		{
			bright;
			system(PAUSE);
		}
	}

	const float nan = 1.0e+30f;
	yellow;

	for( uint32 i = 0; i < arrsize(bqs.cells); ++i )
	{
		rMapTile& c = bqs.cells[i];
		rArenaFormat::FloatPair& k = bqs.heights[i];

		if( k.min == nan && k.max == -nan && c.arenaDescriptorsCount == 0 )
		{
			//everything okay
		}
		else if( k.min != nan && k.max != -nan && c.arenaDescriptorsCount && k.min <= k.max )
		{
			//everything also okay
		}
		else
		{
			//pattern is broken
			printf( "%2i]cell.descriptors=%u\theights.min = %f  heights.max = %f\n", i, c.arenaDescriptorsCount, k.min, k.max );
		}
	}

	uint32 cell = 0;
	while( bqs.cells[cell].arenaDescriptorsCount == 0 ) ++cell;
	uint32 descriptor = 0;
	uint32 descriptorAll = 0;
	uint32 chunksSize = 0;
	float min = nan;
	float max = -nan;
	const float errorThreshold = 0.01601f; //derived from the data itself, all except one values fit within it

	for( uint32 _chunk = 0, _chunks = bqs.geometry.size(); _chunk < _chunks; ++_chunk )
	{
		const rArenaGeometryChunk& chunk = bqs.geometry[_chunk];
		const rArenaLocation& loc = bqs.locations[descriptorAll];
		const rArenaDescriptor& desc = bqs.cells[cell].descriptors[descriptor];
		chunksSize += chunk._debugChunkSize;

		for( uint32 i = 0, e = chunk.vertexCount; i < e; ++i )
		{
			float v1 = chunk.vertexData16[i*3+1] /32.f +loc.meshPos[1];
			if( min > v1 ) min = v1;
			if( max < v1 ) max = v1;
		}

		if( desc.chunksSize == chunksSize + loc._debugChunkSize )
		{
			chunksSize = 0;
			++descriptorAll;
			if( ++descriptor >= bqs.cells[cell].descriptors.size() )
			{
				float diff = min - bqs.heights[cell].min;
				if( diff < 0 ) diff = -diff;
				if( diff > errorThreshold )
					printf( "%2i]heights.min = %f, geom lower bound = %f, diff > error margin\n",
						cell, bqs.heights[cell].min, min );

				diff = max - bqs.heights[cell].max;
				if( diff < 0 ) diff = -diff;
				if( diff > errorThreshold )
					printf( "%2i]heights.max = %f, geom upper bound = %f, diff > error margin\n",
						cell, bqs.heights[cell].max, max );

				descriptor = 0;
				min = nan;
				max = -nan;
				do
				{
					++cell;
				}
				while( cell < arrsize(bqs.cells) && bqs.cells[cell].arenaDescriptorsCount == 0 );
			}
		}
	}
}
