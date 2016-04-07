#ifndef __uChareLib_h_
#define __uChareLib_h_
/**
 * uChare Library provides user interface for charm++ style programming
 * based on a message-driven programming model with support of automatic
 * messsage aggregation. 
 *
 * uChare Library extends Charm++ programming model with concept of 
 * mirochares (or uchares) which is used as a base class for user 
 * application classes. uchares are grouped into uchare sets which are
 * usual Charm++ chares. Application developer can declare uchare entry
 * methods and call them from other uchares. uChare Library transfer calls
 * correspondent uchare and initiate method execution. 
 *
 * This is an experimental project.
 */
#include "charm++.h"
#include "NDMeshStreamer.h"
#include "TopoManager.h"
#include <vector>
#include <uChareLib.decl.h>

#define UCHARELIB

//#define DEBUG
#ifdef DEBUG
	#define DEBUGGING(...) CkPrintf(__VA_ARGS__)
#else
	#define DEBUGGING(...) while(0) {}
#endif

/**
 * Global readonly variables
 */
extern std::size_t N_uChares;
extern std::size_t N_uChareSets;

#include <Distribution.h>
#include <Message.h>
#include <uChareSet.h>

#define CK_TEMPLATES_ONLY
#include "uChareLib.def.h"
#undef CK_TEMPLATES_ONLY

#endif
