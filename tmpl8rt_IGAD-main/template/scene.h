#pragma once

// -----------------------------------------------------------
// scene.h
// Simple test scene for ray tracing experiments. Goals:
// - Super-fast scene intersection
// - Easy interface: scene.FindNearest / IsOccluded
// - With normals and albedo: GetNormal / GetAlbedo
// - Area light source (animated), for light transport
// - Primitives can be hit from inside - for dielectrics
// - Can be extended with other primitives and/or a BVH
// - Optionally animated - for temporal experiments
// - Not everything is axis aligned - for cache experiments
// - Can be evaluated at arbitrary time - for motion blur
// - Has some high-frequency details - for filtering
// Some speed tricks that severely affect maintainability
// are enclosed in #ifdef SPEEDTRIX / #endif. Mind these
// if you plan to alter the scene in any way.
// -----------------------------------------------------------
#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>

#define SPEEDTRIX
// #define FOURLIGHTS
// #define USEBVH
// #define USEOCTREE
 #define USEGRID
// bvh settings:
#define SPATIALSPLITS
#define SBVHUNSPLITTING
#define BVHBINS 128
#define C_INN 1.2f
#define C_TRI 1.0f

// BVH, from here, towards The Optimal BVH:
// - Subtree collapsing: attempt to join leafs for better performance
//   (see: Fast Insertion-Based Optimization of Bounding Volume Hierarchies)
// - Treelet restructuring:
//   Fast Parallel Construction of HighQuality Bounding Volume Hierarchies (2013)
//   Bounding Volume Hierarchy Optimization through Agglomerative Treelet Restructuring (2015)
// - Treelet re-insertion:
//   Fast Insertion-Based Optimization of Bounding Volume Hierarchies (2013)
//   T-SAH: Animation Optimized Bounding Volume Hierarchies (2015)

#define PLANE_X(o,i) {t=-(ray.O.x+o)*ray.rD.x;if(t<ray.t&&t>0)ray.t=t,ray.objIdx=i;}
#define PLANE_Y(o,i) {t=-(ray.O.y+o)*ray.rD.y;if(t<ray.t&&t>0)ray.t=t,ray.objIdx=i;}
#define PLANE_Z(o,i) {t=-(ray.O.z+o)*ray.rD.z;if(t<ray.t&&t>0)ray.t=t,ray.objIdx=i;}

namespace Tmpl8 {

__declspec(align(64)) class Ray
{
public:
	Ray() = default;
	Ray( const float3 origin, const float3 direction, const float distance = 1e34f, const int idx = -1 )
	{
		O = origin, D = direction, t = distance;
		// calculate reciprocal ray direction for triangles and AABBs
		rD = float3( 1 / D.x, 1 / D.y, 1 / D.z );
	#ifdef SPEEDTRIX
		d0 = 1, d1 = d2 = 0; // ready for SIMD matrix math
	#endif
		objIdx = idx;
	}
	float3 IntersectionPoint() const { return O + t * D; }
	// ray data
#ifndef SPEEDTRIX
	float3 O, D, rD;
#else
	union { struct { float3 O; float d0; }; __m128 O4; };
	union { struct { float3 D; float d1; }; __m128 D4; };
	union { struct { float3 rD; float d2; }; __m128 rD4; };
#endif
	float t = 1e34f;
	int objIdx = -1;
	bool inside = false; // true when in medium
};

#ifdef USEBVH

// code taken from https://github.com/jbikker/bvh_article

// triangle count
#define PRIMCOUNT	12582 // hardcoded for the unity vehicle mesh

struct Tri
{
	aabb bounds;
	uint idx;
	float3 vertex0, vertex1, vertex2;
};

struct BVHNode
{
	union { struct { float3 aabbMin; uint leftFirst; }; __m128 aabbMin4; };
	union { struct { float3 aabbMax; uint triCount; }; __m128 aabbMax4; };
	bool isLeaf() const { return triCount > 0; }
};

struct Clipped
{
	Clipped( const Tri& tri, const aabb& box )
	{
		// set original triangle
		v[0] = tri.vertex0, v[1] = tri.vertex1, v[2] = tri.vertex2;
		verts = 3;
		// clip, one axis at a time
		for (int a = 0; a < 3; a++)
		{
			int ntmp = 0;
			float3 tmp[9], C, v0 = v[verts - 1], v1;
			float plane = box.bmin[a];
			bool in = (v0.cell[a] - plane) >= 0;
			for (int i = 0; i < verts; i++, v0 = v1)
			{
				v1 = v[i];
				const float d0 = v0.cell[a] - plane, d1 = v1.cell[a] - plane;
				if (in && d1 >= 0) /* both in */ tmp[ntmp++] = v1;
				else if (!in && d1 > 0) // coming in: emit C and d1
					C = v0 + (d0 / (d0 - d1)) * (v1 - v0),
					C[a] = plane, tmp[ntmp++] = C, tmp[ntmp++] = v1, in = true;
				else if (in && d1 < 0) // going out: emit C
					C = v0 + (d0 / (d0 - d1)) * (v1 - v0),
					C[a] = plane, tmp[ntmp++] = C, in = false;
			}
			verts = 0;
			if (ntmp < 3) break;
			v0 = tmp[ntmp - 1], plane = box.bmax[a], in = (plane - v0.cell[a]) >= 0;
			for (int i = 0; i < ntmp; i++, v0 = v1)
			{
				v1 = tmp[i];
				const float d0 = plane - v0.cell[a], d1 = plane - v1.cell[a];
				if (in && d1 >= 0) /* both in */ v[verts++] = v1;
				else if (!in && d1 > 0) // coming in: emit C and d1
					C = v0 + (d0 / (d0 - d1)) * (v1 - v0),
					C[a] = plane, v[verts++] = C, v[verts++] = v1, in = true;
				else if (in && d1 < 0) // going out: emit C
					C = v0 + (d0 / (d0 - d1)) * (v1 - v0),
					C[a] = plane, v[verts++] = C, in = false;
			}
			if (verts < 3) break; // nothing or degenerate
		}
		// calculate bounding box
		bounds = aabb();
		for (int i = 0; i < verts; i++) bounds.Grow( v[i] );
	}
	int verts;
	float3 v[9]; // 3 + 6
	aabb bounds;
};
class BVH
{
public:
	// constructor
	BVH() = default;
	BVH( const char* fileName, mat4 transform = mat4::Identity() )
	{
		auto vertices = ReadMesh(fileName);

		verticesCount = vertices.size();

		// proceed
	#ifdef SPATIALSPLITS
		tri = new Tri[verticesCount * 4];
		triIdx = new uint[verticesCount * 4]; // extra space for duplicate indices
		triTmp = new uint[verticesCount * 4]; // we'll need to double-buffer this
	#else
		tri = new Tri[PRIMCOUNT];
		triIdx = new uint[PRIMCOUNT];
	#endif
		
		triPtr = vertices.size(); // we'll reserve tri copies for spatial splits from here
		N = new float3[vertices.size()];
		
		//FILE* file = fopen( fileName, "r" );
		//for (int t = 0; t < PRIMCOUNT; t++)
		//	fscanf( file, "%f %f %f %f %f %f %f %f %f\n",
		//		&tri[t].vertex0.x, &tri[t].vertex0.y, &tri[t].vertex0.z,
		//		&tri[t].vertex1.x, &tri[t].vertex1.y, &tri[t].vertex1.z,
		//		&tri[t].vertex2.x, &tri[t].vertex2.y, &tri[t].vertex2.z );

		for (int t = 0; t < vertices.size(); t++) {
			tri[t].vertex0.x = vertices[t].vertex0.x;
			tri[t].vertex0.y = vertices[t].vertex0.y;
			tri[t].vertex0.z = vertices[t].vertex0.z;
			tri[t].vertex1.x = vertices[t].vertex1.x;
			tri[t].vertex1.y = vertices[t].vertex1.y;
			tri[t].vertex1.z = vertices[t].vertex1.z;
			tri[t].vertex2.x = vertices[t].vertex2.x;
			tri[t].vertex2.y = vertices[t].vertex2.y;
			tri[t].vertex2.z = vertices[t].vertex2.z;
		}
		
		BuildBVH();

	#ifdef SPATIALSPLITS
		FinalizeSBVH();
	#endif

		printf( "SAH:    %.1f\n", GetTotalSAH() );
		printf( "nodes:  %i\n", GetNodeCount() );
		printf( "leafs:  %i\n", GetLeafCount() );
		printf( "spatial splits: %i\n", spatialSplits );
		printf( "SAH cost = %.1f\n", SAHCost() );
		T = transform;
		invT = transform.Inverted();
		// normals
		for (int i = 0; i < verticesCount; i++)
		{
			float3 edge1 = tri[i].vertex1 - tri[i].vertex0;
			float3 edge2 = tri[i].vertex2 - tri[i].vertex0;
			N[i] = normalize( TransformVector( cross( edge1, edge2 ), T ) );
		}
	}

	std::vector<Tri> ReadMesh(const char* file) {
		std::vector<Tri> triangles;

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file, "", false);

		for (size_t s = 0; s < shapes.size(); s++) {

			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

				std::vector<float3> vertices;

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
					tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
					tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

					vertices.emplace_back(vx, vy, vz);
				}

				Tri triangle = Tri();

				triangle.vertex0 = vertices[0];
				triangle.vertex1 = vertices[1];
				triangle.vertex2 = vertices[2];

				triangle.bounds = aabb(triangle.vertex0, triangle.vertex0);
				triangle.bounds.Grow(triangle.vertex1);
				triangle.bounds.Grow(triangle.vertex2);
				triangle.idx = f;

				triangles.emplace_back(triangle);

