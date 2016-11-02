#ifndef __DataTypes_h__
#define __DataTypes_h__
#include "charm++.h"

namespace GraphLib {
	typedef CmiUInt8 VertexId;
	typedef enum {SingleVertex = 0, MultiVertex} VertexMapping;
	typedef enum {Directed = 0, Undirected} GraphType;
	typedef enum {Charm = 0, Tram, uChareLib} TransportType;
	typedef enum {Kronecker = 0 /*Kronecker will be deprecated*/, RMAT, SSCA, Random} GraphGeneratorType;
}
#endif
