#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library

#include "MilkshapeModel.h"

#include <fstream.h>

MilkshapeModel::MilkshapeModel()
{
}

MilkshapeModel::~MilkshapeModel()
{
}

/* 
	MS3D STRUCTURES 
*/

// byte-align structures
#ifdef _MSC_VER
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#	define PATH_MAX _MAX_PATH
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#	include <limits.h>
#else
#	error you must byte-align these structures with the appropriate compiler directives
#endif

typedef unsigned char byte;
typedef unsigned short word;

// File header
struct MS3DHeader
{
	char m_ID[10];
	int m_version;
} PACK_STRUCT;

// Vertex information
struct MS3DVertex
{
	byte m_flags;
	float m_vertex[3];
	char m_boneID;
	byte m_refCount;
} PACK_STRUCT;

// Triangle information
struct MS3DTriangle
{
	word m_flags;
	word m_vertexIndices[3];
	float m_vertexNormals[3][3];
	float m_s[3], m_t[3];
	byte m_smoothingGroup;
	byte m_groupIndex;
} PACK_STRUCT;

// Material information
struct MS3DMaterial
{
    char m_name[32];
    float m_ambient[4];
    float m_diffuse[4];
    float m_specular[4];
    float m_emissive[4];
    float m_shininess;	// 0.0f - 128.0f
    float m_transparency;	// 0.0f - 1.0f
    byte m_mode;	// 0, 1, 2 is unused now
    char m_texture[128];
    char m_alphamap[128];
} PACK_STRUCT;

//	Joint information
struct MS3DJoint
{
	byte m_flags;
	char m_name[32];
	char m_parentName[32];
	float m_rotation[3];
	float m_translation[3];
	word m_numRotationKeyframes;
	word m_numTranslationKeyframes;
} PACK_STRUCT;

// Keyframe data
struct MS3DKeyframe
{
	float m_time;
	float m_parameter[3];
} PACK_STRUCT;

// Default alignment
#ifdef _MSC_VER
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT

