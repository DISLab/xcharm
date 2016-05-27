#ifndef __uChareMessage_h__
#define __uChareMessage_h__

class UserMessage {
	public:
		void *buf;
		int size;
		UserMessage (const UserMessage &m) {
			size = m.size;
			buf = malloc(size);
			memcpy(buf, m.buf, size);
		}
		UserMessage(int size) : size(size) {
			buf = malloc(size);
		}
		~UserMessage() {
			free(buf);
		}
};

/**
 * Argument list implementation
 */
class ArgumentList {
	std::vector<int> intArgs;
	std::vector<long long> longlongArgs;
	std::vector<double> doubleArgs;
	std::vector<UserMessage *> messageArgs;
	//...	
	enum ArgType {INT, LONGLONG, DOUBLE, MESSAGE};
	struct ArgIndex {
		ArgType type;
		int idx;
		ArgIndex(ArgType type, int idx) : type(type), idx(idx) {}
	};
	std::vector<ArgIndex> index;

	public:
		ArgumentList() {}

		~ArgumentList() {
			typedef std::vector<UserMessage *>::iterator Iterator;
			for (Iterator i = messageArgs.begin(); i != messageArgs.end(); i++)
				delete *i;
		}

		ArgumentList& operator=(const ArgumentList &rhs) {
			index = rhs.index;
			intArgs = rhs.intArgs;
			longlongArgs = rhs.longlongArgs;
			doubleArgs = rhs.doubleArgs;
			//typedef std::vector<UserMessage *>::iterator Iterator;
			//for (Iterator i = rhs.messageArgs.begin(); i != rhs.messageArgs.end(); i++)
			//	messageArgs.push_back(*i);
			for (int i = 0; i != rhs.messageArgs.size(); i++)
				messageArgs.push_back(new UserMessage(*rhs.messageArgs[i]));
		}

		void push(int arg) {
			index.push_back(ArgIndex(INT,intArgs.size()));
			intArgs.push_back(arg);
		}
		void push(unsigned int arg) {
			index.push_back(ArgIndex(INT,intArgs.size()));
			intArgs.push_back(arg);
		}
		void push(long arg) {
			index.push_back(ArgIndex(INT,intArgs.size()));
			intArgs.push_back(arg);
		}
		void push(unsigned long arg) {
			index.push_back(ArgIndex(INT,intArgs.size()));
			intArgs.push_back(arg);
		}
		void push(long long arg) {
			index.push_back(ArgIndex(LONGLONG,longlongArgs.size()));
			longlongArgs.push_back(arg);
		}
		void push(unsigned long long arg) {
			index.push_back(ArgIndex(LONGLONG,longlongArgs.size()));
			longlongArgs.push_back(arg);
		}
		void push(double arg) {
			index.push_back(ArgIndex(DOUBLE, doubleArgs.size()));
			doubleArgs.push_back(arg);
		}
		void push(UserMessage * arg) {
			index.push_back(ArgIndex(MESSAGE,intArgs.size()));
			messageArgs.push_back(arg);
		}
		void erase (int idx) { index.erase(index.begin() + idx); }
		const void *operator [](int idx) const {
			switch (index[idx].type) {
				case INT:
					return &intArgs[index[idx].idx];
				case LONGLONG:
					return &longlongArgs[index[idx].idx];
				case DOUBLE:
					return &doubleArgs[index[idx].idx];
				case MESSAGE:
					return messageArgs[index[idx].idx];
				default:
					return NULL;
			}
		}

		int size() {
			int totalsize = sizeof(int) + index.size() * sizeof(char);
			for (int i = 0; i < index.size(); i++) {
				switch (index[i].type) {
					case INT:
						totalsize += sizeof(int);						
						break;
					case LONGLONG:
						totalsize += sizeof(long long);						
						break;
					case DOUBLE:
						totalsize += sizeof(double);
						break;
					case MESSAGE:
						totalsize += sizeof(int) + messageArgs[index[i].idx]->size;
						break;
					default:
						CkAbort("Unknown argument type");
				}
			}
			return totalsize;
		}

