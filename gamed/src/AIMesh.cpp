#include "AIMesh.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include "RAFManager.h"

#define FRACPOS(x)		((x)-((int)(x)))


// TODO: Clean this junk up

AIMesh::AIMesh() :m_File(0), m_Heightmap(0)
{
}


AIMesh::~AIMesh()
{
   if (m_Heightmap!=0)
      delete[] m_Heightmap;
}

bool AIMesh::load(std::string a_File)
{
   // Old direct file loading code
	/*std::ifstream t_FileStream(a_File, std::ios::binary);
	t_FileStream.seekg(0, std::ios::end);
	std::streamsize size = t_FileStream.tellg();
	t_FileStream.seekg(0, std::ios::beg);

	m_Buffer.resize((unsigned)size);

	if (t_FileStream.read(m_Buffer.data(), size))
	{
		m_File = (__AIMESHFILE*)m_Buffer.data();
		OutputMesh(1024, 1024);
		return true;
	}*/

   // LEVELS/Map1/AIPath.aimesh

   m_Buffer.clear();
   std::cout << "Before reading" << std::endl;
   if (RAFManager::getInstance()->readFile(a_File, m_Buffer)) // Our file exists. Load it.
   {
      std::cout << "After reading" << std::endl;
      for (int i = 0; i < 1024; i++) // For every scanline for the triangle rendering
      {
         m_Lowest[i].u = m_Lowest[i].v = m_Lowest[i].x = m_Lowest[i].y = m_Lowest[i].z = 1e10f; // Init to zero
         m_Highest[i].u = m_Highest[i].v = m_Highest[i].x = m_Highest[i].y = m_Highest[i].z = -1e10f;
      }

      m_Heightmap = new float[1024 * 1024]; // Shows occupied or not
      m_File = (__AIMESHFILE*)m_Buffer.data();
      OutputMesh(1024, 1024);

      std::cout << "Opened AIMesh file for this map." << std::endl;
      return true;
   }
   return false;
}

