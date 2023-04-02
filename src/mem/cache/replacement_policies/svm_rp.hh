
#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_SVM_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_SVM_RP_HH__

#include <unordered_map>
#include "mem/cache/replacement_policies/base.hh"

namespace gem5
{

struct SvmRPParams;

GEM5_DEPRECATED_NAMESPACE(ReplacementPolicy, replacement_policy);
namespace replacement_policy
{

class SvmRP : public Base
{
  class SvmReplData : public ReplacementData
    {
    public:
        SvmReplData() : lastTouchTick(0), programCounter(0) {}

        Tick lastTouchTick;
        Addr programCounter;
    };


  public:
    typedef SvmRPParams Params;
    SvmRP(const Params *p);
    ~SvmRP() = default;

  protected:
    void reset(const std::shared_ptr<ReplacementData>& replacement_data) const override;
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) const override;
    void reset(const std::shared_ptr<ReplacementData>& replacement_data, const PacketPtr pkt) override;
    void touch(const std::shared_ptr<ReplacementData>& replacement_data, const PacketPtr pkt) override;
    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data) const override;
    ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) const override;

  private:
    // Declare the PC table data structure here
    std::unordered_map<Addr, Tick> pcTable;
    size_t pcTableSize;
};

}
}

#endif