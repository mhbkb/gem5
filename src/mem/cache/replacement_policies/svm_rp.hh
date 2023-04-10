
#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_SVM_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_SVM_RP_HH__

#include <deque>
#include <unordered_map>
#include <vector>
#include "base/types.hh"
#include "mem/cache/replacement_policies/brrip_rp.hh"

namespace gem5
{

struct SvmRPParams;

GEM5_DEPRECATED_NAMESPACE(ReplacementPolicy, replacement_policy);
namespace replacement_policy
{

class Svm : public BRRIP
{
  protected:

    struct SvmReplData : BRRIPReplData
    {
        Tick lastTouchTick;
        Addr programCounter;

        SvmReplData(int num_bits) : lastTouchTick(0), programCounter(0) {}
    };

  public:
    typedef SvmRPParams Params;
    Svm(const Params &p);
    ~Svm() = default;

    void reset(const std::shared_ptr<ReplacementData>& replacement_data) const override;
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) const override;
    void reset(const std::shared_ptr<ReplacementData>& replacement_data, const PacketPtr pkt) override;
    void touch(const std::shared_ptr<ReplacementData>& replacement_data, const PacketPtr pkt) override;
    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data) override;
    std::shared_ptr<ReplacementData> instantiateEntry() override;

  private:
    // Declare the isvm table
    std::unordered_map<Addr, std::vector<int>> isvmTable;

    // PC history register
    std::deque<Addr> pchr;
};

}
}

#endif