bool AIMesh::OutputMesh(unsigned a_Width, unsigned a_Height)
{
	for (unsigned i = 0; i < a_Width*a_Height; i++) m_Heightmap[i] = 0.0f; // Clear the map

   m_LowX = 9e9, m_LowY = 9e9, m_HighX = 0, m_HighY = 0; // Init triangle

	for (unsigned i = 0; i < m_File->triangle_count; i++) 
   // Need to find the absolute values.. So we can map it to 1024x1024 instead of 13000x15000
	{
      // Triangle low X check
		if (m_File->triangles[i].Face.v1[0] < m_LowX)
			m_LowX = m_File->triangles[i].Face.v1[0];
		if (m_File->triangles[i].Face.v2[0] < m_LowX)
			m_LowX = m_File->triangles[i].Face.v2[0];
		if (m_File->triangles[i].Face.v3[0] < m_LowX)
			m_LowX = m_File->triangles[i].Face.v3[0];		

      // Triangle low Y check
		if (m_File->triangles[i].Face.v1[2] < m_LowY)
			m_LowY = m_File->triangles[i].Face.v1[2];
		if (m_File->triangles[i].Face.v2[2] < m_LowY)
			m_LowY = m_File->triangles[i].Face.v2[2];
		if (m_File->triangles[i].Face.v3[2] < m_LowY)
			m_LowY = m_File->triangles[i].Face.v3[2];

      // Triangle high X check
		if (m_File->triangles[i].Face.v1[0] > m_HighX)
			m_HighX = m_File->triangles[i].Face.v1[0];
		if (m_File->triangles[i].Face.v2[0] > m_HighX)
			m_HighX = m_File->triangles[i].Face.v2[0];
		if (m_File->triangles[i].Face.v3[0] > m_HighX)
			m_HighX = m_File->triangles[i].Face.v3[0];
		
      // Triangle high Y check
		if (m_File->triangles[i].Face.v1[2] > m_HighY)
			m_HighY = m_File->triangles[i].Face.v1[2];
		if (m_File->triangles[i].Face.v2[2] > m_HighY)
			m_HighY = m_File->triangles[i].Face.v2[2];
		if (m_File->triangles[i].Face.v3[2] > m_HighY)
			m_HighY = m_File->triangles[i].Face.v3[2];
	}

   // If the width or height larger?
   if ((m_HighY - m_LowY) < (m_HighX - m_LowX))
   {
      m_HighX = 1.0f / (m_HighX - m_LowX)*a_Width; // We're wider than we're high, map on width
      m_HighY = m_HighX; // Keep aspect ratio Basically, 1 y should be 1 x.
      m_LowY = 0; // Though we need to project this in the middle, no offset
   }
   else
   {
      m_HighY = 1.0f / (m_HighY - m_LowY)*a_Height; // We're higher than we're wide, map on height
      m_HighX = m_HighY; // Keep aspect ratio Basically, 1 x should be 1 y.
      // m_LowX = 0; // X is already in the middle? ??????
   }

   for (unsigned i = 0; i <m_File->triangle_count; i++) // For every triangle
	{
      Triangle t_Triangle; // Create a triangle that is warped to heightmap coordinates
      t_Triangle.Face.v1[0] = (m_File->triangles[i].Face.v1[0] - m_LowX)*m_HighX;
      t_Triangle.Face.v1[1] = m_File->triangles[i].Face.v1[1];
      t_Triangle.Face.v1[2] = (m_File->triangles[i].Face.v1[2] - m_LowY)*m_HighY;

      t_Triangle.Face.v2[0] = (m_File->triangles[i].Face.v2[0] - m_LowX)*m_HighX;
      t_Triangle.Face.v2[1] = m_File->triangles[i].Face.v2[1];
      t_Triangle.Face.v2[2] = (m_File->triangles[i].Face.v2[2] - m_LowY)*m_HighY;

      t_Triangle.Face.v3[0] = (m_File->triangles[i].Face.v3[0] - m_LowX)*m_HighX;
      t_Triangle.Face.v3[1] = m_File->triangles[i].Face.v3[1];
      t_Triangle.Face.v3[2] = (m_File->triangles[i].Face.v3[2] - m_LowY)*m_HighY;

      /*
      // Draw just the wireframe.
      Line(t_Triangle.Face.v1[0], t_Triangle.Face.v1[2], t_Triangle.Face.v2[0], t_Triangle.Face.v2[2], t_Info, a_Width, a_Height);
      Line(t_Triangle.Face.v2[0], t_Triangle.Face.v2[2], t_Triangle.Face.v3[0], t_Triangle.Face.v3[2], t_Info, a_Width, a_Height);
      Line(t_Triangle.Face.v3[0], t_Triangle.Face.v3[2], t_Triangle.Face.v1[0], t_Triangle.Face.v1[2], t_Info, a_Width, a_Height);
      */

      // Draw this triangle onto the heightmap using an awesome triangle drawing function
      DrawTriangle(t_Triangle, m_Heightmap, a_Width, a_Height);
	}

	//WriteToFile(t_Info, a_Width, a_Height);
	return true;
}

float AIMesh::getY(float a_X, float a_Y)
{
   unsigned t_Pos = (unsigned)((a_X - m_LowX)*m_HighX * 1024.0f) + (a_Y - m_LowY)*m_HighY;
   return m_Heightmap[t_Pos];
}

void AIMesh::Line(float x1, float y1, float x2, float y2, char *a_Info, unsigned m_Width, unsigned m_Height)
{
   if ((x1 < 0) || (y1 < 0) || (x1 >= m_Width) || (y1 >= m_Height) ||
      (x2 < 0) || (y2 < 0) || (x2 >= m_Width) || (y2 >= m_Height))
   {
      return;
   }
   float b = x2 - x1;
   float h = y2 - y1;
   float l = fabsf(b);
   if (fabsf(h) > l) l = fabsf(h);
   int il = (int)l;
   float dx = b / (float)l;
   float dy = h / (float)l;
   for (int i = 0; i <= il; i++)
   {
      a_Info[(unsigned)x1 + (unsigned)y1 * m_Width] = (char)255;
      x1 += dx, y1 += dy;
   }
}

