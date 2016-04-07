#ifndef __Aggregator_h__
#define __Aggregator_h__
#include <vector>

/**
 * Base class for Aggregators
 */
class Aggregator {
	public:
		typedef enum {Naive, TRAM} Type; 
		virtual void send(const Message & m) = 0;
		virtual void flush() = 0;
		virtual void process(const Message & m) = 0;
};

inline void operator|(PUP::er &p,Aggregator::Type &t) {
	char x = (char)t;
	p(x);
	t = (Aggregator::Type)x;
}
/**
 * Naive aggregator
 */ 
#define AGGREGATED_MESSAGE_MAX_SIZE	256
template <class User_uChare, class User_uChare_Proxy, class User_uChare_Base>
class NaiveAggregator : public Aggregator {
	private:
		int aggregated_message_max_size;
		int pending_msg_count;
		std::vector<AggregatedMessage *> aggregated_messages;
		uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> *uchareset;

		int dst; //FIXME: remove this parameter
	public:
		NaiveAggregator() : aggregated_messages(N_uChareSets), pending_msg_count(0), aggregated_message_max_size(AGGREGATED_MESSAGE_MAX_SIZE) {
			for (int i = 0; i < N_uChareSets; i++)
				aggregated_messages[i] = new AggregatedMessage();	
		}
		void send(const Message & m) {}
		void flush() {}
		
	private:
		void addNewMsgAggrTables(Message & m) {
			std::size_t ucharesetId = uChareDistribution::getuChareSetId(this->dst);
			if (!(ucharesetId < N_uChareSets))
				CkAbort("Invalid uChareSet identifier");
			AggregatedMessage *aggr = aggregated_messages[ucharesetId];
			aggr->addNewMsg(m);
			pending_msg_count++;

			DEBUGGING("%d(%d): addNewMsg: uchareId=%d, setId=%d,  message size = %d\n", uchareset.getId(), CkMyPe(), 
					this->dst, uChareDistribution::getuChareSetId(this->dst), aggr->size());

			if (aggr->size() == aggregated_message_max_size) {
				 DEBUGGING("%d(%d): flush: send aggregated msg to %d, msg count is %d\n", 
						 uchareset.getId(), CkMyPe(), uChareDistribution::getuChareSetId(this->dst), aggr->size());

				 pending_msg_count -= aggr->size();
				 uchareset->sendAgregatedMessage(uChareDistribution::getuChareSetId(this->dst), aggr);	
				 aggregated_messages[uChareDistribution::getuChareSetId(this->dst)] = new AggregatedMessage();
			}	
		}
		void flush_() {
			for (int i = 0; i < N_uChareSets; i++)
				if ((i != uchareset->getId()) && !aggregated_messages[i]->messages.empty()) {
					DEBUGGING("%d(%d): flush: send aggregated msg to %d, msg count is %d\n", uchareset.getId(), CkMyPe(), i, aggregated_messages[i]->messages.size());
					pending_msg_count -= aggregated_messages[i]->size();
					uchareset->sendAgregatedMessage(i, aggregated_messages[i]);	
					aggregated_messages[i] = new AggregatedMessage();	
				}
		}
		int getPendingMessageNum() { return pending_msg_count; }

		void recvAggregatedMsg(AggregatedMessage *m) {
			typedef typename std::vector<Message>::iterator Iterator; 
			typedef void (* UnpackFunction)(const Message & m);

			DEBUGGING("vs%d.pe%d: recvAggregatedMsg: messages.size = %d\n", uchareset.getId(), CmiMyPe(), m->messages.size());
			for (Iterator i = m->messages.begin(); i != m->messages.end(); i++) {
				//FIXME!!!: UnpackFunction unpack_func = static_cast<UnpackFunction>(epTable[i->registeredFunctionId].second);
				//FIXME!!!: (this->*unpack_func)(*i);
			}	
		}

		void process(const Message & m) {}
};

/**
 * TRAM aggregator
 */ 
template <class User_uChare, class User_uChare_Proxy, class User_uChare_Base>
class TramAggregator : public Aggregator {
	private:
		typedef CProxy_ArrayMeshStreamer<Message, int, 
			uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base>, SimpleMeshRouter> 
				CProxy_ArrayMeshStreamerT;
		typedef ArrayMeshStreamer<Message, int, 
			uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base>, SimpleMeshRouter>
				ArrayMeshStreamerT;
		CProxy_ArrayMeshStreamerT globalAggregator;
		ArrayMeshStreamerT *localAggregator;

		typedef void (User_uChare_Proxy::*UnpackFunction)(const Message & m);
		typedef std::vector<std::pair<bool, UnpackFunction> > EpTable;
		const EpTable & epTable;

		typedef uChareSet<User_uChare, User_uChare_Proxy, User_uChare_Base> uChareSetT; 
		uChareSetT & uchareset;
		User_uChare_Proxy * proxy;

