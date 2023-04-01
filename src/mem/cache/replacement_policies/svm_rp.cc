#include "mem/cache/replacement_policies/svm_rp.hh"

#include <limits>
#include "params/SvmRP.hh"

Svm::Svm(const Params *p)
    : Base(p)
{
}

void
Svm::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
{
    Addr pc = replacement_data->programCounter;
    BaseReplacementPolicy::invalidate(replacement_data);
    reusePredictions.erase(pc);
}

void
Svm::touch(const std::shared_ptr<ReplacementData>& replacement_data)
{
    Tick access_time = curTick();
    AccessInfo access_info{access_time, replacement_data->programCounter};
    optgenQueue.push(access_info);
}

void
Svm::reset(const std::shared_ptr<ReplacementData>& replacement_data)
{
    Addr pc = replacement_data->programCounter;
    BaseReplacementPolicy::reset(replacement_data);
    reusePredictions.erase(pc);
}

ReplaceableEntry*
Svm::findVictim(Addr addr)
{
    while (!optgenQueue.empty()) {
        AccessInfo access_info = optgenQueue.front();
        optgenQueue.pop();
        updateReusePredictions(access_info.pc, access_info.accessTime);
    }

    Addr lowest_prediction_pc = findLowestReusePrediction();
    return policy->findEntry(lowest_prediction_pc);
}

std::shared_ptr<ReplacementData>
Svm::instantiateEntry()
{
    return std::make_shared<ReplacementData>();
}

void
Svm::updateReusePredictions(Addr pc, Tick access_time)
{
    auto it = reusePredictions.find(pc);

    if (it == reusePredictions.end()) {
        reusePredictions[pc] = access_time;
    } else {
        it->second = access_time;
    }
}

Addr
Svm::findLowestReusePrediction()
{
    Addr lowest_prediction_pc = 0;
    Tick lowest_prediction = std::numeric_limits<Tick>::max();

    for (const auto &entry : reusePredictions) {
        if (entry.second < lowest_prediction) {
            lowest_prediction = entry.second;
            lowest_prediction_pc = entry.first;
        }
    }

    return lowest_prediction_pc;
}

Svm*
SvmRPParams::create()
{
    return new Svm(this);
}