				index_offset += fv;
			}
		}

		return triangles;
	}

	// methods
	float3 GetNormal( const uint triIdx ) const { return N[triIdx]; }

	void IntersectTri( Ray& ray, const uint triIdx ) const
	{
		const Tri& tri = this->tri[triIdx];
		const float3 edge1 = tri.vertex1 - tri.vertex0, edge2 = tri.vertex2 - tri.vertex0;
		const float3 h = cross( ray.D, edge2 );
		const float a = dot( edge1, h );
		if (a > -0.0001f && a < 0.0001f) return; // ray parallel to triangle
		const float3 s = ray.O - tri.vertex0;
		const float f = 1 / a, u = f * dot( s, h );
		if (u < 0 || u > 1) return;
		const float3 q = cross( s, edge1 );
		const float v = f * dot( ray.D, q );
		if (v < 0 || u + v > 1) return;
		const float t = f * dot( edge2, q );
		if (t > 0.0001f && t < ray.t) ray.t = t, ray.objIdx = 1000 + triIdx;
	}

	float IntersectAABB( const Ray& ray, const float3 bmin, const float3 bmax ) const
	{
		float tx1 = (bmin.x - ray.O.x) * ray.rD.x, tx2 = (bmax.x - ray.O.x) * ray.rD.x;
		float tmin = min( tx1, tx2 ), tmax = max( tx1, tx2 );
		float ty1 = (bmin.y - ray.O.y) * ray.rD.y, ty2 = (bmax.y - ray.O.y) * ray.rD.y;
		tmin = max( tmin, min( ty1, ty2 ) ), tmax = min( tmax, max( ty1, ty2 ) );
		float tz1 = (bmin.z - ray.O.z) * ray.rD.z, tz2 = (bmax.z - ray.O.z) * ray.rD.z;
		tmin = max( tmin, min( tz1, tz2 ) ), tmax = min( tmax, max( tz1, tz2 ) );
		if (tmax >= tmin && tmin < ray.t && tmax > 0) return tmin; else return 1e30f;
	}

	void Intersect( Ray& ray ) const
	{
		const float3 O = TransformPosition_SSE( ray.O4, invT );
		const float3 D = TransformVector_SSE( ray.D4, invT );
		Ray rayT( O, D, ray.t, ray.objIdx );
		BVHNode* node = &bvhNode[rootNodeIdx], * stack[64];
		uint stackPtr = 0;
		while (1)
		{
			if (node->isLeaf())
			{
				for (uint i = 0; i < node->triCount; i++)
					IntersectTri( rayT, triIdx[node->leftFirst + i] );
				if (stackPtr == 0) break; else node = stack[--stackPtr];
				continue;
			}
			BVHNode* child1 = &bvhNode[node->leftFirst];
			BVHNode* child2 = &bvhNode[node->leftFirst + 1];
			float dist1 = IntersectAABB( rayT, child1->aabbMin, child1->aabbMax );
			float dist2 = IntersectAABB( rayT, child2->aabbMin, child2->aabbMax );
			if (dist1 > dist2) { swap( dist1, dist2 ); swap( child1, child2 ); }
			if (dist1 == 1e30f)
			{
				if (stackPtr == 0) break; else node = stack[--stackPtr];
				continue;
			}
			node = child1;
			if (dist2 != 1e30f) stack[stackPtr++] = child2;
		}
		ray.t = rayT.t, ray.objIdx = rayT.objIdx;
	}

	void BuildBVH()
	{
		// create the BVH node pool
	#ifdef SPATIALSPLITS
		// 4N, to be sure; spatial splits cause this to be a bit unpredictable
		bvhNode = (BVHNode*)_aligned_malloc( sizeof( BVHNode ) * verticesCount * 4, 64 );
	#else
		bvhNode = (BVHNode*)_aligned_malloc( sizeof( BVHNode ) * verticesCount * 2, 64 );
	#endif
		
		// populate triangle index array
		for (int i = 0; i < verticesCount; i++)
			triIdx[i] = i;

		// calculate triangle centroids for partitioning
		for (int i = 0; i < verticesCount; i++)
		{
			tri[i].bounds = aabb( tri[i].vertex0, tri[i].vertex0 );
			tri[i].bounds.Grow( tri[i].vertex1 );
			tri[i].bounds.Grow( tri[i].vertex2 );
			tri[i].idx = i;
			// NOTE: these will be duplicated for spatial splits; original and copy will
			// have unique centroids but the same idx.
		}

		// assign all triangles to root node
		BVHNode& root = bvhNode[rootNodeIdx];
		root.leftFirst = 0, root.triCount = verticesCount;
		// update root node bounds
		root.aabbMin = float3( 1e30f ), 
		root.aabbMax = float3( -1e30f );
		
		for (uint i = 0; i < root.triCount; i++)
		{
			const uint leafTriIdx = triIdx[i];
			root.aabbMin = fminf( root.aabbMin, tri[leafTriIdx].bounds.bmin3 );
			root.aabbMax = fmaxf( root.aabbMax, tri[leafTriIdx].bounds.bmax3 );
		}

		// subdivide recursively
		spatialSplits = 0;
		#ifdef SPATIALSPLITS
			Subdivide( rootNodeIdx, verticesCount /* initial spare prim idx slots */ );
		#else
			Subdivide( rootNodeIdx );
		#endif
	}
	float GetTotalSAH( const uint nodeIdx = 0 )
	{
		BVHNode& node = bvhNode[nodeIdx];
		if (node.isLeaf()) return aabb( node.aabbMin, node.aabbMax ).Area();
		return GetTotalSAH( node.leftFirst ) + GetTotalSAH( node.leftFirst + 1 );
	}
	int GetNodeCount( const uint nodeIdx = 0 )
	{
		BVHNode& node = bvhNode[nodeIdx];
		if (node.isLeaf()) return 1;
		return GetNodeCount( node.leftFirst ) + GetNodeCount( node.leftFirst + 1 );
	}
	int GetLeafCount( const uint nodeIdx = 0 )
	{
		BVHNode& node = bvhNode[nodeIdx];
		if (node.isLeaf()) return 1;
		return GetLeafCount( node.leftFirst ) + GetLeafCount( node.leftFirst + 1 );
	}
	void FinalizeSBVH( const uint nodeIdx = 0 )
	{
		BVHNode& node = bvhNode[nodeIdx];
		if (node.isLeaf())
		{
			for (uint i = 0; i < node.triCount; i++)
			{
				uint idx = triIdx[node.leftFirst + i];
				triIdx[node.leftFirst + i] = tri[idx].idx;
			}
			return;
		}
		FinalizeSBVH( node.leftFirst );
		FinalizeSBVH( node.leftFirst + 1 );
	}
	float FindBestObjectSplitPlane( const BVHNode& node, int& axis, float& splitPos, aabb& lbox, aabb& rbox )
	{
		float bestCost = 1e30f;
		for (int a = 0; a < 3; a++)
		{
			float boundsMin = 1e30f, boundsMax = -1e30f;
			for (uint i = 0; i < node.triCount; i++)
			{
				Tri& triangle = tri[triIdx[node.leftFirst + i]];
				float triCenter = triangle.bounds.Center( a );
				boundsMin = min( boundsMin, triCenter );
				boundsMax = max( boundsMax, triCenter );
			}
			if (boundsMin == boundsMax) continue;
			// loop over split plane candidates
			float binWidth = (boundsMax - boundsMin) / BVHBINS;
			for (int b = 1; b < BVHBINS; b++)
			{
				float plane = boundsMin + binWidth * b;
				aabb leftBox, rightBox;
				uint leftCount = 0, rightCount = 0;
				for (uint i = 0; i < node.triCount; i++)
				{
					Tri& triangle = tri[triIdx[node.leftFirst + i]];
					if (triangle.bounds.Center( a ) < plane)
					{
						leftBox.Grow( triangle.bounds );
						leftCount++;
					}
					else
					{
						rightBox.Grow( triangle.bounds );
						rightCount++;
					}
				}
				float planeCost = leftCount * leftBox.Area() + rightCount * rightBox.Area();
				if (planeCost < bestCost)
					axis = a, splitPos = plane, bestCost = planeCost,
					lbox = leftBox, rbox = rightBox;
			}
		}
		return bestCost;
	}
	float FindBestSpatialSplitPlane( const BVHNode& node, int& axis, float& splitPos, int& Nleft, int& Nright, aabb& boundsLeft, aabb& boundsRight, int& splitted )
	{
		float bestCost = 1e30f;
		for (int a = 0; a < 3; a++)
		{
			float boundsMin = node.aabbMin.cell[a], boundsMax = node.aabbMax.cell[a];
			if (boundsMin == boundsMax) continue;
			// calculate cost of spatial splits
			float binExtend = (boundsMax - boundsMin) / BVHBINS;
			for (int b = 1; b < BVHBINS; b++)
			{
				// calculate spatial split plane position
				float pos = boundsMin + b * binExtend;
				// construct left and right bounding box
				aabb leftBounds, rightBounds;
				aabb leftBox( node.aabbMin, node.aabbMax );
				aabb rightBox( node.aabbMin, node.aabbMax );
				leftBox.bmax[a] = rightBox.bmin[a] = pos;
				int leftCount = 0, rightCount = 0;
				// loop over triangles
				for (uint i = 0; i < node.triCount; i++)
				{
					// fetch a triangle
					const Tri& triangle = tri[triIdx[node.leftFirst + i]];
					// extend left and right side
					const Clipped leftTri( triangle, leftBox );
					if (leftTri.verts > 2)
					{
						leftBounds.Grow( leftTri.bounds );
						leftCount++;
					}
					const Clipped rightTri( triangle, rightBox );
					if (rightTri.verts > 2)
					{
						rightBounds.Grow( rightTri.bounds );
						rightCount++;
					}
				}
				// calculate cost for this split plane
				if (leftCount > 0 && rightCount > 0)
				{
					float cost =
						max( 0.0f, leftBounds.Area() ) * leftCount +
						max( 0.0f, rightBounds.Area() ) * rightCount;
					if (cost < bestCost)
						axis = a, bestCost = cost, splitPos = pos,
						splitted = (rightCount + leftCount) - node.triCount,
						Nleft = leftCount, Nright = rightCount,
						boundsLeft = leftBounds, boundsRight = rightBounds;
				}
			}
		}
		return bestCost;
	}

	float SAHCost( const uint nodeIdx = 0 )
	{
		const BVHNode& node = bvhNode[nodeIdx];
		const float3 e = node.aabbMax - node.aabbMin;
		const float surfaceArea = 2 * e.x * e.y + e.y * e.z + e.z * e.x;
		float cost = 0;
		if (node.isLeaf())
		{
			return surfaceArea * node.triCount * C_TRI;
		}
		else
		{
			cost += SAHCost( node.leftFirst );
			cost += SAHCost( node.leftFirst + 1 );
			cost += surfaceArea * C_INN;
		}
		if (nodeIdx == 0) cost *= 1.0f / surfaceArea;
		return cost;
	}

	float CalculateNodeCost( BVHNode& node ) const
	{
		float3 e = node.aabbMax - node.aabbMin; // extent of the node
		float surfaceArea = e.x * e.y + e.y * e.z + e.z * e.x;
		return node.triCount * surfaceArea;
	}

	void Subdivide( uint nodeIdx, int slack = 0 )
	{
		// terminate recursion
		BVHNode& node = bvhNode[nodeIdx];
		int objSplitAxis = 0, spatialSplitAxis = 0, splitted = 0;
		aabb leftBox, rightBox;
		float objSplitPos, spatialSplitPos, nosplitCost = CalculateNodeCost( node );
		// determine split axis using SAH
		float objectSplitCost = FindBestObjectSplitPlane( node, objSplitAxis, objSplitPos, leftBox, rightBox );
	#ifdef SPATIALSPLITS
		// consider a spatial split
		float rootArea = aabb( bvhNode[0].aabbMin, bvhNode[0].aabbMax ).Area();
		float lambda = leftBox.Intersection( rightBox ).Area() / rootArea;
		if (lambda > 1e-5f)
		{
			int Nleft = 0, Nright = 0;
			aabb boundsLeft, boundsRight;
			float spatialSplitCost = FindBestSpatialSplitPlane( node, spatialSplitAxis, spatialSplitPos, Nleft, Nright, boundsLeft, boundsRight, splitted );
			if (spatialSplitCost < objectSplitCost && splitted < slack)
			{
				if (spatialSplitCost >= nosplitCost) return; // don't split, not worth it
				aabb leftOfSplit, rightOfSplit;
				leftOfSplit = rightOfSplit = aabb( node.aabbMin, node.aabbMax );
				leftOfSplit.bmax[spatialSplitAxis] = rightOfSplit.bmin[spatialSplitAxis] = spatialSplitPos;
				// spatial split partition, via temporary array
				int leftPos = node.leftFirst, leftCount = 0;
				int rightPos = node.leftFirst + node.triCount + slack, rightCount = 0;
				for (uint i = 0; i < node.triCount; i++)
				{
					uint idx = triIdx[node.leftFirst + i];
					Clipped leftPart( tri[idx], leftOfSplit );
					Clipped rightPart( tri[idx], rightOfSplit );
					bool inLeft = leftPart.verts >= 3;
					bool inRight = rightPart.verts >= 3;
				#ifdef SBVHUNSPLITTING
					// consider unsplitting straddlers
					if (inLeft && inRight)
					{
						float C1 = boundsLeft.Union( tri[idx].bounds ).Area() * Nleft + boundsRight.Area() * (Nright - 1);
						float C2 = boundsLeft.Area() * (Nleft - 1) + boundsRight.Union( tri[idx].bounds ).Area() * Nright;
						if (C1 < spatialSplitCost || C2 < spatialSplitCost) if (C1 < C2)
						{
							spatialSplitCost = C1, Nright--;
							boundsLeft.Grow( tri[idx].bounds );
							leftPart.bounds = tri[idx].bounds, inRight = false; // undo clip
						}
						else
						{
							spatialSplitCost = C2, Nleft--;
							boundsRight.Grow( tri[idx].bounds );
							rightPart.bounds = tri[idx].bounds, inLeft = false; // undo clip
						}
					}
				#endif
					if (inLeft) triIdx[leftPos++] = idx, tri[idx].bounds = leftPart.bounds, leftCount++;
					if (inRight)
					{
						if (inLeft) tri[triPtr] = tri[idx], idx = triPtr++;
						triTmp[--rightPos] = idx, tri[idx].bounds = rightPart.bounds, rightCount++;
					}
				}
				// update slack size
				slack -= splitted;
				int halfSlack = slack / 2; // will be assigned to each child
				// copy right slice from temp array to triangle index array
				memmove( triIdx + node.leftFirst + leftCount + halfSlack, triTmp + rightPos, rightCount * sizeof( uint ) );
				// create child nodes
				int leftChildIdx = nodesUsed++, rightChildIdx = nodesUsed++;
				bvhNode[leftChildIdx].leftFirst = node.leftFirst;
				bvhNode[leftChildIdx].triCount = leftCount;
				bvhNode[rightChildIdx].leftFirst = node.leftFirst + leftCount + halfSlack;
				bvhNode[rightChildIdx].triCount = rightCount;
				node.leftFirst = leftChildIdx, node.triCount = 0;
				// set node bounds
				bvhNode[leftChildIdx].aabbMin = boundsLeft.bmin3;
				bvhNode[leftChildIdx].aabbMax = boundsLeft.bmax3;
				bvhNode[rightChildIdx].aabbMin = boundsRight.bmin3;
				bvhNode[rightChildIdx].aabbMax = boundsRight.bmax3;
				// recurse
				spatialSplits++;
				Subdivide( leftChildIdx, halfSlack );
				Subdivide( rightChildIdx, halfSlack );
				return;
			}
		}
	#endif
		if (objectSplitCost >= nosplitCost) return;
		// in-place partition
		int i = node.leftFirst, j = i + node.triCount - 1;
		while (i <= j)
		{
			if (tri[triIdx[i]].bounds.Center( objSplitAxis ) < objSplitPos) i++;
			else swap( triIdx[i], triIdx[j--] );
		}
		// abort split if one of the sides is empty
		int halfSlack = slack / 2, leftCount = i - node.leftFirst;
		if (leftCount == 0 || leftCount == node.triCount) return;
		// if we're doing spatial splits we need to divide slack space here
		if (halfSlack > 0) memmove( triIdx + i + halfSlack, triIdx + i, (node.triCount - leftCount) * sizeof( uint ) );
		// create child nodes
		int leftChildIdx = nodesUsed++, rightChildIdx = nodesUsed++;
		bvhNode[leftChildIdx].leftFirst = node.leftFirst;
		bvhNode[leftChildIdx].triCount = leftCount;
		bvhNode[rightChildIdx].leftFirst = i + halfSlack;
		bvhNode[rightChildIdx].triCount = node.triCount - leftCount;
		node.leftFirst = leftChildIdx, node.triCount = 0;
		bvhNode[leftChildIdx].aabbMin = leftBox.bmin3;
		bvhNode[leftChildIdx].aabbMax = leftBox.bmax3;
		bvhNode[rightChildIdx].aabbMin = rightBox.bmin3;
		bvhNode[rightChildIdx].aabbMax = rightBox.bmax3;
		// recurse
		Subdivide( leftChildIdx, halfSlack );
		Subdivide( rightChildIdx, halfSlack );
	}
	// BVH data members
	int verticesCount = 0;

	Tri* tri = 0;
	BVHNode* bvhNode = 0;
	uint* triIdx = 0, * triTmp = 0, rootNodeIdx = 0, nodesUsed = 2, triPtr = 0;
	uint spatialSplits = 0;
	mat4 T, invT;
	float3* N = 0;
};

