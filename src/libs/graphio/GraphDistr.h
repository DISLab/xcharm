#ifndef __GraphDistr_h__
#define __GraphDistr_h__ 
#include <cstdint>

// Specify distribution of vertecis on chares, not
// chares always
enum class DistrMode {Block /*default*/, Cyclic};

inline uint64_t toChareId(uint64_t vertexId, uint64_t nVertex, uint16_t nPEs, DistrMode mode) {
	if (mode == DistrMode::Block) 
		return vertexId;
	else if (mode == DistrMode::Cyclic)
		return (vertexId % nPEs) * (nVertex / nPEs) + vertexId % (nVertex / nPEs);
	return -1;
}

inline uint16_t toPEId(uint64_t vertexId, uint64_t nVertex, uint16_t nPEs, DistrMode mode) {
	if (mode == DistrMode::Block) 
		return vertexId / (nVertex / nPEs);
	else if (mode == DistrMode::Cyclic)
		return vertexId % nPEs;
	return -1;
}

inline uint64_t toVertexId(uint64_t chareId, uint64_t nVertex, uint16_t nPEs, DistrMode mode) {
	if (mode == DistrMode::Block)
		return chareId;
	else if (mode == DistrMode::Cyclic)
		return (chareId % nPEs) * (nVertex / nPEs) + chareId % (nVertex / nPEs);
	return -1; 
} 


#endif
