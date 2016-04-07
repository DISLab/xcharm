#ifndef __Reduction_h__
#define __Reduction_h__

class ReductionMsg {
	public:
		int dataSize;
		void *data;
		CkReduction::reducerType type;
		CkCallback cb;

		ReductionMsg(int dataSize, const void *srcData, CkReduction::reducerType type, const CkCallback &cb) :
			dataSize(dataSize), type(type), cb(cb) {
				data = malloc(dataSize);
				memcpy(data, srcData, dataSize);
		}
		~ReductionMsg() {
			free(data);
		}
		void reduce(int dataSize, const void *srcData, CkReduction::reducerType type, const CkCallback &cb) {
			if ((dataSize != this->dataSize) || (type != this->type) /*|| (cb != this->cb)*/) 
				CkAbort("ReductionMsg reduce mismatch");
			switch (type) {
				case CkReduction::sum_long:
					*(long long *)data += *(long long *)srcData;
					break;
				default:
					CkAbort("Reduction type is not supported");
			}
		}
};

#endif
