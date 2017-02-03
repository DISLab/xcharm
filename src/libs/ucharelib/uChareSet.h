#ifndef __uChareSet_h__
#define __uChareSet_h__

#include <Aggregator.h>
#include <Reduction.h>
/**
 * Macros
 */

/**
 * uChareAttr class is used to pass attributes to uChare  
 */
template <class User_uChare, class User_uChare_Proxy, class User_uChare_Base>
struct uChareAttr {
	CmiUInt8 index;
	uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> *uchareset;
	uChareAttr(CmiUInt8 index, uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> *uchareset) :
		index(index), uchareset(uchareset) {}
};

/**
 * uChare class is a base class for all user uchares
 */
template <class User_uChare, class User_uChare_Proxy, class User_uChare_Base>
class uChare {
	private:
		uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> *uchareset;
		bool locked;

	protected:
		CmiUInt8 thisIndex;
		//uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> &thisProxy;
		//User_uChare_Proxy &thisProxy;

		uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> &thisProxy;

	public:
		uChare(const uChareAttr<User_uChare, User_uChare_Proxy, User_uChare_Base> &attr) : 
			thisProxy(*attr.uchareset), locked(false) {
			thisIndex = attr.index;
			uchareset = attr.uchareset;
		}

		void lock() { locked = true; }

		void unlock() { locked = false; }

		bool isLocked() { return locked; }

		void contribute(const CkCallback &cb) {
			uchareset->callContribute(cb);
		}

		void contribute(int dataSize, const void *data, CkReduction::reducerType type, const CkCallback &cb) {
			uchareset->callContribute(dataSize, data, type, cb);
		}

		void contribute(CmiUInt8 n, CkReduction::reducerType type, const CkCallback &cb) {
			uchareset->callContribute(n, type, cb);
		}

		void pup(PUP::er &p) {
			CkAbort("This method is not implemented");
			p | thisIndex;
			//TODO:...
		}

		// old api methods
		void run() {}

		void run(int phase) {}

		void exit() {}

		void done(const CkCallback &userCb) {
			uchareset->done(userCb);
		}

		uChare<User_uChare, User_uChare_Proxy, User_uChare_Base> & operator= 
			(const uChare<User_uChare, User_uChare_Proxy, User_uChare_Base> &rhs) {
			CkAbort("uChare:: operator= called\n");
			return *this;
		}
};

/**
 * uChare_Proxy class
 */
template <class User_uChare, class User_uChare_Proxy, class User_uChare_Base>
class uChare_Proxy {
	typedef void (uChare_Proxy::*UnpackFunction)(const Message & m);
	typedef void (User_uChare_Proxy::*UnpackFuncType)(const Message & m);
	private:
		std::vector<std::pair<bool, UnpackFuncType> > registered_funcs;
		uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> *uchareset;
		Aggregator *aggregator_; // TODO: rename aggregator_ to aggregator
		
	protected:
		std::size_t dst;
		Aggregator::Type aggregatorType;

	public:
		uChare_Proxy() {}
		uChare_Proxy(uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> *uchareset, Aggregator::Type aggregatorType = Aggregator::TRAM) : 
			uchareset(uchareset), aggregatorType(aggregatorType) {

			// creating of aggregator should be done in init because of the PUP::er method
			/*if (aggregatorType == Aggregator::TRAM)
				aggregator_ = new TramAggregator<User_uChare, User_uChare_Proxy, User_uChare_Base>(); 
			else
				aggregator_ = new NaiveAggregator<User_uChare, User_uChare_Proxy, User_uChare_Base>();*/ 
		}

		void init(const CProxy_ArrayMeshStreamer<Message, int, 
				uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base>, SimpleMeshRouter> &aggregator) {

			if (aggregatorType == Aggregator::TRAM) {
				aggregator_ = new TramAggregator<User_uChare, User_uChare_Proxy, User_uChare_Base>(registered_funcs, *uchareset); 
				TramAggregator<User_uChare, User_uChare_Proxy, User_uChare_Base> * tramAggr =
					static_cast<TramAggregator<User_uChare, User_uChare_Proxy, User_uChare_Base> *> (aggregator_); //->init(aggregator);
				tramAggr->init(aggregator);
			}
		}

		uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> & getProxy() {
			return *uchareset;
		}
		
		void clearEntryFunctions() { 
			registered_funcs.clear(); 
		}

		void registerEntryFunction(UnpackFuncType f, bool isReentrant = true) { 
			registered_funcs.push_back(std::pair<bool,UnpackFuncType>(isReentrant,f)); 
		}

		int getFunctionId (UnpackFuncType f) {
			for (int i = 0; i < registered_funcs.size(); i++) {
				if (f == registered_funcs[i].second)
					return i;
			}
			CkAbort("Member function must be registered");
			return -1;
		}

		void set_dst(std::size_t dst) { 
			this->dst = dst; 
		}

		void addNewMsg(Message & m) {
			m.destination = this->dst;
			aggregator_->send(m);
		}

		void process (const Message & m) {
			aggregator_->process(m);
		}

		void flush() {
			aggregator_->flush();
		}

		void pup(PUP::er &p) {
			p | aggregatorType;
		}
};

/**
 * uChareSet template
 * uChareSet ...
 */
template <class User_uChare, class User_uChare_Proxy, class User_uChare_Base>
class uChareSet : public CBase_uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> {
	public:
		// FIXME: must be private!!!
			std::vector<User_uChare> uchares;
			User_uChare_Proxy proxy;

	private:
			int dst;
			CkCallback cb;
			CmiUInt8 contrib_counter;
			CmiUInt8 contrib_max;
			CmiUInt8 contrib_accum;
			CmiUInt8 done_counter;
			ReductionMsg *redMsg;

			CkCallback userCb;


			// TRAM support
			bool use_tram;

	public:	
			uChareSet(bool use_tram) : proxy(this), 
				contrib_counter(0), contrib_accum(0), done_counter(0), use_tram(use_tram) {
				DEBUGGING("%d: uChareSet constructed on pe %d\n", this->thisIndex, CkMyPe());

				int localStartIndex = this->thisIndex * N_uChares/N_uChareSets;
				contrib_max = N_uChares/N_uChareSets;

				// create local uchares 
				for (int i = 0; i < N_uChares/N_uChareSets; i++) {
					uchares.push_back(User_uChare(
								uChareAttr<User_uChare, User_uChare_Proxy, User_uChare_Base>(localStartIndex++, this)));
				}
			}

			uChareSet(CkMigrateMessage *m) : proxy(this) {}

			void init(const CProxy_ArrayMeshStreamer<Message, int, 
					uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base>, SimpleMeshRouter> &aggregator) {
				DEBUGGING("%d(%d): init called\n", this->thisIndex, CmiMyPe());
				proxy.init(aggregator);
			}

			int getPe() { return CmiMyPe(); }

			User_uChare_Base *operator [] (std::size_t i) { 
					//FIXME:!!!!!!!!!!
				/*if (uChareDistribution::isLocal(i, this->thisIndex)) {
					User_uChare *uch = 	&uchares[uChareDistribution::getLocaluChareId(i)];
					if (uch->isLocked()) { 
						//CkPrintf("uchare is locked!\n");
						proxy.set_dst(i);
						return &proxy;
					}
					else
						return &uchares[uChareDistribution::getLocaluChareId(i)];
				}
				else*/ {
					proxy.set_dst(i);
				 	return &proxy;
				}	
			}

			User_uChare_Base *getLocalUChare (std::size_t i) {
				if (!uChareDistribution::isLocal(i, this->thisIndex))
					CkAbort("getLocalUChare error");
				return &uchares[uChareDistribution::getLocaluChareId(i)];
			}

