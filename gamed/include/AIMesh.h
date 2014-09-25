#ifndef _AIMESH_H
#define _AIMESH_H
#include <vector>
#include <string>

#define MAX_TRIANGLE_COUNT 2500

struct ScanLine
{
   float x, y, z, u, v;
};

struct Vertex
{
   float x, y, z;
};

#pragma pack(push, 1)
struct Triangle
{
	struct
	{
		float v1[3]; // Seems to be a vertex [x,y,z]
		float v2[3]; // Seems to be a vertex [x,y,z]
		float v3[3]; // Seems to be a vertex [x,y,z]
	} Face; // If above is true

	short unk1;
	short unk2;
	short triangle_reference;
};

struct __AIMESHFILE
{
	char magic[8];
	int version;
	int triangle_count;
	int zero[2];

	Triangle triangles[MAX_TRIANGLE_COUNT];
}; 
#pragma pack(pop)

class AIMesh
{
public:
	AIMesh();
	~AIMesh();

   bool load(std::string a_File);
   float getY(float a_X, float a_Y);
	
private:
   void Line(float x1, float y1, float x2, float y2, char *a_Info, unsigned m_Width, unsigned m_Height);
   void DrawTriangle(Triangle a_Triangle, float *a_Info, unsigned a_Width, unsigned a_Height);
   void FillScanLine(Vertex a_V1, Vertex a_V2);
	bool OutputMesh(unsigned a_Width, unsigned a_Height);
	bool WriteToFile(float *a_PixelInfo, unsigned a_Width, unsigned a_Height);


	std::vector<unsigned char> m_Buffer;
   __AIMESHFILE *m_File;

   double m_LowX, m_LowY, m_HighX, m_HighY;
   ScanLine m_Lowest[1024], m_Highest[1024];
   float *m_Info;
};

#endif