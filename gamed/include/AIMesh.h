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

   bool load(std::string inputFile);
   float getY(float argX, float argY);

   float getWidth() { return mapWidth; }
   float getHeight() { return mapHeight; }
	
private:
   void drawLine(float x1, float y1, float x2, float y2, char *heightInfo, unsigned width, unsigned height);
   void drawTriangle(Triangle triangle, float *heightInfo, unsigned width, unsigned height);
   void fillScanLine(Vertex vertex1, Vertex vertex2);
	bool outputMesh(unsigned width, unsigned height);
	bool writeFile(float *pixelInfo, unsigned width, unsigned height);


	std::vector<unsigned char> buffer;
   __AIMESHFILE *fileStream;

   double lowX, lowY, highX, highY;
   ScanLine scanlineLowest[1024], scanlineHighest[1024];
   float *heightMap;
   float mapWidth, mapHeight;
};

#endif