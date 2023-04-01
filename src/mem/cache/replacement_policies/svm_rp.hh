
#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_SVM_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_SVM_RP_HH__

#include "mem/cache/replacement_policies/base.hh"
#include <unordered_map>
#include <queue>

namespace gem5
{

struct SvmRPParams;

GEM5_DEPRECATED_NAMESPACE(ReplacementPolicy, replacement_policy);
namespace replacement_policy
{

class Svm : public Base
{
  protected:
    struct AccessInfo {
        Tick accessTime;
        Addr pc;
    };

    // OPTgen queue
    std::queue<AccessInfo> optgenQueue;

    // Store the PC-based reuse predictions
    std::unordered_map<Addr, uint64_t> reusePredictions;

  public:
    typedef SvmRPParams Params;
    Svm(const Params *p);

    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data) override;
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) override;
    void reset(const std::shared_ptr<ReplacementData>& replacement_data) override;
    ReplaceableEntry* findVictim(Addr addr) override;
    std::shared_ptr<ReplacementData> instantiateEntry() override;

  private:
    void updateReusePredictions(Addr pc, Tick access_time);
    Addr findLowestReusePrediction();
};

}
}

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_SVM_RP_HH__