		bool yieldFlag_;
		int yieldCount_;
		int oldsize;
		std::vector<Message> delayedMessages;

	public:
		TramAggregator(const EpTable & epTable, uChareSetT & uchareset) : 
			epTable(epTable), uchareset(uchareset), yieldFlag_(true), yieldCount_(0), oldsize(0) {
				proxy = &uchareset.proxy;
		}

		void init(const CProxy_ArrayMeshStreamerT &aggregator) {
			globalAggregator = aggregator;
			localAggregator = aggregator.ckLocalBranch(); 
		}

		void send(const Message & m) {
			localAggregator->insertData(m,uChareDistribution::getuChareSetId(m.destination));
		}

		void flush() {
			DEBUGGING("%d(%d): flush called\n", /*FIXME: getuChareSet()->getId()*/ 333, CkMyPe());
			// scan and execute delayed messages
			for (std::vector<Message>::iterator it = delayedMessages.begin();	it != delayedMessages.end(); ) {
				const Message &m = *it;
				UnpackFunction unpack_func = static_cast<UnpackFunction>(epTable[m.registeredFunctionId].second);
				bool isReentrant = epTable[m.registeredFunctionId].first; 
	
				User_uChare &uchare = uchareset.uchares[uChareDistribution::getLocaluChareId(m.destination)];
				
				if (/*isReentrant ||*/ !uchare.isLocked()) { 
					if (!isReentrant) uchare.lock();
					(proxy->*unpack_func)(m);
					if (!isReentrant) uchare.unlock();

					delayedMessages.erase(it);
				} else
					it++;
			}
			//???
			localAggregator->done();
		}

		void process(const Message & m) {
			DEBUGGING("vs%d.pe%d: process: received message\n", uchareset.getId(), CmiMyPe());
			UnpackFunction unpack_func = static_cast<UnpackFunction>(epTable[m.registeredFunctionId].second);
			bool isReentrant = epTable[m.registeredFunctionId].first; 

      if (yieldFlag_ && ++yieldCount_ == 128) {
				//CkPrintf("---(%d): CthYield called\n", /*FIXME: uchareset.getId()*/  CkMyPe());
        yieldCount_ = 0;
        CthYield();
      }

			User_uChare &uchare = uchareset.uchares[uChareDistribution::getLocaluChareId(m.destination)];

			if (/*!isReentrant &&*/ uchare.isLocked()) {
				//CkAbort("Destination uchare is locked!!!");
				// if destination uchare is locked (i.e. is executing entry method
				// then store this message for further processing
				delayedMessages.push_back(m);

				/*CkPrintf("[chare=%d,pe=%d]: message to %d delayed, total delayed messages %d\n",
						uchareset.getId(), uchareset.getPe(), m.destination, delayedMessages.size());*/
					int delta = 100;
					if (abs(delayedMessages.size()-oldsize) > delta) {
						//CkPrintf("[chare=%d,pe=%d]: %d messages, total delayed messages %d (%d)\n",
						//		uchareset.getId(), uchareset.getPe(), delayedMessages.size()-oldsize, delayedMessages.size(), oldsize);
						oldsize = delayedMessages.size();
					}
			}
			else {
				// if destination uchare is unlocked then execute this message immidiately
				if (!isReentrant) uchare.lock();
				(proxy->*unpack_func)(m);
				if (!isReentrant) uchare.unlock();
			}
			
			// scan and execute delayed messages
			for (std::vector<Message>::iterator it = delayedMessages.begin();	it != delayedMessages.end(); ) {
				const Message m = *it;
				UnpackFunction unpack_func = static_cast<UnpackFunction>(epTable[m.registeredFunctionId].second);
				bool isReentrant = epTable[m.registeredFunctionId].first; 
				User_uChare &uchare = uchareset.uchares[uChareDistribution::getLocaluChareId(m.destination)];

				if (/*isReentrant ||*/ !uchare.isLocked()) {

					/*if (getuChareSet())
						CkPrintf("[chare=%d,pe=%d]: execute delayed call (delayedMessages.size = %d)\n", 
								uchareset.getId(), CkMyPe(), delayedMessages.size());*/
					// erase message afore its been processed
					delayedMessages.erase(it);

					int delta = 100;
					if (abs(delayedMessages.size()-oldsize) > delta) {
						//CkPrintf("[chare=%d,pe=%d]: %d messages, total delayed messages %d (%d)\n",
						//		uchareset.getId(), uchareset.getPe(), delayedMessages.size()-oldsize, delayedMessages.size(), oldsize);
						oldsize = delayedMessages.size();
					}

					if (!isReentrant) uchare.lock();

					// this call can change delayedMessages buffer therefore we can not trust 
					// it anymore and have to refresh it with begin value
					(proxy->*unpack_func)(m);
					it = delayedMessages.begin(); //TODO: ?!

					if (!isReentrant) uchare.unlock();
				} else
					it++;
			}
		}
};

#endif
