#ifndef _CHARE_H
#define _CHARE_H

#include "xi-AstNode.h"
#include "xi-Template.h"
#include "xi-Member.h"

namespace xi {

class CEntry;

/* Base charem is an interim parent for Chare and UChare */

class BaseChare : public TEntity {
	protected:
	public:
		BaseChare() {}
		virtual XStr proxyName(int withTemplates=1) = 0; 
		virtual XStr indexName(int withTemplates=1) = 0; 
		virtual XStr indexList() = 0;
		virtual XStr baseName(int withTemplates=1) const = 0;
		virtual int isTemplated(void) = 0 ;
		virtual	bool isTemplateDeclaration() = 0 ;
		virtual	bool isTemplateInstantiation() = 0 ;
		virtual	int isMigratable(void) = 0 ;
		virtual	int isPython(void) = 0 ;
		virtual	int isMainChare(void) = 0 ;
		virtual	int isChare(void) = 0 ;
		virtual	int isArray(void) = 0 ;
		virtual	int isGroup(void) = 0 ;
		virtual	int isNodeGroup(void) = 0 ;
		virtual	int isForElement(void) const = 0 ;
		virtual	int isForSection(void) const  = 0;
    virtual void  setSdag(int f) = 0 ;
    virtual forWhom getForWhom(void) const  = 0;
	public:
   enum { //Set these attribute bits in "attrib"
   	CMIGRATABLE=1<<2,
    CPYTHON=1<<3,
   	CCHARE=1<<9,                 // plain non-migratable chare
   	CMAINCHARE=1<<10,
   	CARRAY=1<<11,
   	CGROUP=1<<12,
   	CNODEGROUP=1<<13
   };
   typedef unsigned int attrib_t;
   XStr sdagPUPReg;
   XStr sdagDefs, closuresDecl, closuresDef;
   //NamedType *type;
};

/* Chare or group is a templated entity */
//class Chare : public TEntity {
class Chare : public BaseChare {
 private:
   AstChildren<Member> *list;

 public:
   /*enum { //Set these attribute bits in "attrib"
   	CMIGRATABLE=1<<2,
    CPYTHON=1<<3,
   	CCHARE=1<<9,                 // plain non-migratable chare
   	CMAINCHARE=1<<10,
   	CARRAY=1<<11,
   	CGROUP=1<<12,
   	CNODEGROUP=1<<13
   };
   typedef unsigned int attrib_t;
   XStr sdagPUPReg;
   XStr sdagDefs, closuresDecl, closuresDef;*/
   NamedType *type;

 protected:
   attrib_t attrib;
   int hasElement;//0-- no element type; 1-- has element type
   forWhom forElement;
   int hasSection; //1-- applies only to array section

   TypeList *bases; //Base classes used by proxy
   TypeList *bases_CBase; //Base classes used by CBase (or NULL)

   int entryCount;
   int hasSdagEntry;

   void genTypedefs(XStr& str);
   void genRegisterMethodDef(XStr& str);
   void sharedDisambiguation(XStr &str,const XStr &superclass);
   void genMemberDecls(XStr &str);

 public:
   Chare(int ln, attrib_t Nattr, NamedType *t, TypeList *b=0, AstChildren<Member> *l=0);
   void genProxyNames(XStr& str, const char *prefix, const char *middle, 
                       const char *suffix, const char *sep);
   void genIndexNames(XStr& str, const char *prefix, const char *middle, 
                       const char *suffix, const char *sep);
   void printChareNames();
   XStr proxyName(int withTemplates=1); 
   XStr indexName(int withTemplates=1); 
   XStr indexList();
   XStr baseName(int withTemplates=1) const
   {
   	XStr str;
   	str<<type->getBaseName();
   	if (withTemplates) str<<tvars();
   	return str;
   }
   XStr cbaseTType();
   int  isTemplated(void) { return (templat!=0); }
   bool isTemplateDeclaration() { return templat; }
   bool isTemplateInstantiation() { return type->isTemplated(); }
   int  isMigratable(void) { return attrib&CMIGRATABLE; }
   int  isPython(void) { return attrib&CPYTHON; }
   int  isMainChare(void) {return attrib&CMAINCHARE;}
   int  isChare(void) {return attrib&CCHARE;}     // plain non-migratable chare
   int  isArray(void) {return attrib&CARRAY;}
   int  isGroup(void) {return attrib&CGROUP;}
   int  isNodeGroup(void) {return attrib&CNODEGROUP;}
   int  isForElement(void) const {return forElement==forIndividual;}
   int  isForSection(void) const {return forElement==forSection;}
   int  hasSdag() const { return hasSdagEntry; }
   void  setSdag(int f) { hasSdagEntry = f; }
   forWhom getForWhom(void) const {return forElement;}
   void print(XStr& str);
   void check();
   void genDefs(XStr& str);
   void genReg(XStr& str);
   void genDecls(XStr &str);
   void genGlobalCode(XStr scope, XStr &decls, XStr &defs);
   void preprocess();