#endif


#ifdef USEGRID

struct Tri
{
	uint idx;
	aabb bounds;
	float3 vertex0, vertex1, vertex2;
};

struct GridCell {
	aabb bounds;
	int triIndices;
	std::vector<uint> triIdx;
};

struct GridNode {
	aabb bounds;
	int cellCount;
	GridCell* cells;
};

class Grid {

private:
	std::vector<Tri> triangles;
	std::vector<float3> N;
	GridNode gridNode;

public:
	Grid() = default;

	Grid(const char* file, int grids, mat4 t) {
		BuildGrid(file, grids, grids, grids, t);
	}

	Grid(const char* file, int width, int height, int depth, mat4 t) {
		BuildGrid(file, width, height, depth, t);
	}

	void BuildGrid(const char* file, int width, int height, int depth, mat4 transform = mat4::Identity()) {
		triangles = ReadMesh(file);

		aabb boundingSpace = findBoundingBox(&triangles);

		gridNode.bounds = boundingSpace;
		gridNode.cellCount = width * height * depth;
		gridNode.cells = new GridCell[width * height * depth];

		int tri_count = 0;

		float cellSizeX = boundingSpace.Extend(0) / static_cast<float>(width);
		float cellSizeY = boundingSpace.Extend(1) / static_cast<float>(height);
		float cellSizeZ = boundingSpace.Extend(2) / static_cast<float>(depth);

		auto vertexCount = 0;

		for (int i = 0; i < width; ++i) {
			for (int j = 0; j < height; ++j) {
				for (int k = 0; k < depth; ++k) {
					auto& cell = gridNode.cells[i * j * k];

					float minX = boundingSpace.Minimum(0) + i * cellSizeX;
					float minY = boundingSpace.Minimum(1) + j * cellSizeY;
					float minZ = boundingSpace.Minimum(2) + k * cellSizeZ;

					float maxX = minX + cellSizeX;
					float maxY = minY + cellSizeY;
					float maxZ = minZ + cellSizeZ;

					aabb cellBounds;
					cellBounds.SetBounds({ minX, minY, minZ }, { maxX, maxY, maxZ });

					if (boundingSpace.Intersection(cellBounds).Area() > 0.0f) {
						cell.bounds = cellBounds;

						std::vector<uint> tri_indices;
						
						for (int i = 0; i < triangles.size(); i++) {
							const Tri& triangle = triangles[i];

							float3 triangleMiddlepoint = (triangle.vertex0 + triangle.vertex1 + triangle.vertex2) / 3;

							if ((triangleMiddlepoint.x <= cellBounds.bmax[0]) && (triangleMiddlepoint.x >= cellBounds.bmin[0]) &&
								(triangleMiddlepoint.y <= cellBounds.bmax[1]) && (triangleMiddlepoint.y >= cellBounds.bmin[1]) &&
								(triangleMiddlepoint.z <= cellBounds.bmax[2]) && (triangleMiddlepoint.z >= cellBounds.bmin[2]))
							{
								vertexCount += 1;
								tri_indices.emplace_back(i);
							}
						}

						cell.triIdx = tri_indices;
						cell.triIndices = tri_indices.size();
					}
				}
			}
		}

		mat4 T = transform;
		mat4 Tinv = transform.Inverted();

		for (int i = 0; i < triangles.size(); i++)
		{
 			float3 edge1 = triangles[i].vertex1 - triangles[i].vertex0;
			float3 edge2 = triangles[i].vertex2 - triangles[i].vertex0;
			N.emplace_back(normalize(TransformVector(cross(edge1, edge2), T)));
		}
	}

