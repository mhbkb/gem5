
#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_SVM_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_SVM_RP_HH__

#include "mem/cache/cache_blk.hh"
#include "mem/cache/replacement_policies/base.hh"
#include <unordered_map>
#include <list>

namespace gem5
{

struct SvmRPParams;

GEM5_DEPRECATED_NAMESPACE(ReplacementPolicy, replacement_policy);
namespace replacement_policy
{

class Svm : public Base
{
  public:
    typedef SvmRPParams Params;
    Svm(const Params *p);
    ~Svm() = default

  protected:
    CacheBlk* getNextVictim(const ReplacementCandidates& candidates) override;

  private:
    // Parameters for Svm algorithm
    const int numSets;
    const int numWays;

    // Data structures for Svm algorithm
    std::vector<std::vector<CacheBlk*>> shadowSets;
    std::unordered_map<Addr, uint64_t> accessTimes;
    std::unordered_map<Addr, Addr> lastPc;
    std::unordered_map<Addr, uint32_t> sampledReuseDistance;
};

}
}

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_SVM_RP_HH__
