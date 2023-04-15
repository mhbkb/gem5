#include "mem/cache/replacement_policies/svm_rp.hh"

#include "params/SvmRP.hh"
#include "sim/cur_tick.hh"

namespace gem5
{

namespace replacement_policy
{

Svm::SvmReplData::SvmReplData(int num_bits)
  : BRRIPReplData(num_bits), outcome(false)
{
}

Svm::Svm(const Params &p)
    : BRRIP(p)
{
}

void
Svm::reset(const std::shared_ptr<ReplacementData>& replacement_data, const PacketPtr pkt)
{
    std::shared_ptr<SvmReplData> casted_replacement_data = std::static_pointer_cast<SvmReplData>(replacement_data);

    Addr currPC = pkt->req->getPC();
    if (isvmTable.find(currPC) == isvmTable.end()) {
        std::vector<int> vec(16, 0);
        isvmTable[currPC] = vec;
    }

    std::vector<int> weights = isvmTable.find(currPC)->second;
    int weightSum = 0;
    for(Addr hisPC: pchr) {
        int weightIdx = hisPC % 16;
        weightSum += weights[weightIdx];

        // Set threshold -30, and decreasing by 1 during cache miss
        if (weights[weightIdx] > -30) {
            weights[weightIdx] -= 1;
        }
    }

    // Follow the paper to setup the rrpv based on the weight threshold.
    int targetRRPV = 0;
    if(weightSum > 60) {
        targetRRPV = 0;
    } else if (weightSum < 0) {
        targetRRPV = 7;
    } else {
        targetRRPV = 2;
    }

    // Increment the counter until it reaches the desired value (7)
    while (casted_replacement_data->rrpv < targetRRPV) {
        casted_replacement_data->rrpv++;
    }

    // Decrement the counter until it reaches the desired value (7)
    while (casted_replacement_data->rrpv > targetRRPV) {
        casted_replacement_data->rrpv--;
    }

    // Processing a cache miss. Maintain the PCHR
    if(pchr.size() >= 5) {
        pchr.pop_front();
    }
    pchr.push_back(currPC);
}

void
Svm::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    panic("Cant train svm predictor without access information.");
}

void
Svm::touch(const std::shared_ptr<ReplacementData>& replacement_data, const PacketPtr pkt)
{
    std::shared_ptr<SvmReplData> casted_replacement_data = std::static_pointer_cast<SvmReplData>(replacement_data);

    Addr currPC = pkt->req->getPC();
    if (isvmTable.find(currPC) == isvmTable.end()) {
        std::vector<int> vec(16, 0);
        isvmTable[currPC] = vec;
    }

    std::vector<int> weights = isvmTable.find(currPC)->second;
    int weightSum = 0;
    for(Addr hisPC: pchr) {
        int weightIdx = hisPC % 16;
        weightSum += weights[weightIdx];

        // Set threshold 30, and increasing by 1 during cache hit
        if (weights[weightIdx] < 30) {
            weights[weightIdx] += 1;
        }
    }

    // Follow the paper to setup the rrpv based on the weight threshold.
    int targetRRPV = 0;
    if(weightSum > 60) {
        targetRRPV = 0;
    } else if (weightSum < 0) {
        targetRRPV = 7;
    } else {
        targetRRPV = 2;
    }

    // Increment the counter until it reaches the desired value (7)
    while (casted_replacement_data->rrpv < targetRRPV) {
        casted_replacement_data->rrpv++;
    }

    // Decrement the counter until it reaches the desired value (7)
    while (casted_replacement_data->rrpv > targetRRPV) {
        casted_replacement_data->rrpv--;
    }

    // Processing a cache miss. Maintain the PCHR
    if(pchr.size() >= 5) {
        pchr.pop_front();
    }
    pchr.push_back(currPC);
}

void
Svm::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    panic("Cant train svm predictor without access information.");
}

void
Svm::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
{
    std::shared_ptr<SvmReplData> casted_replacement_data = std::static_pointer_cast<SvmReplData>(replacement_data);
    BRRIP::invalidate(replacement_data);
}

std::shared_ptr<ReplacementData>
Svm::instantiateEntry()
{
    return std::shared_ptr<ReplacementData>(new SvmReplData(numRRPVBits));
}

} // namespace replacement_policy
} // namespace gem5