		void pack (void *buf) {
			char *p = (char *)buf;
			*(int *)p = index.size();
			p += sizeof(int);

			for (int i = 0; i < index.size(); i++) {
				switch (index[i].type) {
					case INT:
						*p = index[i].type;
						p++;
						*(int *)p = intArgs[index[i].idx];
						p += sizeof(int);
						break;
					case LONGLONG:
						*p = index[i].type;
						p++;
						*(long long *)p = longlongArgs[index[i].idx];
						p += sizeof(long long);
						break;
					case DOUBLE:
						*p = index[i].type;
						p++;
						*(double *)p = doubleArgs[index[i].idx];
						p += sizeof(double);
						break;
					case MESSAGE:
						*p = index[i].type;
						p++;
						*(int *)p = messageArgs[index[i].idx]->size;
						p += sizeof(int);
						memcpy(p, messageArgs[index[i].idx]->buf, messageArgs[index[i].idx]->size);
						p += sizeof(messageArgs[index[i].idx]->size);
						break;
					default:
						CkAbort("Unknown argument type");
				}
			}
		}

		void unpack (void *buf) {
			char *p = (char *)buf;
			int size = *(int *)p;
			p += sizeof(int);

			for (int i = 0; i < size; i++) {
				switch (*(char *)p) {
					case INT:
						p++;
						index.push_back(ArgIndex(INT,intArgs.size()));
						intArgs.push_back(*(int *)p);
						p += sizeof(int);
						break;
					case LONGLONG:
						p++;
						index.push_back(ArgIndex(LONGLONG,intArgs.size())); //FIXME: intArgs???
						longlongArgs.push_back(*(long long *)p);
						p += sizeof(long long);
						break;
					case DOUBLE:
						p++;
						index.push_back(ArgIndex(DOUBLE,doubleArgs.size()));
						doubleArgs.push_back(*(double *)p);
						p += sizeof(double);
						break;
					case MESSAGE: {
								p++;
								int s = *(int *)p;
								p += sizeof(int);
								UserMessage *m = new UserMessage(s);
								memcpy(m->buf, p, m->size);
								p += sizeof(m->size);
								index.push_back(ArgIndex(MESSAGE,intArgs.size()));
								messageArgs.push_back(m);
					  }
						break;
					default:
						CkAbort("Unknown argument type");
				}
			}
		}
};
  
/**
 * Messages are created when uchares call entry-methods other uchares.
 */
class Message {
public:
	// magic word (only for debugging)
	int magic;
	// destionation uchare
	std::size_t destination;
	// function identifier
	int registeredFunctionId;
	// argument list
  ArgumentList argumentList;	

	Message() : magic(0xbad0), registeredFunctionId(-1), 
		destination(-1) {}

	Message(int registeredFunctionId) :  magic(0xbad1), registeredFunctionId(registeredFunctionId), 
		destination(-1) {}

	Message(const Message& m) {
		this->magic = m.magic;
		this->destination = m.destination;
		this->registeredFunctionId = m.registeredFunctionId;
		this->argumentList = m.argumentList;
	}
	
	void operator | (int a) { 
		argumentList.push(a); 
	}

	void operator | (unsigned int a) { 
		argumentList.push(a); 
	}

	void operator | (long a) { 
		argumentList.push(a); 
	}

	void operator | (unsigned long a) { 
		argumentList.push(a); 
	}

	void operator | (long long a) { 
		argumentList.push(a); 
	}

	void operator | (unsigned long long a) { 
		argumentList.push(a); 
	}

	void operator | (double a) { 
		argumentList.push(a); 
	}

	void operator | (UserMessage *m) { 
		argumentList.push(m); 
	}

