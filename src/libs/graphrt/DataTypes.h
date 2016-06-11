#ifndef __DataTypes_h__
#define __DataTypes_h__
#include "charm++.h"

namespace GraphLib {
	typedef CmiUInt8 VertexId;
	typedef enum {Directed = 0, Undirected} GraphType;
	typedef enum {Charm = 0, Tram, uChareLib} TransportType;
	typedef enum {Kronecker = 0, SSCA2} GraphGeneratorType;
}
#endif