	// methods
	float3 GetNormal(const uint triIdx) const { 
		return N[triIdx]; 
	}
	

	std::vector<Tri> ReadMesh(const char* file) {
		std::vector<Tri> triangles;

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string warn;
		std::string err;

		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, file);

		for (size_t s = 0; s < shapes.size(); s++) {

			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

				std::vector<float3> vertices;

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
					tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
					tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

					vertices.emplace_back(vx, vy, vz);
				}

				Tri triangle = Tri();

				triangle.vertex0 = vertices[0];
				triangle.vertex1 = vertices[1];
				triangle.vertex2 = vertices[2];

				triangle.bounds = aabb(triangle.vertex0, triangle.vertex0);
				triangle.bounds.Grow(triangle.vertex1);
				triangle.bounds.Grow(triangle.vertex2);
				triangle.idx = f;

				triangles.emplace_back(triangle);

				index_offset += fv;
			}
		}

		return triangles;
	}

	aabb findBoundingBox(std::vector<Tri>* triangles) {
		aabb boundingBox;

		boundingBox.SetBounds({ 1e30f, 1e30f ,1e30f }, { -1e30f, -1e30f, -1e30f });

		for (auto& triangle : *triangles) {
			boundingBox.bmin3 = fminf(boundingBox.bmin3, triangle.bounds.bmin3);
			boundingBox.bmax3 = fmaxf(boundingBox.bmax3, triangle.bounds.bmax3);
		}
		
		return boundingBox;
	}

	void IntersectTri(Ray& ray, const uint triIdx) const
	{
		const Tri& tri = this->triangles[triIdx];
		const float3 edge1 = tri.vertex1 - tri.vertex0, edge2 = tri.vertex2 - tri.vertex0;
		const float3 h = cross(ray.D, edge2);
		const float a = dot(edge1, h);
		
		if (a > -0.0001f && a < 0.0001f) 
			return; // ray parallel to triangle
		
		const float3 s = ray.O - tri.vertex0;
		const float f = 1 / a, u = f * dot(s, h);
		
		if (u < 0 || u > 1) 
			return;
		
		const float3 q = cross(s, edge1);
		const float v = f * dot(ray.D, q);
		
		if (v < 0 || u + v > 1) 
			return;
		
		const float t = f * dot(edge2, q);
		
		if (t > 0.0001f && t < ray.t) 
			ray.t = t, ray.objIdx = 1000 + triIdx;
	}

	float IntersectAABB(Ray& ray, const aabb& bounds) const {
		float tx1 = (bounds.bmin3.x - ray.O.x) * ray.rD.x;
		float tx2 = (bounds.bmax3.x - ray.O.x) * ray.rD.x;
		float tmin = min(tx1, tx2), tmax = max(tx1, tx2);
		float ty1 = (bounds.bmin3.y - ray.O.y) * ray.rD.y;
		float ty2 = (bounds.bmax3.y - ray.O.y) * ray.rD.y;
		
		tmin = max(tmin, min(ty1, ty2));
		tmax = min(tmax, max(ty1, ty2));
		
		float tz1 = (bounds.bmin3.z - ray.O.z) * ray.rD.z;
		float tz2 = (bounds.bmax3.z - ray.O.z) * ray.rD.z;

		tmin = max(tmin, min(tz1, tz2));
		tmax = min(tmax, max(tz1, tz2));

		if (tmax >= tmin && tmin < ray.t && tmax > 0) 
			return tmin; 
		else 
			return -1e30f;
	}

	int Intersect(Ray& ray) const {
		int intersections = 0;
		bool isInside = false;

		if (ray.O.x >= gridNode.bounds.bmin3.x && ray.O.x <= gridNode.bounds.bmax3.x &&
			ray.O.y >= gridNode.bounds.bmin3.y && ray.O.z <= gridNode.bounds.bmax3.y &&
			ray.O.z >= gridNode.bounds.bmin3.z && ray.O.y <= gridNode.bounds.bmax3.z) 
		{
			isInside = true;
		}

		float t = IntersectAABB(ray, gridNode.bounds);

		if (t < 0 && !isInside) {
			return 0;
		}

		intersections++;

		std::vector<GridCell> intersectedCells;

		for (int i = 0; i < gridNode.cellCount; i++) {
			auto cell = gridNode.cells[i];

			if (IntersectAABB(ray, cell.bounds) >= 0) {
				intersectedCells.emplace_back(cell);
			}
		}

		for (auto& cell : intersectedCells) {
			for (int i = 0; i <= cell.triIndices - 1; i++) {
				auto triangleIndex = cell.triIdx[i];

				IntersectTri(ray, triangleIndex);
				intersections++;
			}
		}

		return intersections;
	}
};


#endif

// -----------------------------------------------------------
// Sphere primitive
// Basic sphere, with explicit support for rays that start
// inside it. Good candidate for a dielectric material.
// -----------------------------------------------------------
class Sphere
{
public:
	Sphere() = default;
	Sphere( int idx, float3 p, float r ) :
		pos( p ), r2( r* r ), invr( 1 / r ), objIdx( idx ) {}
	void Intersect( Ray& ray ) const
	{
	#if 1
		const __m128 oc = _mm_sub_ps( ray.O4, _mm_set_ps( 1, 1, this->pos.y, -1.8f ) );
		const float b = _mm_dp_ps( oc, ray.D4, 0xFF ).m128_f32[0];
		const float c = _mm_dp_ps( oc, oc, 0xFF ).m128_f32[0];
	#else
		float3 oc = ray.O - this->pos;
		float b = dot( oc, ray.D );
		float c = dot( oc, oc ) - this->r2;
	#endif
		float t, d = b * b - c;
		if (d <= 0) return;
		d = sqrtf( d ), t = -b - d;
		bool hit = t < ray.t && t > 0;
		if (hit)
		{
			ray.t = t, ray.objIdx = objIdx;
			return;
		}
		if (c > 0) return; // we're outside; safe to skip option 2
		t = d - b, hit = t < ray.t && t > 0;
		if (hit) ray.t = t, ray.objIdx = objIdx;
	}
	bool IsOccluded( const Ray& ray ) const
	{
	#if 1
		const __m128 oc = _mm_sub_ps( ray.O4, _mm_set_ps( 1, 1, this->pos.y, -1.8f ) );
		const float b = _mm_dp_ps( oc, ray.D4, 0xFF ).m128_f32[0];
		const float c = _mm_dp_ps( oc, oc, 0xFF ).m128_f32[0];
	#else
		float3 oc = ray.O - this->pos;
		float b = dot( oc, ray.D );
		float c = dot( oc, oc ) - this->r2;
	#endif
		float t, d = b * b - c;
		if (d <= 0) return false;
		d = sqrtf( d ), t = -b - d;
		bool hit = t < ray.t && t > 0;
		return hit;
	}
	float3 GetNormal( const float3 I ) const
	{
		return (I - this->pos) * invr;
	}
	float3 GetAlbedo( const float3 I ) const
	{
		return float3( 0.93f );
	}
	union
	{
		float3 pos;
		__m128 pos4;
	};
	float r2 = 0, invr = 0;
	int objIdx = -1;
};

// -----------------------------------------------------------
// Plane primitive
// Basic infinite plane, defined by a normal and a distance
// from the origin (in the direction of the normal).
// -----------------------------------------------------------
class Plane
{
public:
	Plane() = default;
	Plane( int idx, float3 normal, float dist ) : N( normal ), d( dist ), objIdx( idx ) {}
	void Intersect( Ray& ray ) const
	{
		float t = -(dot( ray.O, this->N ) + this->d) / (dot( ray.D, this->N ));
		if (t < ray.t && t > 0) ray.t = t, ray.objIdx = objIdx;
	}
	float3 GetNormal( const float3 I ) const
	{
		return N;
	}
	float3 GetAlbedo( const float3 I ) const
	{
		if (N.y == 1)
		{
			// floor albedo: checkerboard
			int ix = (int)(I.x * 2 + 96.01f);
			int iz = (int)(I.z * 2 + 96.01f);
			// add deliberate aliasing to two tile
			if (ix == 98 && iz == 98) ix = (int)(I.x * 32.01f), iz = (int)(I.z * 32.01f);
			if (ix == 94 && iz == 98) ix = (int)(I.x * 64.01f), iz = (int)(I.z * 64.01f);
			return float3( ((ix + iz) & 1) ? 1 : 0.3f );
		}
		else if (N.z == -1)
		{
			// back wall: logo
			static Surface logo( "../assets/logo.png" );
			int ix = (int)((I.x + 4) * (128.0f / 8)), iy = (int)((2 - I.y) * (64.0f / 3));
			uint p = logo.pixels[(ix & 127) + (iy & 63) * 128];
			uint3 i3( (p >> 16) & 255, (p >> 8) & 255, p & 255 );
			return float3( i3 ) * (1.0f / 255.0f);
		}
		else if (N.x == 1)
		{
			// left wall: red
			static Surface red( "../assets/red.png" );
			int ix = (int)((I.z - 4) * (512.0f / 7)), iy = (int)((2 - I.y) * (512.0f / 3));
			uint p = red.pixels[(ix & 511) + (iy & 511) * 512];
			uint3 i3( (p >> 16) & 255, (p >> 8) & 255, p & 255 );
			return float3( i3 ) * (1.0f / 255.0f);
		}
		else if (N.x == -1)
		{
			// right wall: blue
			static Surface blue( "../assets/blue.png" );
			int ix = (int)((I.z - 4) * (512.0f / 7)), iy = (int)((2 - I.y) * (512.0f / 3));
			uint p = blue.pixels[(ix & 511) + (iy & 511) * 512];
			uint3 i3( (p >> 16) & 255, (p >> 8) & 255, p & 255 );
			return float3( i3 ) * (1.0f / 255.0f);
		}
		return float3( 0.93f );
	}
	float3 N;
	float d;
	int objIdx = -1;
};

