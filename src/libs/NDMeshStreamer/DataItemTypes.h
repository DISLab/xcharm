#ifndef DATA_ITEM_TYPES_H
#define DATA_ITEM_TYPES_H

#define CHUNK_SIZE 256

template<class dtype, class itype>
class ArrayDataItem {

public:
  itype arrayIndex;
  int sourcePe;
  dtype dataItem;

  ArrayDataItem(itype i, int srcPe, const dtype d)
    : arrayIndex(i), sourcePe(srcPe), dataItem(d) {}
  ArrayDataItem() {}
  int size(); /*	{ 
		return sizeof(itype) + sizeof(int) + dataItem.size(); 
	}*/

	void pack(void *buf); /* {
		char *p = (char *)buf;
		memcpy(p, &arrayIndex, sizeof(itype));
		p += sizeof(itype);
		memcpy(p, &sourcePe, sizeof(int));
		p += sizeof(int);
		dtype::pack(&dataItem, p);
	}*/

	void unpack(void *buf); /* {
		char *p = (char *)buf;
		memcpy(&arrayIndex, p, sizeof(itype));
		p += sizeof(itype);
		memcpy(&sourcePe, p, sizeof(int));
		p += sizeof(int);
		dtype::unpack(&dataItem, p);
	}*/
};

template <class dtype, class itype>
int ArrayDataItem<dtype, itype>::size() {
	return sizeof(itype) + sizeof(int) + dataItem.size(); 
}

/*template <>
int ArrayDataItem<CmiUInt8, int>::size() {
	return sizeof(int) + sizeof(int) + sizeof(CmiUInt8); 
}*/

template <class dtype, class itype>
void ArrayDataItem<dtype, itype>::pack(void *buf) {
	char *p = (char *)buf;
	memcpy(p, &arrayIndex, sizeof(itype));
	p += sizeof(itype);
	memcpy(p, &sourcePe, sizeof(int));
	p += sizeof(int);
	dtype::pack(&dataItem, p);
}

/*template <>
void ArrayDataItem<CmiUInt8, int>::pack(void *buf) {
	char *p = (char *)buf;
	memcpy(p, &arrayIndex, sizeof(int));
	p += sizeof(int);
	memcpy(p, &sourcePe, sizeof(int));
	p += sizeof(int);
	memcpy(p, &dataItem, sizeof(CmiUInt8));
	p += sizeof(CmiUInt8);
}*/

template <class dtype, class itype>
void ArrayDataItem<dtype, itype>::unpack(void *buf) {
	char *p = (char *)buf;
	memcpy(&arrayIndex, p, sizeof(itype));
	p += sizeof(itype);
	memcpy(&sourcePe, p, sizeof(int));
	p += sizeof(int);
	dtype::unpack(&dataItem, p);
}

/*template <>
void ArrayDataItem<CmiUInt8, int>::unpack(void *buf) {
	char *p = (char *)buf;
	memcpy(&arrayIndex, p, sizeof(int));
	p += sizeof(int);
	memcpy(&sourcePe, p, sizeof(int));
	p += sizeof(int);
	memcpy(&dataItem, p, sizeof(CmiUInt8));
	p += sizeof(CmiUInt8);
}*/

class ChunkDataItem {

public:
  short chunkSize;
  int bufferNumber;
  int sourcePe; 
  int chunkNumber; 
  int numChunks;  
  int numItems;
  char rawData[CHUNK_SIZE];
  
  ChunkDataItem& operator=(const ChunkDataItem &rhs) {
    
    if (this != &rhs) {      
      chunkSize = rhs.chunkSize; 
      bufferNumber = rhs.bufferNumber;
      sourcePe = rhs.sourcePe;
      chunkNumber = rhs.chunkNumber; 
      numChunks = rhs.numChunks;
      numItems = rhs.numItems;
      memcpy(rawData, rhs.rawData, CHUNK_SIZE);
    }

    return *this;
  }
  
};

#endif