   // DMK - Accel Support
   int genAccels_spe_c_funcBodies(XStr& str) {
     int rtn = 0;
     if (list) { rtn += list->genAccels_spe_c_funcBodies(str); }
     return rtn;
   }
   void genAccels_spe_c_regFuncs(XStr& str) {
     if (list) { list->genAccels_spe_c_regFuncs(str); }
   }
   void genAccels_spe_c_callInits(XStr& str) {
     if (list) { list->genAccels_spe_c_callInits(str); }
   }
   void genAccels_spe_h_includes(XStr& str) {
     if (list) { list->genAccels_spe_h_includes(str); }
   }
   void genAccels_spe_h_fiCountDefs(XStr& str) {
     if (list) { list->genAccels_spe_h_fiCountDefs(str); }
   }
   void genAccels_ppe_c_regFuncs(XStr& str) {
     if (list) { list->genAccels_ppe_c_regFuncs(str); }
   }

   int nextEntry(void) {return entryCount++;}
   virtual void genSubDecls(XStr& str);
   virtual void outputClosuresDecl(XStr& str);
   virtual void outputClosuresDef(XStr& str);
   virtual void genClosureEntryDecls(XStr& str);
   virtual void genClosureEntryDefs(XStr& str);
   void genPythonDecls(XStr& str);
   void genPythonDefs(XStr& str);
   virtual char *chareTypeName(void) {return (char *)"chare";}
   virtual char *proxyPrefix(void);
   virtual void genSubRegisterMethodDef(XStr& str) { (void)str; }
   void lookforCEntry(CEntry *centry);

private:
   XStr virtualPupDef(const XStr &name);
};

class MainChare : public Chare {
  public:
    MainChare(int ln, attrib_t Nattr,
              NamedType *t, TypeList *b=0, AstChildren<Member> *l=0):
	    Chare(ln, Nattr|CMAINCHARE, t,b,l) {}
    virtual char *chareTypeName(void) {return (char *) "mainchare";}
};

class Array : public Chare {
  protected:
    XStr indexSuffix;
    XStr indexType;//"CkArrayIndex"+indexSuffix;
  public:
    Array(int ln, attrib_t Nattr, NamedType *index,
          NamedType *t, TypeList *b=0, AstChildren<Member> *l=0);
    virtual int is1D(void) {return indexSuffix==(const char*)"1D";}
    virtual const char* dim(void) {return indexSuffix.get_string_const();}
    virtual void genSubDecls(XStr& str);
    virtual char *chareTypeName(void) {return (char *) "array";}
    bool hasVoidConstructor;
};

class Group : public Chare {
  public:
    Group(int ln, attrib_t Nattr,
          NamedType *t, TypeList *b=0, AstChildren<Member> *l=0);
    virtual void genSubDecls(XStr& str);
    virtual char *chareTypeName(void) {return (char *) "group";}
    virtual void genSubRegisterMethodDef(XStr& str);
};

class NodeGroup : public Group {
  public:
    NodeGroup(int ln, attrib_t Nattr,
              NamedType *t, TypeList *b=0, AstChildren<Member> *l=0):
	    Group(ln,Nattr|CNODEGROUP,t,b,l) {}
    virtual char *chareTypeName(void) {return (char *) "nodegroup";}
};

class UChare : public BaseChare {
	private:
		AstChildren<Member> *list;
	public:
   typedef unsigned int attrib_t;
   NamedType *type;

	protected:
	 attrib_t attrib;
   TypeList *bases; //Base classes used by proxy

	public:
   UChare(int ln, attrib_t Nattr, NamedType *t, TypeList *b=0, AstChildren<Member> *l=0) 
		 : attrib(Nattr), type(t), list(l), bases(b) {
			 setTemplate(0);
		 }
   void print(XStr& str);
   virtual char *chareTypeName(void) {return (char *)"uchare";}
	 void genDecls(XStr& str);
	 void genDefs(XStr& str);
	 void check();

	public:
		virtual XStr proxyName(int withTemplates=1) {} 
		virtual XStr indexName(int withTemplates=1) {} 
		virtual XStr indexList() {}
		virtual XStr baseName(int withTemplates=1) const {
			XStr str;
			str<<type->getBaseName();
			if (withTemplates) str<<tvars();
			return str;
		}
		virtual int isTemplated(void) {}
		virtual	bool isTemplateDeclaration() {}
		virtual	bool isTemplateInstantiation() {}
		virtual	int isMigratable(void) {}
		virtual	int isPython(void) {}
		virtual	int isMainChare(void) {}
		virtual	int isChare(void) {}
		virtual	int isArray(void) {}
		virtual	int isGroup(void) {}
		virtual	int isNodeGroup(void) {}
		virtual	int isForElement(void) const {}
		virtual	int isForSection(void) const  {}
    virtual void  setSdag(int f) {}
    virtual forWhom getForWhom(void) const  {}
};

class UChareArray : public UChare {
	public:
    UChareArray(int ln, attrib_t Nattr, NamedType *index,
          NamedType *t, TypeList *b=0, AstChildren<Member> *l=0) : 
			UChare(ln,Nattr,t,b,l) {}
   //void print(XStr& str) { str << "UChareArray::print called\n"; }
   virtual char *chareTypeName(void) {return (char *)"uchare array";}
}; 

}   // namespace xi

#endif  // ifndef _CHARE_H