// -----------------------------------------------------------
// Cube primitive
// Oriented cube. Unsure if this will also work for rays that
// start inside it; maybe not the best candidate for testing
// dielectrics.
// -----------------------------------------------------------
class Cube
{
public:
	Cube() = default;
	Cube( int idx, float3 pos, float3 size, mat4 transform = mat4::Identity() )
	{
		objIdx = idx;
		b[0] = float4( pos - 0.5f * size, 1 );
		b[1] = float4( pos + 0.5f * size, 1 );
		M = transform, invM = transform.FastInvertedTransformNoScale();
	}
	void Intersect( Ray& ray ) const
	{
		// 'rotate' the cube by transforming the ray into object space
		// using the inverse of the cube transform.
	#ifdef SPEEDTRIX
		// an AABB can be efficiently tested with SIMD - and matrix math is easy too. Profit!
		const __m128 a4 = ray.O4, b4 = ray.D4;
		__m128 v0 = _mm_mul_ps( a4, _mm_load_ps( &invM.cell[0] ) ), v1 = _mm_mul_ps( a4, _mm_load_ps( &invM.cell[4] ) );
		__m128 v2 = _mm_mul_ps( a4, _mm_load_ps( &invM.cell[8] ) ), v3 = _mm_mul_ps( a4, _mm_load_ps( &invM.cell[12] ) );
		_MM_TRANSPOSE4_PS( v0, v1, v2, v3 );
		__m128 o4 = _mm_add_ps( _mm_add_ps( v0, v1 ), _mm_add_ps( v2, v3 ) );
		__m128 v4 = _mm_mul_ps( b4, _mm_load_ps( &invM.cell[0] ) ), v5 = _mm_mul_ps( b4, _mm_load_ps( &invM.cell[4] ) );
		__m128 v6 = _mm_mul_ps( b4, _mm_load_ps( &invM.cell[8] ) ), v7 = _mm_mul_ps( b4, _mm_load_ps( &invM.cell[12] ) );
		_MM_TRANSPOSE4_PS( v4, v5, v6, v7 );
		__m128 d4 = _mm_add_ps( _mm_add_ps( v4, v5 ), v6 );
		__m128 rd4 = _mm_div_ps( _mm_set1_ps( 1.0f ), d4 ); // _mm_rcp_ps( d4 ); // reduced precision unsufficient?
		// AABB test
		__m128 t1 = _mm_mul_ps( _mm_sub_ps( bmin4, o4 ), rd4 );
		__m128 t2 = _mm_mul_ps( _mm_sub_ps( bmax4, o4 ), rd4 );
		__m128 vmax4 = _mm_max_ps( t1, t2 ), vmin4 = _mm_min_ps( t1, t2 );
		float tmax = min( vmax4.m128_f32[0], min( vmax4.m128_f32[1], vmax4.m128_f32[2] ) );
		float tmin = max( vmin4.m128_f32[0], max( vmin4.m128_f32[1], vmin4.m128_f32[2] ) );
		if (tmin < tmax) if (tmin > 0)
		{
			if (tmin < ray.t) ray.t = tmin, ray.objIdx = objIdx;
		}
		else if (tmax > 0)
		{
			if (tmax < ray.t) ray.t = tmax, ray.objIdx = objIdx;
		}
	#else
		float3 O = TransformPosition( ray.O, invM );
		float3 D = TransformVector( ray.D, invM );
		float rDx = 1 / D.x, rDy = 1 / D.y, rDz = 1 / D.z;
		int signx = D.x < 0, signy = D.y < 0, signz = D.z < 0;
		float tmin = (b[signx].x - O.x) * rDx;
		float tmax = (b[1 - signx].x - O.x) * rDx;
		float tymin = (b[signy].y - O.y) * rDy;
		float tymax = (b[1 - signy].y - O.y) * rDy;
		if (tmin > tymax || tymin > tmax) return;
		tmin = max( tmin, tymin ), tmax = min( tmax, tymax );
		float tzmin = (b[signz].z - O.z) * rDz;
		float tzmax = (b[1 - signz].z - O.z) * rDz;
		if (tmin > tzmax || tzmin > tmax) return;
		tmin = max( tmin, tzmin ), tmax = min( tmax, tzmax );
		if (tmin > 0)
		{
			if (tmin < ray.t) ray.t = tmin, ray.objIdx = objIdx;
		}
		else if (tmax > 0)
		{
			if (tmax < ray.t) ray.t = tmax, ray.objIdx = objIdx;
		}
	#endif
	}
	bool IsOccluded( const Ray& ray ) const
	{
	#ifdef SPEEDTRIX
	// an AABB can be efficiently tested with SIMD - and matrix math is easy too. Profit!
		const __m128 a4 = ray.O4, b4 = ray.D4;
		__m128 v0 = _mm_mul_ps( a4, _mm_load_ps( &invM.cell[0] ) ), v1 = _mm_mul_ps( a4, _mm_load_ps( &invM.cell[4] ) );
		__m128 v2 = _mm_mul_ps( a4, _mm_load_ps( &invM.cell[8] ) ), v3 = _mm_mul_ps( a4, _mm_load_ps( &invM.cell[12] ) );
		_MM_TRANSPOSE4_PS( v0, v1, v2, v3 );
		__m128 o4 = _mm_add_ps( _mm_add_ps( v0, v1 ), _mm_add_ps( v2, v3 ) );
		__m128 v4 = _mm_mul_ps( b4, _mm_load_ps( &invM.cell[0] ) ), v5 = _mm_mul_ps( b4, _mm_load_ps( &invM.cell[4] ) );
		__m128 v6 = _mm_mul_ps( b4, _mm_load_ps( &invM.cell[8] ) ), v7 = _mm_mul_ps( b4, _mm_load_ps( &invM.cell[12] ) );
		_MM_TRANSPOSE4_PS( v4, v5, v6, v7 );
		__m128 d4 = _mm_add_ps( _mm_add_ps( v4, v5 ), v6 );
		__m128 rd4 = _mm_div_ps( _mm_set1_ps( 1.0f ), d4 ); // _mm_rcp_ps( d4 ); // reduced precision unsufficient?
		// AABB test
		__m128 t1 = _mm_mul_ps( _mm_sub_ps( bmin4, o4 ), rd4 );
		__m128 t2 = _mm_mul_ps( _mm_sub_ps( bmax4, o4 ), rd4 );
		__m128 vmax4 = _mm_max_ps( t1, t2 ), vmin4 = _mm_min_ps( t1, t2 );
		float tmax = min( vmax4.m128_f32[0], min( vmax4.m128_f32[1], vmax4.m128_f32[2] ) );
		float tmin = max( vmin4.m128_f32[0], max( vmin4.m128_f32[1], vmin4.m128_f32[2] ) );
		return tmax > 0 && tmin < tmax && tmin < ray.t;
	#else
		float3 O = TransformPosition_SSE( ray.O4, invM );
		float3 D = TransformVector_SSE( ray.D4, invM );
		float rDx = 1 / D.x, rDy = 1 / D.y, rDz = 1 / D.z;
		float t1 = (b[0].x - O.x) * rDx, t2 = (b[1].x - O.x) * rDx;
		float t3 = (b[0].y - O.y) * rDy, t4 = (b[1].y - O.y) * rDy;
		float t5 = (b[0].z - O.z) * rDz, t6 = (b[1].z - O.z) * rDz;
		float tmin = max( max( min( t1, t2 ), min( t3, t4 ) ), min( t5, t6 ) );
		float tmax = min( min( max( t1, t2 ), max( t3, t4 ) ), max( t5, t6 ) );
		return tmax > 0 && tmin < tmax && tmin < ray.t;
	#endif
	}
	float3 GetNormal( const float3 I ) const
	{
		// transform intersection point to object space
		float3 objI = TransformPosition( I, invM );
		// determine normal in object space
		float3 N = float3( -1, 0, 0 );
		float d0 = fabs( objI.x - b[0].x ), d1 = fabs( objI.x - b[1].x );
		float d2 = fabs( objI.y - b[0].y ), d3 = fabs( objI.y - b[1].y );
		float d4 = fabs( objI.z - b[0].z ), d5 = fabs( objI.z - b[1].z );
		float minDist = d0;
		if (d1 < minDist) minDist = d1, N.x = 1;
		if (d2 < minDist) minDist = d2, N = float3( 0, -1, 0 );
		if (d3 < minDist) minDist = d3, N = float3( 0, 1, 0 );
		if (d4 < minDist) minDist = d4, N = float3( 0, 0, -1 );
		if (d5 < minDist) minDist = d5, N = float3( 0, 0, 1 );
		// return normal in world space
		return TransformVector( N, M );
	}
	float3 GetAlbedo( const float3 I ) const
	{
		return float3( 1, 1, 1 );
	}
#ifdef SPEEDTRIX
	union { float4 b[2]; struct { __m128 bmin4, bmax4; }; };
#else
	float3 b[2];
#endif
	mat4 M, invM;
	int objIdx = -1;
};

