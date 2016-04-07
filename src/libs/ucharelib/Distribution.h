#ifndef __uChareDistribution_h__
#define __uChareDistribution_h__ 
/**
 * Provides uChareDistribution class with static methods
 * for obtaining local and global uchare indeces via specified
 * distribution function.
 */
class uChareDistribution {
	private:
	public:
		static std::size_t getLocaluChareId(std::size_t vid) {
			return vid % (N_uChares/N_uChareSets); 
		}
		static std::size_t getuChareSetId(std::size_t vid) {
			return vid / (N_uChares/N_uChareSets); 
		}
		static bool isLocal(std::size_t vid, std::size_t vsid) {
			return vsid == getuChareSetId(vid); 
		}
};


#endif