			void sendAgregatedMessage(std::size_t dst, AggregatedMessage *m) {
				DEBUGGING("%d(%d): uChareSet sendAgregatedMessage to %d (msg size = %d)\n", this->thisIndex, CkMyPe(), dst, m->messages.size());
				this->thisProxy[dst].recvAggregatedMsg(m);
			}

			/*entry*/ void recvAggregatedMsg(AggregatedMessage *m) {
				DEBUGGING("recvAggregatedMsg called\n");
				//FIXME!!!: proxy.recvAggregatedMsg(m);
				delete m;
			}

			void run() {
				DEBUGGING("%d(pe%d): uChareSet::run is called\n", this->thisIndex, CkMyPe());
					for (typename std::vector<User_uChare>::iterator i = uchares.begin(); i != uchares.end(); i++)
						i->run();

				//proxy.flush();
			}

			/*entry*/ void run(CkCallback &cb, std::size_t index=0, int runAll=1) {
				DEBUGGING("%d(pe%d): uChareSet::run is called\n", this->thisIndex, CkMyPe());
				userCb = cb;

				if (runAll)
					for (typename std::vector<User_uChare>::iterator i = uchares.begin(); i != uchares.end(); i++)
						i->run();
				else {
					if (uChareDistribution::isLocal(index, this->thisIndex))
						uchares[uChareDistribution::getLocaluChareId(index)].run();
				}
				//this->contribute(0,0,CkReduction::concat,cb);
				proxy.flush();
			}


			/*entry*/ void run(int phase, CkCallback &cb, std::size_t index=0, int runAll=1) {
				DEBUGGING("%d(pe%d): uChareSet::run is called\n", this->thisIndex, CkMyPe());
				userCb = cb;

				if (runAll)
					for (typename std::vector<User_uChare>::iterator i = uchares.begin(); i != uchares.end(); i++)
						i->run(phase);
				else {
					if (uChareDistribution::isLocal(index, this->thisIndex))
						uchares[uChareDistribution::getLocaluChareId(index)].run(phase);
				}
				//this->contribute(0,0,CkReduction::concat,cb);
				proxy.flush();
			}
			
			void exit() {}

			/*entry*/void flush(CkCallback &cb) {
				proxy.flush();
				this->contribute(0,0,CkReduction::concat,cb);
			}

			void flush() {
				proxy.flush();
			}

			void done(const CkCallback &userCb) {
				if (++done_counter == uchares.size()) {
					proxy.flush();
					this->contribute(0,0,CkReduction::concat,userCb);
					done_counter = 0;
				}
			}
			
			/*void print() {
				for (typename std::vector<User_uChare>::iterator i = uchares.begin(); i != uchares.end(); i++)
				 i->print();	
			} */

			std::size_t getId() { return this->thisIndex; }

			void callContribute(const CkCallback &cb) {
				if (cb.isInvalid()) {
					CkAbort("Invalid callback!");
				}

				if (++contrib_counter == contrib_max) {
					this->contribute(cb);
					contrib_counter = 0;
				}
			}

			void callContribute(int dataSize, const void *data, CkReduction::reducerType type, const CkCallback &cb) {
				if (cb.isInvalid()) {
					CkAbort("Invalid callback!");
				}

				if (contrib_counter > 0)
					redMsg->reduce(dataSize, data, type, cb);
				else
					redMsg = new ReductionMsg(dataSize, data, type, cb);

				if (++contrib_counter == contrib_max) {
					this->contribute(redMsg->dataSize, redMsg->data, redMsg->type, redMsg->cb);
					contrib_counter = 0;
					delete redMsg;
				}
			}

			void callContribute(CmiUInt8 n, CkReduction::reducerType type, const CkCallback &cb) {
				//FIXME: dirty hack!!!
				if (cb.isInvalid()) {
					CkAbort("Invalid callback!");
				}
				contrib_accum += n;

				if (++contrib_counter == contrib_max) {
					this->contribute(sizeof(CmiUInt8), &contrib_accum, type, cb);
					contrib_accum = 0;
					contrib_counter = 0;
				}
			}