// -----------------------------------------------------------
// Quad primitive
// Oriented quad, intended to be used as a light source.
// -----------------------------------------------------------
class Quad
{
public:
	Quad() = default;
	Quad( int idx, float s, mat4 transform = mat4::Identity() )
	{
		objIdx = idx;
		size = s * 0.5f;
		T = transform, invT = transform.FastInvertedTransformNoScale();
	}
	void Intersect( Ray& ray ) const
	{
		const float Oy = invT.cell[4] * ray.O.x + invT.cell[5] * ray.O.y + invT.cell[6] * ray.O.z + invT.cell[7];
		const float Dy = invT.cell[4] * ray.D.x + invT.cell[5] * ray.D.y + invT.cell[6] * ray.D.z;
		const float t = Oy / -Dy;
		if (t < ray.t && t > 0)
		{
			const float Ox = invT.cell[0] * ray.O.x + invT.cell[1] * ray.O.y + invT.cell[2] * ray.O.z + invT.cell[3];
			const float Oz = invT.cell[8] * ray.O.x + invT.cell[9] * ray.O.y + invT.cell[10] * ray.O.z + invT.cell[11];
			const float Dx = invT.cell[0] * ray.D.x + invT.cell[1] * ray.D.y + invT.cell[2] * ray.D.z;
			const float Dz = invT.cell[8] * ray.D.x + invT.cell[9] * ray.D.y + invT.cell[10] * ray.D.z;
			const float Ix = Ox + t * Dx, Iz = Oz + t * Dz;
			if (Ix > -size && Ix < size && Iz > -size && Iz < size)
				ray.t = t, ray.objIdx = objIdx;
		}
	}
	bool IsOccluded( const Ray& ray ) const
	{
		const float Oy = invT.cell[4] * ray.O.x + invT.cell[5] * ray.O.y + invT.cell[6] * ray.O.z + invT.cell[7];
		const float Dy = invT.cell[4] * ray.D.x + invT.cell[5] * ray.D.y + invT.cell[6] * ray.D.z;
		const float t = Oy / -Dy;
		if (t < ray.t && t > 0)
		{
			const float Ox = invT.cell[0] * ray.O.x + invT.cell[1] * ray.O.y + invT.cell[2] * ray.O.z + invT.cell[3];
			const float Oz = invT.cell[8] * ray.O.x + invT.cell[9] * ray.O.y + invT.cell[10] * ray.O.z + invT.cell[11];
			const float Dx = invT.cell[0] * ray.D.x + invT.cell[1] * ray.D.y + invT.cell[2] * ray.D.z;
			const float Dz = invT.cell[8] * ray.D.x + invT.cell[9] * ray.D.y + invT.cell[10] * ray.D.z;
			const float Ix = Ox + t * Dx, Iz = Oz + t * Dz;
			return Ix > -size && Ix < size && Iz > -size && Iz < size;
		}
		return false;
	}
	float3 GetNormal( const float3 I ) const
	{
		// TransformVector( float3( 0, -1, 0 ), T ) 
		return float3( -T.cell[1], -T.cell[5], -T.cell[9] );
	}
	float3 GetAlbedo( const float3 I ) const
	{
		return float3( 10 );
	}
	float size;
	mat4 T, invT;
	int objIdx = -1;
};

// -----------------------------------------------------------
// Torus primitive - Inigo Quilez, ShaderToy 4sBGDy
// -----------------------------------------------------------
class Torus
{
public:
	Torus() = default;
	Torus::Torus( int idx, float a, float b ) : objIdx( idx )
	{
		rc2 = a * a, rt2 = b * b;
		T = invT = mat4::Identity();
		r2 = sqrf( a + b );
	}
	void Intersect( Ray& ray ) const
	{
		// via: https://www.shadertoy.com/view/4sBGDy
		float3 O = TransformPosition_SSE( ray.O4, invT );
		float3 D = TransformVector_SSE( ray.D4, invT );
		// extension rays need double precision for the quadratic solver!
		double po = 1, m = dot( O, O ), k3 = dot( O, D ), k32 = k3 * k3;
		// bounding sphere test
		double v = k32 - m + r2;
		if (v < 0) return;
		// setup torus intersection
		double k = (m - rt2 - rc2) * 0.5, k2 = k32 + rc2 * D.z * D.z + k;
		double k1 = k * k3 + rc2 * O.z * D.z, k0 = k * k + rc2 * O.z * O.z - rc2 * rt2;
		// solve quadratic equation
		if (fabs( k3 * (k32 - k2) + k1 ) < 0.0001)
		{
			swap( k1, k3 );
			po = -1, k0 = 1 / k0, k1 = k1 * k0, k2 = k2 * k0, k3 = k3 * k0, k32 = k3 * k3;
		}
		double c2 = 2 * k2 - 3 * k32, c1 = k3 * (k32 - k2) + k1;
		double c0 = k3 * (k3 * (-3 * k32 + 4 * k2) - 8 * k1) + 4 * k0;
		c2 *= 0.33333333333, c1 *= 2, c0 *= 0.33333333333;
		double Q = c2 * c2 + c0, R = 3 * c0 * c2 - c2 * c2 * c2 - c1 * c1;
		double h = R * R - Q * Q * Q, z;
		if (h < 0)
		{
			const double sQ = sqrt( Q );
			z = 2 * sQ * cos( acos( R / (sQ * Q) ) * 0.33333333333 );
		}
		else
		{
		#if 1
			const double sQ = cbrtFast( sqrt( h ) + fabs( R ) ); // by Jeppe Vroegindeweij
		#else
			const double sQ = cbrt( sqrt( h ) + fabs( R ) ); // pow( sqrt( h ) + fabs( R ), 0.3333333 );
		#endif
			z = copysign( fabs( sQ + Q / sQ ), R );
		}
		z = c2 - z;
		double d1 = z - 3 * c2, d2 = z * z - 3 * c0;
		if (fabs( d1 ) < 1.0e-8)
		{
			if (d2 < 0) return;
			d2 = sqrt( d2 );
		}
		else
		{
			if (d1 < 0) return;
			d1 = sqrt( d1 * 0.5 ), d2 = c1 / d1;
		}
		double t = 1e20;
		h = d1 * d1 - z + d2;
		if (h > 0)
		{
			h = sqrt( h );
			double t1 = -d1 - h - k3, t2 = -d1 + h - k3;
			t1 = (po < 0) ? 2 / t1 : t1, t2 = (po < 0) ? 2 / t2 : t2;
			if (t1 > 0) t = t1;
			if (t2 > 0) t = min( t, t2 );
		}
		h = d1 * d1 - z - d2;
		if (h > 0)
		{
			h = sqrt( h );
			double t1 = d1 - h - k3, t2 = d1 + h - k3;
			t1 = (po < 0) ? 2 / t1 : t1, t2 = (po < 0) ? 2 / t2 : t2;
			if (t1 > 0) t = min( t, t1 );
			if (t2 > 0) t = min( t, t2 );
		}
		float ft = (float)t;
		if (ft > 0 && ft < ray.t) ray.t = ft, ray.objIdx = objIdx;
	}
	bool IsOccluded( const Ray& ray ) const
	{
		// via: https://www.shadertoy.com/view/4sBGDy
		float3 O = TransformPosition_SSE( ray.O4, invT );
		float3 D = TransformVector_SSE( ray.D4, invT );
		float po = 1, m = dot( O, O ), k3 = dot( O, D ), k32 = k3 * k3;
		// bounding sphere test
		float v = k32 - m + r2;
		if (v < 0.0) return false;
		// setup torus intersection
		float k = (m - rt2 - rc2) * 0.5f, k2 = k32 + rc2 * D.z * D.z + k;
		float k1 = k * k3 + rc2 * O.z * D.z, k0 = k * k + rc2 * O.z * O.z - rc2 * rt2;
		// solve quadratic equation
		if (fabs( k3 * (k32 - k2) + k1 ) < 0.01f)
		{
			swap( k1, k3 );
			po = -1, k0 = 1 / k0, k1 = k1 * k0, k2 = k2 * k0, k3 = k3 * k0, k32 = k3 * k3;
		}
		float c2 = 2 * k2 - 3 * k32, c1 = k3 * (k32 - k2) + k1;
		float c0 = k3 * (k3 * (-3 * k32 + 4 * k2) - 8 * k1) + 4 * k0;
		c2 *= 0.33333333333f, c1 *= 2, c0 *= 0.33333333333f;
		float Q = c2 * c2 + c0, R = 3 * c0 * c2 - c2 * c2 * c2 - c1 * c1;
		float h = R * R - Q * Q * Q, z = 0;
		if (h < 0)
		{
			const float sQ = sqrtf( Q );
			z = 2 * sQ * cosf( acosf( R / (sQ * Q) ) * 0.3333333f );
		}
		else
		{
		#if 1
			const float sQ = (float)cbrtFast( sqrtf( h ) + fabsf( R ) );
		#else
			const float sQ = cbrtf( sqrtf( h ) + fabs( R ) ); // powf( sqrtf( h ) + fabs( R ), 0.3333333f );
		#endif
			z = copysign( fabs( sQ + Q / sQ ), R );
		}
		z = c2 - z;
		float d1 = z - 3 * c2, d2 = z * z - 3 * c0;
		if (fabs( d1 ) < 1.0e-4f)
		{
			if (d2 < 0) return false;
			d2 = sqrtf( d2 );
		}
		else
		{
			if (d1 < 0.0) return false;
			d1 = sqrtf( d1 * 0.5f ), d2 = c1 / d1;
		}
		float t = 1e20f;
		h = d1 * d1 - z + d2;
		if (h > 0)
		{
			float t1 = -d1 - sqrtf( h ) - k3;
			t1 = (po < 0) ? 2 / t1 : t1;
			if (t1 > 0 && t1 < ray.t) return true;
		}
		h = d1 * d1 - z - d2;
		if (h > 0)
		{
			float t1 = d1 - sqrtf( h ) - k3;
			t1 = (po < 0) ? 2 / t1 : t1;
			if (t1 > 0 && t1 < ray.t) return true;
		}
		return false;
	}
	float3 GetNormal( const float3 I ) const
	{
		float3 L = TransformPosition( I, invT );
		float3 N = normalize( L * (dot( L, L ) - rt2 - rc2 * float3( 1, 1, -1 )) );
		return TransformVector( N, T );
	}
	float3 Torus::GetAlbedo( const float3 I ) const
	{
		return float3( 1 ); // material.albedo;
	}
	float rt2, rc2, r2;
	int objIdx;
	mat4 T, invT;
	// these are helper functions for the torus code
	// these function will find the cubic root up till a certain accuracy using the newtonian method
	float cbrtfFast( const float n ) const {

		float x1 = n / 10.0f, x2 = 1.0f;
		int turn = 0;
		while (fabs( x1 - x2 ) > 0.00000001 && turn++ < 100)
			x1 = x2, x2 = (2.0f / 3.0f * x1) + (n / (3.0f * x1 * x1));
		return x2;
	}
	double cbrtFast( const double n ) const {

		double x1 = n / 10.0f, x2 = 1.0f;
		int turn = 0;
		while (fabs( x1 - x2 ) > 0.00000001 && turn++ < 100)
			x1 = x2, x2 = (2.0f / 3.0f * x1) + (n / (3.0f * x1 * x1));
		return x2;
	}
};

