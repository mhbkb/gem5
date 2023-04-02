#include "mem/cache/replacement_policies/svm_rp.hh"

#include <limits>
#include "params/SvmRP.hh"

namespace gem5
{

GEM5_DEPRECATED_NAMESPACE(ReplacementPolicy, replacement_policy);
namespace replacement_policy
{
Svm::Svm(const Params *p)
    : Base(p),
      numSets(p->num_sets),
      numWays(p->num_ways),
      shadowSets(numSets, std::vector<CacheBlk*>(numWays, nullptr))
{
}

CacheBlk*
Svm::getNextVictim(const ReplacementCandidates& candidates)
{
    assert(candidates.size() > 0);

    // Update shadow sets and reuse distance histograms
    for (const auto& blk : candidates) {
        Addr pc = lastPc[blk->tag];
        Addr set = blk->set;
        auto it = std::find(shadowSets[set].begin(), shadowSets[set].end(), blk);

        if (it != shadowSets[set].end()) {
            // Calculate reuse distance
            uint64_t current_time = curTick();
            uint64_t access_time = accessTimes[blk->tag];
            uint32_t reuse_distance = current_time - access_time;

            // Update the histogram
            sampledReuseDistance[pc] = reuse_distance;

            // Remove the block from the shadow set
            shadowSets[set].erase(it);
        }
    }

    // Find the block with the lowest predicted reuse distance
    CacheBlk* victim = nullptr;
    uint32_t min_predicted_reuse_distance = std::numeric_limits<uint32_t>::max();

    for (const auto& blk : candidates) {
        Addr pc = lastPc[blk->tag];
        uint32_t predicted_reuse_distance = sampledReuseDistance[pc];

        if (predicted_reuse_distance < min_predicted_reuse_distance) {
            victim = blk;
            min_predicted_reuse_distance = predicted_reuse_distance;
        }
    }

    // Update access time and last PC for the victim block
    accessTimes[victim->tag] = curTick();
    lastPc[victim->tag] = currentInstPC();

    // Add the victim block to the shadow set
    Addr set = victim->set;
    shadowSets[set].push_back(victim);

    return victim;
}

}
}