#include <uChareLib.h>
#include <test_hello.decl.h>
...
class Hello : public CBase_Hello, public uChare<Hello, CProxy_Hello, CBase_Hello> {
	public:
		Hello() {}
		Hello(std::size_t index, 
			uChareSet<Hello, CProxy_Hello, CBase_Hello> *uchareset) : 
			uChare<Hello, CProxy_Hello, CBase_Hello>(index, uchareset)  {
			...
		}

		void hello(int callee) { 
			CkPrintf("Hello from %d\n", getuChareSet()->getPe());
			getProxy()[callee]->buybuy(getId());
		}

		void buybuy(int callee) { 
			CkPrintf("Buy-buy from %d\n", getuChareSet()->getPe());
		}

		void run() { 
			if ((getId() + 1) < N_uChares)
				getProxy()[getId()+1]->hello(getId());
			else
				getProxy()[0]->hello(getId());
		} 
};
...
#include <test_hello.def.h>