// -----------------------------------------------------------
// Scene class
// We intersect this. The query is internally forwarded to the
// list of primitives, so that the nearest hit can be returned.
// For this hit (distance, obj id), we can query the normal and
// albedo.
// -----------------------------------------------------------
class Scene
{
public:
	Scene()
	{
		// we store all primitives in one continuous buffer
	#ifdef FOURLIGHTS
		for (int i = 0; i < 4; i++) quad[i] = Quad( 0, 0.5f );	// 0: four light sources
	#else
		quad = Quad( 0, 1 );									// 0: light source
	#endif
		sphere = Sphere( 1, float3( 0 ), 0.6f );				// 1: bouncing ball
		sphere2 = Sphere( 2, float3( 0, 2.5f, -3.07f ), 16 );	// 2: rounded corners
		cube = Cube( 3, float3( 0 ), float3( 1.15f ) );			// 3: cube
		plane[0] = Plane( 4, float3( 2, 0, 0 ), 3 );			// 4: left wall
		plane[1] = Plane( 5, float3( -2, 0, 0 ), 2.99f );		// 5: right wall
		plane[2] = Plane( 6, float3( 0, 2, 0 ), 1 );			// 6: floor
		plane[3] = Plane( 7, float3( 0, -2, 0 ), 2 );			// 7: ceiling
		plane[4] = Plane( 8, float3( 0, 0, 2 ), 3 );			// 8: front wall
		plane[5] = Plane( 9, float3( 0, 0, -2 ), 3.99f );		// 9: back wall
		torus = Torus( 10, 0.8f, 0.25f );						// 10: torus
		torus.T = mat4::Translate( -0.25f, 0, 2 ) * mat4::RotateX( PI / 4 );
		torus.invT = torus.T.Inverted();
	#ifdef USEBVH
		mat4 T = mat4::Translate( float3( 0, 0, -3 ) ) * mat4::Scale( 0.5f );
		bvh = BVH( "../assets/spaceship.obj", T );
	#endif

	#ifdef USEGRID
		mat4 T = mat4::Translate(float3(0, 0, -1)) * mat4::Scale(0.5f);
		grid = Grid("../assets/spaceship.obj", 10, T);
	#endif
		SetTime( 0 );
		// Note: once we have triangle support we should get rid of the class
		// hierarchy: virtuals reduce performance somewhat.
	}
	void SetTime( float t )
	{
		// default time for the scene is simply 0. Updating/ the time per frame 
		// enables animation. Updating it per ray can be used for motion blur.
		animTime = t;
	#ifdef FOURLIGHTS
		// four light sources are stationary
		quad[0].T = mat4::Translate( -1, 1.5f, -1 ), quad[0].invT = quad[0].T.FastInvertedTransformNoScale();
		quad[1].T = mat4::Translate( 1, 1.5f, -1 ), quad[1].invT = quad[1].T.FastInvertedTransformNoScale();
		quad[2].T = mat4::Translate( 1, 1.5f, 1 ), quad[2].invT = quad[2].T.FastInvertedTransformNoScale();
		quad[3].T = mat4::Translate( -1, 1.5f, 1 ), quad[3].invT = quad[3].T.FastInvertedTransformNoScale();
	#else
		// light source animation: swing
		mat4 M1base = mat4::Translate( float3( 0, 2.6f, 2 ) );
		mat4 M1 = M1base * mat4::RotateZ( sinf( animTime * 0.6f ) * 0.1f ) * mat4::Translate( float3( 0, -0.9f, 0 ) );
		quad.T = M1, quad.invT = M1.FastInvertedTransformNoScale();
	#endif
		// cube animation: spin
		mat4 M2base = mat4::RotateX( PI / 4 ) * mat4::RotateZ( PI / 4 );
		mat4 M2 = mat4::Translate( float3( 1.8f, 0, 2.5f ) ) * mat4::RotateY( animTime * 0.5f ) * M2base;
		cube.M = M2, cube.invM = M2.FastInvertedTransformNoScale();
		// sphere animation: bounce
		float tm = 1 - sqrf( fmodf( animTime, 2.0f ) - 1 );
		sphere.pos = float3( -1.8f, -0.4f + tm, 1 );
	}
	float3 GetLightPos() const
	{
	#ifndef FOURLIGHTS
		// light point position is the middle of the swinging quad
		float3 corner1 = TransformPosition( float3( -0.5f, 0, -0.5f ), quad.T );
		float3 corner2 = TransformPosition( float3( 0.5f, 0, 0.5f ), quad.T );
		return (corner1 + corner2) * 0.5f - float3( 0, 0.01f, 0 );
	#else
		// function is not valid when using four lights; we'll return the origin
		return float3( 0 );
	#endif
	}
	float3 RandomPointOnLight( const float r0, const float r1 ) const
	{
	#ifndef FOURLIGHTS
		// get a random position on the swinging quad
		const float size = quad.size;
		float3 corner1 = TransformPosition( float3( -size, 0, -size ), quad.T );
		float3 corner2 = TransformPosition( float3( size, 0, -size ), quad.T );
		float3 corner3 = TransformPosition( float3( -size, 0, size ), quad.T );
		return corner1 + r0 * (corner2 - corner1) + r1 * (corner3 - corner1);
	#else
		// select a random light and use that
		uint lightIdx = (uint)(r0 * 4);
		const Quad& q = quad[lightIdx];
		// renormalize r0 for reuse
		float stratum = lightIdx * 0.25f;
		float r2 = (r0 - stratum) / (1 - stratum);
		// get a random position on the selected quad
		const float size = q.size;
		float3 corner1 = TransformPosition( float3( -size, 0, -size ), q.T );
		float3 corner2 = TransformPosition( float3( size, 0, -size ), q.T );
		float3 corner3 = TransformPosition( float3( -size, 0, size ), q.T );
		return corner1 + r2 * (corner2 - corner1) + r1 * (corner3 - corner1);
	#endif
	}
	float3 RandomPointOnLight( uint& seed ) const
	{
		return RandomPointOnLight( RandomFloat( seed ), RandomFloat( seed ) );
	}
	void GetLightQuad( float3& v0, float3& v1, float3& v2, float3& v3, const uint idx = 0 )
	{
	#ifndef FOURLIGHTS
		// return the four corners of the swinging quad, clockwise, for solid angle sampling
		const Quad& q = quad;
	#else
		// return four corners of the specified light
		const Quad& q = quad[idx];
	#endif
		const float size = q.size;
		v0 = TransformPosition( float3( -size, 0, size ), q.T );
		v1 = TransformPosition( float3( size, 0, size ), q.T );
		v2 = TransformPosition( float3( size, 0, -size ), q.T );
		v3 = TransformPosition( float3( -size, 0, -size ), q.T );
	}
	float3 GetLightColor() const
	{
		return float3( 24, 24, 22 );
	}
	float3 GetAreaLightColor() const
	{
	#ifdef FOURLIGHTS
		return quad[0].GetAlbedo( float3( 0 ) ); // they're all the same color
	#else
		return quad.GetAlbedo( float3( 0 ) );
	#endif
	}
	float GetLightArea() const
	{
	#ifdef FOURLIGHTS
		return sqrf( quad[0].size * 2 ); // all the same size
	#else
		return sqrf( quad.size * 2 );
	#endif
	}
	constexpr float GetLightCount() const
	{
	#ifdef FOURLIGHTS
		return 4; // what did you expect
	#else
		return 1;
	#endif
	}