bool MilkshapeModel::loadModelData( const char *filename )
{
	
	ifstream inputFile( filename, ios::in | ios::binary | ios::nocreate );
	if ( inputFile.fail())
		return false;	// "Couldn't open the model file."

	char pathTemp[PATH_MAX+1];
	int pathLength;
	for ( pathLength = strlen( filename ); pathLength--; ) {
		if ( filename[pathLength] == '/' || filename[pathLength] == '\\' ) {
			break;
		}
	}
	strncpy( pathTemp, filename, pathLength );

	inputFile.seekg( 0, ios::end );
	long fileSize = inputFile.tellg();
	inputFile.seekg( 0, ios::beg );

	byte *pBuffer = new byte[fileSize];
	inputFile.read( pBuffer, fileSize );
	inputFile.close();

	const byte *pPtr = pBuffer;
	MS3DHeader *pHeader = ( MS3DHeader* )pPtr;
	pPtr += sizeof( MS3DHeader );

	if ( strncmp( pHeader->m_ID, "MS3D000000", 10 ) != 0 )
		return false; // "Not a valid Milkshape3D model file."

	if ( pHeader->m_version < 3 || pHeader->m_version > 4 )
		return false; // "Unhandled file version. Only Milkshape3D Version 1.3 and 1.4 is supported." );

	int nVertices = *( word* )pPtr; 
	m_numVertices = nVertices;
	m_pVertices = new Vertex[nVertices];
	pPtr += sizeof( word );

	int i;
	for ( i = 0; i < nVertices; i++ )
	{
		MS3DVertex *pVertex = ( MS3DVertex* )pPtr;
		m_pVertices[i].m_boneID = pVertex->m_boneID;
		memcpy( m_pVertices[i].m_location, pVertex->m_vertex, sizeof( float )*3 );
		pPtr += sizeof( MS3DVertex );
	}

	int nTriangles = *( word* )pPtr;
	m_numTriangles = nTriangles;
	m_pTriangles = new Triangle[nTriangles];
	pPtr += sizeof( word );

	for ( i = 0; i < nTriangles; i++ )
	{
		MS3DTriangle *pTriangle = ( MS3DTriangle* )pPtr;
		int vertexIndices[3] = { pTriangle->m_vertexIndices[0], pTriangle->m_vertexIndices[1], pTriangle->m_vertexIndices[2] };
		float t[3] = { 1.0f-pTriangle->m_t[0], 1.0f-pTriangle->m_t[1], 1.0f-pTriangle->m_t[2] };
		memcpy( m_pTriangles[i].m_vertexNormals, pTriangle->m_vertexNormals, sizeof( float )*3*3 );
		memcpy( m_pTriangles[i].m_s, pTriangle->m_s, sizeof( float )*3 );
		memcpy( m_pTriangles[i].m_t, t, sizeof( float )*3 );
		memcpy( m_pTriangles[i].m_vertexIndices, vertexIndices, sizeof( int )*3 );
		pPtr += sizeof( MS3DTriangle );
	}

	int nGroups = *( word* )pPtr;
	m_numMeshes = nGroups;
	m_pMeshes = new Mesh[nGroups];
	pPtr += sizeof( word );
	for ( i = 0; i < nGroups; i++ )
	{
		pPtr += sizeof( byte );	// flags
		pPtr += 32;				// name

		word nTriangles = *( word* )pPtr;
		pPtr += sizeof( word );
		int *pTriangleIndices = new int[nTriangles];
		for ( int j = 0; j < nTriangles; j++ )
		{
			pTriangleIndices[j] = *( word* )pPtr;
			pPtr += sizeof( word );
		}

		char materialIndex = *( char* )pPtr;
		pPtr += sizeof( char );
	
		m_pMeshes[i].m_materialIndex = materialIndex;
		m_pMeshes[i].m_numTriangles = nTriangles;
		m_pMeshes[i].m_pTriangleIndices = pTriangleIndices;
	}

	int nMaterials = *( word* )pPtr;
	m_numMaterials = nMaterials;
	m_pMaterials = new Material[nMaterials];
	pPtr += sizeof( word );
	for ( i = 0; i < nMaterials; i++ )
	{
		MS3DMaterial *pMaterial = ( MS3DMaterial* )pPtr;
		memcpy( m_pMaterials[i].m_ambient, pMaterial->m_ambient, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_diffuse, pMaterial->m_diffuse, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_specular, pMaterial->m_specular, sizeof( float )*4 );
		memcpy( m_pMaterials[i].m_emissive, pMaterial->m_emissive, sizeof( float )*4 );
		m_pMaterials[i].m_shininess = pMaterial->m_shininess;
		if ( strncmp( pMaterial->m_texture, ".\\", 2 ) == 0 ) {
			// MS3D 1.5.x relative path
			strcpy( pathTemp + pathLength, pMaterial->m_texture + 1 );
			m_pMaterials[i].m_pTextureFilename = new char[strlen( pathTemp )+1];
			strcpy( m_pMaterials[i].m_pTextureFilename, pathTemp );
		}
		else {
			// MS3D 1.4.x or earlier - absolute path
			m_pMaterials[i].m_pTextureFilename = new char[strlen( pMaterial->m_texture )+1];
			strcpy( m_pMaterials[i].m_pTextureFilename, pMaterial->m_texture );
		}
		pPtr += sizeof( MS3DMaterial );
	}

	reloadTextures();

// { NEW }
	// Load Skeletal Animation Stuff

	float animFPS = *( float* )pPtr;
	pPtr += sizeof( float );

	// skip currentTime
	pPtr += sizeof( float );

	int totalFrames = *( int* )pPtr;
	pPtr += sizeof( int );

	m_totalTime = totalFrames*1000.0/animFPS;

	m_numJoints = *( word* )pPtr;
	pPtr += sizeof( word );

	m_pJoints = new Joint[m_numJoints];

	struct JointNameListRec
	{
		int m_jointIndex;
		const char *m_pName;
	};

	const byte *pTempPtr = pPtr;

	JointNameListRec *pNameList = new JointNameListRec[m_numJoints];
	for ( i = 0; i < m_numJoints; i++ )
	{
		MS3DJoint *pJoint = ( MS3DJoint* )pTempPtr;
		pTempPtr += sizeof( MS3DJoint );
		pTempPtr += sizeof( MS3DKeyframe )*( pJoint->m_numRotationKeyframes+pJoint->m_numTranslationKeyframes );

		pNameList[i].m_jointIndex = i;
		pNameList[i].m_pName = pJoint->m_name;
	}

	for ( i = 0; i < m_numJoints; i++ )
	{
		MS3DJoint *pJoint = ( MS3DJoint* )pPtr;
		pPtr += sizeof( MS3DJoint );

		int j, parentIndex = -1;
		if ( strlen( pJoint->m_parentName ) > 0 )
		{
			for ( j = 0; j < m_numJoints; j++ )
			{
				if ( stricmp( pNameList[j].m_pName, pJoint->m_parentName ) == 0 )
				{
					parentIndex = pNameList[j].m_jointIndex;
					break;
				}
			}
			if ( parentIndex == -1 ) // Unable to find parent bone in MS3D file
				return false;
		}

		memcpy( m_pJoints[i].m_localRotation, pJoint->m_rotation, sizeof( float )*3 );
		memcpy( m_pJoints[i].m_localTranslation, pJoint->m_translation, sizeof( float )*3 );
		m_pJoints[i].m_parent = parentIndex;
		m_pJoints[i].m_numRotationKeyframes = pJoint->m_numRotationKeyframes;
		m_pJoints[i].m_pRotationKeyframes = new Keyframe[pJoint->m_numRotationKeyframes];
		m_pJoints[i].m_numTranslationKeyframes = pJoint->m_numTranslationKeyframes;
		m_pJoints[i].m_pTranslationKeyframes = new Keyframe[pJoint->m_numTranslationKeyframes];

		for ( j = 0; j < pJoint->m_numRotationKeyframes; j++ )
		{
			MS3DKeyframe *pKeyframe = ( MS3DKeyframe* )pPtr;
			pPtr += sizeof( MS3DKeyframe );

			setJointKeyframe( i, j, pKeyframe->m_time*1000.0f, pKeyframe->m_parameter, true );
		}

		for ( j = 0; j < pJoint->m_numTranslationKeyframes; j++ )
		{
			MS3DKeyframe *pKeyframe = ( MS3DKeyframe* )pPtr;
			pPtr += sizeof( MS3DKeyframe );

			setJointKeyframe( i, j, pKeyframe->m_time*1000.0f, pKeyframe->m_parameter, false );
		}
	}
	delete[] pNameList;

	setupJoints();
// { end NEW }
	delete[] pBuffer;

	return true;
	
}

