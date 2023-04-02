#include "mem/cache/replacement_policies/svm_rp.hh"

#include "params/SvmRP.hh"
#include "sim/cur_tick.hh"

namespace gem5
{

GEM5_DEPRECATED_NAMESPACE(ReplacementPolicy, replacement_policy);
namespace replacement_policy
{

SvmRP::SvmRP(const Params *p)
    : Base(p), pcTableSize(p->pc_table_size)
{
    pcTable.reserve(pcTableSize);
}

void
SvmRP::reset(const std::shared_ptr<ReplacementData>& replacement_data, const PacketPtr pkt)
{
    std::static_pointer_cast<SvmReplData>(replacement_data)->lastTouchTick = curTick();
    std::static_pointer_cast<SvmReplData>(replacement_data)->programCounter = 0;
}

void
SvmRP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    panic("Cant train svm predictor without access information.");
}

void
SvmRP::touch(const std::shared_ptr<ReplacementData>& replacement_data, const PacketPtr pkt))
{
    std::static_pointer_cast<SvmReplData>(replacement_data)->lastTouchTick = curTick();
    std::static_pointer_cast<SvmReplData>(replacement_data)->programCounter = pkt->req->getPC();

    // Update the pcTable with the current reuse distance
    Tick reuse_distance = curTick() - std::static_pointer_cast<SvmReplData>(replacement_data)->lastTouchTick;
    pcTable[std::static_pointer_cast<SvmReplData>(replacement_data)->programCounter] = reuse_distance;
}

void
SvmRP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    panic("Cant train svm predictor without access information.");
}

void
SvmRP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    replacement_data->lastTouchTick = Tick(0);
    replacement_data->programCounter = 0;
}

ReplaceableEntry*
SvmRP::getVictim(const ReplacementCandidates& candidates) const
{
    ReplaceableEntry *evict_candidate = nullptr;
    Tick minReuseDistance = std::numeric_limits<Tick>::max();

    for (const auto& candidate : candidates) {
        Addr pc = candidate->refData->programCounter;
        auto pc_iter = pcTable.find(pc);

        if (pc_iter != pcTable.end()) {
            Tick reuseDistance = pc_iter->second;
            if (reuseDistance < minReuseDistance) {
                minReuseDistance = reuseDistance;
                evict_candidate = candidate;
            }
        } else {
            // If PC is not in the table, evict this block
            evict_candidate = candidate;
            break;
        }
    }

    return evict_candidate;
}

SvmRP*
SvmRPParams::create()
{
    return new SvmRP(this);
}

} // namespace replacement_policy
} // namespace gem5