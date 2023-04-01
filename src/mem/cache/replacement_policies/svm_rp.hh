#include "mem/cache/replacement_policies/base.hh"
#include "params/SvmRP.hh"
#include <unordered_map>
#include <queue>

class SvmRP : public Base
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
    SvmRP(const Params *p);

    void invalidate(const std::shared_ptr<ReplacementData>& replacement_data) override;
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) override;
    void reset(const std::shared_ptr<ReplacementData>& replacement_data) override;
    ReplaceableEntry* findVictim(Addr addr) override;
    std::shared_ptr<ReplacementData> instantiateEntry() override;

  private:
    void updateReusePredictions(Addr pc, Tick access_time);
    Addr findLowestReusePrediction();
};