			/*void gatherPendingMessagesNum(const CkCallback &cb) {
				CmiUInt8 c = proxy.getPendingMessageNum();
				this->contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long, cb); 
			}*/

			void process (const Message &m) {
				proxy.process(m);
			}

			uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> & operator=
					(const uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> &) {
				CkAbort("uChareSet:: operator= called\n");
			}
};


/**
 * uChareArrayOpts
 */
template <class User_uChare, class User_uChare_Proxy, class User_uChare_Base>
struct uChareArrayOpts {
	int nElements, nChareSets;

  uChareArrayOpts(int nElements, int nChareSets) :
		nElements(nElements), nChareSets(nChareSets) {}
};

/**
 * uChareArray template
 * uChareArray ...
 */
template <class User_uChare, class User_uChare_Proxy, class User_uChare_Base>
class uChareArray {
	protected:
		int nElements; // N_uChares
		int nChareSets; // nChareSets; 
		User_uChare_Proxy user_proxy; // needed to communicate with uchares from main

		CProxy_uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> uchareset_proxy;

		// for TRAM support
		int dims[2];
		CProxy_ArrayMeshStreamer<Message, int, 
			uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base>, SimpleMeshRouter> aggregator_;

	public:
		uChareArray() {}
		uChareArray(int nElements, int nChareSets):
					nElements(nElements), nChareSets(nChareSets), user_proxy(NULL) {
			N_uChares = nElements;
			N_uChareSets = nChareSets;
		  uchareset_proxy = CProxy_uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base>
				::ckNew(/*use_tram*/true, N_uChareSets);

			dims[0] = CkNumNodes();
			dims[1] = CkNumPes() / CkNumNodes();
			CkPrintf("Aggregation topology: %d %d\n", dims[0], dims[1]);

			aggregator_ =
				CProxy_ArrayMeshStreamer<Message, int, uChareSet<User_uChare, User_uChare_Proxy, 
					User_uChare_Base>, SimpleMeshRouter> 
					::ckNew(1024, 2, dims, uchareset_proxy, 0 /*1*/);

			// need to pass aggregator to uChareSets somehow
			uchareset_proxy.init(aggregator_);
			user_proxy.init(aggregator_);
		}

		static uChareArray ckNew(const uChareArrayOpts<User_uChare, User_uChare_Proxy, User_uChare_Base> &opts) { 
			uChareArray uchareArray(opts.nElements, opts.nChareSets);
			return uchareArray; 
		}


		static uChareArray ckNew(const CmiUInt8 & nElements) { 
			uChareArray uchareArray(nElements, CkNumPes());
			return uchareArray; 
		}

		void init() {
			CkAbort("uChareArray::init is deprecated");
		}

		// TODO: Do I realy need run?
		void run(const CkCallback &cb) {
			CkCallback startCb(CkIndex_uChareSet<User_uChare, User_uChare_Proxy, 
					User_uChare_Base>::run(), uchareset_proxy);
			CkCallback endCb(CkIndex_uChareSet<User_uChare, User_uChare_Proxy, 
					User_uChare_Base>::exit(), uchareset_proxy);
			// TODO: find out which of the init method is best for this
			aggregator_.init(uchareset_proxy.ckGetArrayID(), startCb, endCb, -1, true /*false*/);

			//this->contribute(0, 0, CkReduction::concat, cb);
			CkStartQD(cb);
		}


	public:
		void pup(PUP::er &p) {
			p | nElements;
			p | nChareSets;
			p | user_proxy;
			p | uchareset_proxy;
			p | dims[0];
			p | dims[1];
			p | aggregator_;
			user_proxy.init(aggregator_);
		}	
};


//CProxy_ArrayMeshStreamer<AggregatedMessage, int, uChareSet,
//	SimpleMeshRouter> aggregator;

#endif