	void FindNearest( Ray& ray ) const
	{
		// room walls - ugly shortcut for more speed
	#ifdef SPEEDTRIX
		// prefetching
		const float3 spos = sphere.pos;
		const float3 ro = ray.O, rd = ray.D;
		// TODO: the room is actually just an AABB; use slab test
		static const __m128 x4min = _mm_setr_ps( 12, 4, 12, 1e30f );
		static const __m128 x4max = _mm_setr_ps( -11.96f, -8, -15.96f, 1e30f );
		static const __m128 idmin = _mm_castsi128_ps( _mm_setr_epi32( 4, 6, 8, -1 ) );
		static const __m128 idmax = _mm_castsi128_ps( _mm_setr_epi32( 5, 7, 9, -1 ) );
		static const __m128 zero4 = _mm_setzero_ps();
		const __m128 selmask = _mm_cmpge_ps( ray.D4, zero4 );
		const __m128i idx4 = _mm_castps_si128( _mm_blendv_ps( idmin, idmax, selmask ) );
		const __m128 x4 = _mm_blendv_ps( x4min, x4max, selmask );
		const __m128 d4 = _mm_sub_ps( zero4, _mm_mul_ps( _mm_add_ps( ray.O4, x4 ), ray.rD4 ) );
		const __m128 mask4 = _mm_cmple_ps( d4, zero4 );
		const __m128 t4 = _mm_blendv_ps( d4, _mm_set1_ps( 1e34f ), mask4 );
		/* first: unconditional */  ray.t = t4.m128_f32[0], ray.objIdx = idx4.m128i_i32[0];
		if (t4.m128_f32[1] < ray.t) ray.t = t4.m128_f32[1], ray.objIdx = idx4.m128i_i32[1];
		if (t4.m128_f32[2] < ray.t) ray.t = t4.m128_f32[2], ray.objIdx = idx4.m128i_i32[2];
	#else
		if (ray.D.x < 0) PLANE_X( 3, 4 ) else PLANE_X( -2.99f, 5 );
		if (ray.D.y < 0) PLANE_Y( 1, 6 ) else PLANE_Y( -2, 7 );
		if (ray.D.z < 0) PLANE_Z( 3, 8 ) else PLANE_Z( -3.99f, 9 );
	#endif
	#ifdef FOURLIGHTS
	#ifdef SPEEDTRIX
		// efficient four-quad intersection by Jesse Vrooman
		const __m128 t = _mm_div_ps( _mm_add_ps( _mm_set1_ps( ray.O.y ),
			_mm_set1_ps( -1.5 ) ), _mm_xor_ps( _mm_set1_ps( ray.D.y ), _mm_set1_ps( -0.0 ) ) );
		const __m128 Ix = _mm_add_ps( _mm_add_ps( _mm_set1_ps( ray.O.x ),
			_mm_set_ps( 1, -1, -1, 1 ) ), _mm_mul_ps( t, _mm_set1_ps( ray.D.x ) ) );
		const __m128 Iz = _mm_add_ps( _mm_add_ps( _mm_set1_ps( ray.O.z ),
			_mm_set_ps( 1, 1, -1, -1 ) ), _mm_mul_ps( t, _mm_set1_ps( ray.D.z ) ) );
		const static __m128 size = _mm_set1_ps( 0.25f );
		const static __m128 nsize = _mm_xor_ps( _mm_set1_ps( 0.25f ), _mm_set1_ps( -0.0 ) );
		const __m128 maskedT = _mm_and_ps( t, _mm_and_ps(
			_mm_and_ps( _mm_cmpgt_ps( Ix, nsize ), _mm_cmplt_ps( Ix, size ) ),
			_mm_and_ps( _mm_cmpgt_ps( Iz, nsize ), _mm_cmplt_ps( Iz, size ) ) ) );
		if (maskedT.m128_f32[3] > 0) ray.t = maskedT.m128_f32[3], ray.objIdx = 0;
		if (maskedT.m128_f32[2] > 0) ray.t = maskedT.m128_f32[2], ray.objIdx = 0;
		if (maskedT.m128_f32[1] > 0) ray.t = maskedT.m128_f32[1], ray.objIdx = 0;
		if (maskedT.m128_f32[0] > 0) ray.t = maskedT.m128_f32[0], ray.objIdx = 0;
	#else
		for (int i = 0; i < 4; i++) quad[i].Intersect( ray );
	#endif

	#else
		quad.Intersect( ray );
	#endif

	#ifdef SPEEDTRIX // hardcoded spheres, a bit faster this way but very ugly
		{
			// SIMD sphere intersection code by Jesse Vrooman
			const __m128 oc = _mm_sub_ps( ray.O4, sphere.pos4 );
			const float b = _mm_dp_ps( oc, ray.D4, 0x71 ).m128_f32[0];
			const float d = b * b - (_mm_dp_ps( oc, oc, 0x71 ).m128_f32[0] - 0.36f);
			if (d > 0)
			{
				const float t = -b - sqrtf( d );
				const bool hit = t < ray.t && t > 0;
				if (hit) { ray.t = t, ray.objIdx = 1; }
			};
		}
		//{
		//	// SIMD sphere intersection code by Jesse Vrooman
		//	const static __m128 s4 = _mm_setr_ps( 0, 2.5f, -3.07f, 4 );
		//	const __m128 oc = _mm_sub_ps( ray.O4, s4 );
		//	const float b = _mm_dp_ps( oc, ray.D4, 0x71 ).m128_f32[0];
		//	const float d = b * b - (_mm_dp_ps( oc, oc, 0x71 ).m128_f32[0] - 64.0f);
		//	if (d > 0)
		//	{
		//		const float t = sqrtf( d ) - b;
		//		const bool hit = t < ray.t && t > 0;
		//		if (hit) { ray.t = t, ray.objIdx = 2; }
		//	};
		//}
	#else
		sphere.Intersect( ray );
		sphere2.Intersect( ray );
	#endif
		cube.Intersect( ray );
		torus.Intersect( ray );

	#ifdef USEBVH
		bvh.Intersect( ray );
	#endif

	#ifdef USEGRID
		grid.Intersect(ray);
	#endif
	}

	bool IsOccluded( const Ray& ray ) const
	{
		if (cube.IsOccluded( ray )) 
			return true;
	#ifdef SPEEDTRIX
		const float3 oc = ray.O - sphere.pos;
		const float b = dot( oc, ray.D ), c = dot( oc, oc ) - (0.6f * 0.6f);
		const float d = b * b - c;
		if (d > 0)
		{
			const float t = -b - sqrtf( d );
			const bool hit = t < ray.t && t > 0;
			if (hit) return true;
		}
	#else
		if (sphere.IsOccluded( ray )) return true;
	#endif
	#ifdef FOURLIGHTS
		for (int i = 0; i < 4; i++) if (quad[i].IsOccluded( ray )) return true;
	#else
		if (quad.IsOccluded( ray )) return true;
	#endif
		
		if (torus.IsOccluded( ray )) return true;
	
	#ifdef USEBVH
		Ray shadow = ray;
		shadow.t = 1e34f;
		bvh.Intersect( shadow );

		if (shadow.objIdx >= 1000) 
			return true;
	#endif

	#ifdef USEGRID
		Ray shadow = ray;
		shadow.t = 1e34f;
		grid.Intersect(shadow);

		if (shadow.objIdx >= 1000)
			return true;
	#endif

		return false; // skip planes and rounded corners
	}

	float3 GetNormal( const int objIdx, const float3 I, const float3 wo ) const
	{
		// we get the normal after finding the nearest intersection:
		// this way we prevent calculating it multiple times.
		if (objIdx == -1) return float3( 0 ); // or perhaps we should just crash
		float3 N;
	#ifdef FOURLIGHTS
		if (objIdx == 0) N = quad[0].GetNormal( I ); // they're all oriented the same
	#else
		if (objIdx == 0) N = quad.GetNormal(I);
#endif
		else if (objIdx == 1) N = sphere.GetNormal(I);
		else if (objIdx == 2) N = sphere2.GetNormal(I);
		else if (objIdx == 3) N = cube.GetNormal(I);
		else if (objIdx == 10) N = torus.GetNormal(I);
#ifdef USEBVH
		else if (objIdx >= 1000) N = bvh.GetNormal(objIdx - 1000);
#endif

#ifdef USEGRID
		else if (objIdx >= 1000) 
			N = grid.GetNormal(objIdx - 1000);
#endif
		else
		{
			// faster to handle the 6 planes without a call to GetNormal
			N = float3( 0 );
			N[(objIdx - 4) / 2] = 1 - 2 * (float)(objIdx & 1);
		}
		if (dot( N, wo ) > 0) N = -N; // hit backside / inside
		return N;
	}

	float3 GetAlbedo( int objIdx, float3 I ) const
	{
		if (objIdx == -1) return float3( 0 ); // or perhaps we should just crash
	#ifdef FOURLIGHTS
		if (objIdx == 0) return quad[0].GetAlbedo( I ); // they're all the same
	#else
		if (objIdx == 0) return quad.GetAlbedo( I );
	#endif
		if (objIdx == 1) return sphere.GetAlbedo( I );
		if (objIdx == 2) return sphere2.GetAlbedo( I );
		if (objIdx == 3) return cube.GetAlbedo( I );
		if (objIdx == 10) return torus.GetAlbedo( I );
	#ifdef USEBVH
		if (objIdx >= 1000) return float3( 0.7f, 0.2f, 0.2f );
	#endif

	#ifdef USEGRID
			if (objIdx >= 1000) 
				return float3(0.7f, 0.2f, 0.2f);
	#endif
		return plane[objIdx - 4].GetAlbedo( I );
		// once we have triangle support, we should pass objIdx and the bary-
		// centric coordinates of the hit, instead of the intersection location.
	}

	float GetReflectivity( int objIdx, float3 I ) const
	{
		if (objIdx == 1 /* ball */) return 1;
		if (objIdx == 6 /* floor */) return 0.3f;
		return 0;
	}

	float GetRefractivity( int objIdx, float3 I ) const
	{
		return (objIdx == 3 || objIdx == 10) ? 1.0f : 0.0f;
	}

	float3 GetAbsorption( int objIdx )
	{
		return objIdx == 3 ? float3( 0.5f, 0, 0.5f ) : float3( 0 );
	}
	__declspec(align(64)) // start a new cacheline here
		float animTime = 0;

#ifdef FOURLIGHTS
	Quad quad[4];
#else
	Quad quad, dummyQuad1, dummyQuad2, dummyQuad3;
#endif

	Sphere sphere;
	Sphere sphere2;
	Cube cube;
	Plane plane[6];
	Torus torus;

#ifdef USEBVH
	BVH bvh;
#endif

#ifdef USEGRID
	Grid grid;
#endif
};

}