	void operator >> (int & a) { 
		a = *(int *)argumentList[0];
		argumentList.erase(0);
	}	

	void operator >> (long long & a) { 
		a = *(long long *)argumentList[0];
		argumentList.erase(0);
	}	

	void operator >> (unsigned long long & a) { 
		a = *(long long *)argumentList[0];
		argumentList.erase(0);
	}	

	void operator >> (double & a) { 
		a = *(double *)argumentList[0];
		argumentList.erase(0);
	}	

	void operator >> (UserMessage * & m) { 
		//argumentList.push(m); 
	}
		
	int size() {
		return 
			sizeof(int) + // magic
			sizeof(std::size_t) + // dest
			sizeof(int) + // function id
			argumentList.size();
	}

	static int pack(Message *m, void *buf) {
		char *p = (char *)buf;
		*(int *)p = m->magic;
		p += sizeof(int); 
		*(std::size_t *)p = m->destination; 
		p += sizeof(std::size_t);
		*(int *)p = m->registeredFunctionId; 
		p += sizeof(int); 
		m->argumentList.pack((void *)p);
		return m->size();
	}

	static void unpack(Message *m, void *buf) {
		char *p = (char *)buf;
		m->magic = *(int *)p;
		p += sizeof(int); 
		m->destination = *(std::size_t *)p; 
		p += sizeof(std::size_t);
		m->registeredFunctionId = *(int *)p; 
		p += sizeof(int); 
		m->argumentList.unpack((void *)p);
	}
	static Message *unpack(void *buf) {
		CkAbort("Method is not implemented");
		return 0;
	}

	Message& operator=(const Message &rhs) {
		if (this != &rhs) {
			this->magic = rhs.magic;
			this->destination = rhs.destination;
			this->registeredFunctionId = rhs.registeredFunctionId;
			this->argumentList = rhs.argumentList;
		}
		return *this;
	}

};

struct AggregatedMessage : public CMessage_AggregatedMessage {
	public:
		std::vector<Message> messages;
		AggregatedMessage() {}

		int size() { return messages.size(); }

		void addNewMsg(Message & m) { messages.push_back(m); }

    static void *pack(AggregatedMessage *m) {
			// compute total size of the message
			int totalsize = 2*sizeof(int);
			for (int i = 0; i < m->messages.size(); i++)
				totalsize += m->messages[i].size();

			// allocate buffer in Charm++ runtime
			char *buf = (char *)CkAllocBuffer(m, totalsize);
			char *p = buf; 

			// store total size
			*(int *)p = totalsize;
			p += sizeof(int);

			// store message count
			*(int *)p = m->messages.size();
			p += sizeof(int);

		  // store messages in buffer
			for (int i = 0; i < m->messages.size(); i++) {
				int size = Message::pack(&m->messages[i], p);
				p += size;
			}

			delete m;
			return (void *)buf;
		}

    static AggregatedMessage* unpack(void* buf) {
			//CkPrintf("AggregatedMessage::unpack called, buf = %llx\n", buf);
			char *p = (char *)buf;

			// read total size 
			int totalsize = *(int *)p;
			p += sizeof(int);

			// allocate aggregated message 
			AggregatedMessage *m = (AggregatedMessage *)CkAllocBuffer(buf, totalsize);
			m = new ((void *)m) AggregatedMessage();

			// read and resize message vector FIXME: ????
			m->messages.resize(*(int *)p);
			p += sizeof(int);

			//unpack messages
			for (int i = 0; i < m->messages.size(); i++) {
				m->messages[i] = *(Message::unpack(p)); 
				p += m->messages[i].size();
			}
			CkFreeMsg(buf);
			return m;
		}

		void operator=(const AggregatedMessage &msg) {
			CkError("operator= is not implemented!");
		}

		AggregatedMessage(const AggregatedMessage&) {
			CkError("constructor is not implemented!");
		}
};

class FastMessage {
	PUP::sizer s;
}; 

#endif