bool AIMesh::WriteToFile(float *a_PixelInfo, unsigned a_Width, unsigned a_Height)
{
   std::ofstream t_TGA("maps/test.tga", std::ios::binary);
   if (!t_TGA) return false;

   // The image header
   unsigned char header[18] = { 0 };
   header[2] = 1;  // truecolor
   header[12] = a_Width & 0xFF;
   header[13] = (a_Width >> 8) & 0xFF;
   header[14] = a_Height & 0xFF;
   header[15] = (a_Height >> 8) & 0xFF;
   header[16] = 24;  // bits per pit_Xel

   t_TGA.write((const char*)header, 18);

   // The image data is stored bottom-to-top, left-to-right
   for (int t_Y = a_Height - 1; t_Y >= 0; t_Y--)
      //for (int t_Y = 0; t_Y<a_Height; t_Y++)
   for (int t_X = 0; t_X < a_Width; t_X++)
   {
      t_TGA.put((char)(a_PixelInfo[(t_Y * a_Width) + t_X]));
      t_TGA.put((char)(a_PixelInfo[(t_Y * a_Width) + t_X]));
      t_TGA.put((char)(a_PixelInfo[(t_Y * a_Width) + t_X]));
   }

   // The file footer. This part is totally optional.
   static const char footer[26] =
      "\0\0\0\0"  // no et_XTension area
      "\0\0\0\0"  // no developer directort_Y
      "TRUEVISION-XFILE"  // Yep, this is a TGA file
      ".";
   t_TGA.write(footer, 26);

   t_TGA.close();
   return true;
}

void AIMesh::DrawTriangle(Triangle a_Triangle, float *a_Info, unsigned a_Width, unsigned a_Height)
{
#define MIN(a,b) (((a)>(b))?(b):(a))
#define MAX(a,b) (((a)>(b))?(a):(b))
   // The heart of the rasterizer

   Vertex m_Vertex[3];

   m_Vertex[0].x = a_Triangle.Face.v1[0];
   m_Vertex[0].y = a_Triangle.Face.v1[2];
   m_Vertex[0].z = a_Triangle.Face.v1[1];

   m_Vertex[1].x = a_Triangle.Face.v2[0];
   m_Vertex[1].y = a_Triangle.Face.v2[2];
   m_Vertex[1].z = a_Triangle.Face.v2[1];

   m_Vertex[2].x = a_Triangle.Face.v3[0];
   m_Vertex[2].y = a_Triangle.Face.v3[2];
   m_Vertex[2].z = a_Triangle.Face.v3[1];

   FillScanLine(m_Vertex[0], m_Vertex[1]);
   FillScanLine(m_Vertex[1], m_Vertex[2]);
   FillScanLine(m_Vertex[2], m_Vertex[0]);

   float t_Width = a_Width;
   float t_Height = a_Height;
   // target width and height

   int t_StartY = (int)MIN(m_Vertex[0].y, MIN(m_Vertex[1].y, m_Vertex[2].y));
   int t_EndY = (int)MAX(m_Vertex[0].y, MAX(m_Vertex[1].y, m_Vertex[2].y));
   // Get the scanline where we start drawing and where we stop.

   t_EndY = MIN(t_EndY, a_Height - 1);
   t_StartY = MAX(0, t_StartY);

   for (int y = t_StartY; y <= t_EndY; y++) // for each scanline
   {
      if (m_Lowest[y].x<m_Highest[y].x) // If we actually have something filled in this scanline
      {
         int yw = y * a_Height;

         float z = m_Lowest[y].z;
         float u = m_Lowest[y].u;
         float v = m_Lowest[y].v;
         // The start of the Z, U, and V coordinate.

         float t_DeltaX = 1.f / (m_Highest[y].x - m_Lowest[y].x);
         // Interpolation over X (change in X between the two, then reverse it so it's usable as multiplication
         // in divisions

         float t_DeltaZ = (m_Highest[y].z - m_Lowest[y].z) * t_DeltaX;
         float t_DeltaU = (m_Highest[y].u - m_Lowest[y].u) * t_DeltaX;
         float t_DeltaV = (m_Highest[y].v - m_Lowest[y].v) * t_DeltaX;
         // The interpolation in Z, U and V in respect to the interpolation of X	

         // Sub-texel correction
         int x = (int)m_Lowest[y].x;
         int t_X = x + 1;
         int t_Distance = (int)(m_Highest[y].x) - (int)(m_Lowest[y].x);
         if (t_Distance > 0.0f)
         {
            u += (((float)(t_X)) - t_X) * t_DeltaU;
            v += (((float)(t_X)) - t_X) * t_DeltaV;
            z += (((float)(t_X)) - t_X) * t_DeltaZ;
         }

         if (!(m_Highest[y].x<0 || x >= a_Height))
         for (; x<(int)m_Highest[y].x; x++) // for each piece of the scanline
         {
            if (x >= a_Height) break; // If we're out of screen, break out this loop

            if (x<0)
            {
               int t_InverseX = abs(x);
               z += t_DeltaZ * t_InverseX;
               u += t_DeltaU * t_InverseX;
               v += t_DeltaV * t_InverseX;
               x = 0;
            }        


            {
               //LeaveCriticalSection(&(a_Target->GetDrawingCS()));

               float t_InverseZ = 1.f / z;
               float t_DrawU = u * t_InverseZ;
               float t_DrawV = v * t_InverseZ;

               t_DrawU = (t_DrawU>0) ? (t_DrawU - (int)t_DrawU) : ((1.f + t_DrawU) - (int)t_DrawU);
               t_DrawV = (1.0f) - (((t_DrawV>0) ? (t_DrawV - (int)t_DrawV) : ((t_DrawV + t_DrawV) - (int)t_DrawV)));
               // Get the point on the texture that we need to draw. It basically picks a pixel based on the uv.

               //a_Target->GetRenderTarget()->Plot(x, t_Height - y, 255);
               a_Info[a_Height-x + y * a_Height] = z;
            }

            z += t_DeltaZ;
            u += t_DeltaU;
            v += t_DeltaV;
            // interpolate
         }
      }

      m_Lowest[y].x = 1e10f;
      m_Highest[y].x = -1e10f;
   }
}

