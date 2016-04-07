#ifndef __GraphIO_h__
#define __GraphIO_h__

enum class GraphIOType {Generator, File}; 

template <class V>
class GraphIO {
	public:
		GraphIO() {}
		~GraphIO() {}
		virtual uint64_t getVertexNum() = 0;
		virtual uint64_t getEdgesNum() = 0;
		virtual uint64_t getVertex(V &v) = 0; 
};

#endif