void AIMesh::FillScanLine(Vertex a_V1, Vertex a_V2)
{
   // Fills a scanline structure with information about the triangle on this y scanline.

   if (a_V1.y > a_V2.y)
      return FillScanLine(a_V2, a_V1);
   // We need to go from low to high so switch if the other one is higher

   if (a_V2.y < 0 || a_V1.y >= 1024)
      return;
   // There's nothing on this line

   Vertex t_DeltaPos;
   
   t_DeltaPos.x = a_V2.x - a_V1.x;
   t_DeltaPos.y = a_V2.y - a_V1.y;
   t_DeltaPos.z = a_V2.z - a_V1.z;

   float t_Width = 1024;
   float t_Height = 1024;

   float t_DYResp = t_DeltaPos.y == 0 ? 0 : 1.f / t_DeltaPos.y;
   int t_StartY = (int)a_V1.y, t_EndY = (int)a_V2.y;

   float x = a_V1.x;
   float z = a_V1.z;

   float t_DeltaX = t_DeltaPos.x * t_DYResp;
   float t_DeltaZ = t_DeltaPos.z * t_DYResp;

   float t_Inc = 1.f - FRACPOS(a_V1.y);

   // subpixel correction
   t_StartY++;
   x += t_DeltaX * t_Inc;
   z += t_DeltaZ * t_Inc;

   if (t_StartY < 0)
   {
      x -= t_DeltaX * t_StartY;
      z -= t_DeltaZ * t_StartY;
      t_StartY = 0;
   }

   // Small fix
   if (t_EndY >= 1024) t_EndY = 1024 - 1;

   // For each scanline that this triangle uses
   for (int y = t_StartY; y <= t_EndY; y++)
   {
      if (x<m_Lowest[y].x) // If the x is lower than our lowest x
      {
         m_Lowest[y].x = (x); // Fill the scanline struct with our info
         m_Lowest[y].y = (float)y;
         m_Lowest[y].z = z;
      }
      if (x>m_Highest[y].x) // If the x is higher than our highest x
      {
         m_Highest[y].x = (x); // Fill the scanline struct with our info
         m_Highest[y].y = (float)y;
         m_Highest[y].z = z;
      }

      // Interpolate
      // Or going to the part of the triangle on the next scanline
      x += t_DeltaX;
      z += t_DeltaZ;
   }
}