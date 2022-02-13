#include "Artus/KappaAnalysis/interface/Producers/CrossSectionWeightProducer.h"

std::string CrossSectionWeightProducer::GetProducerId() const {
    return "CrossSectionWeightProducer";
}

void CrossSectionWeightProducer::Produce(KappaEvent const &event, KappaProduct &product,
                                         KappaSettings const &settings) const {
    assert(event.m_genLumiInfo);
    LOG(DEBUG) << "\n[" << this->GetProducerId() << "]";

    if (static_cast<double>(settings.GetCrossSection()) > 0.0) {
        LOG(DEBUG) << "Adding crossSectionPerEventWeight: " << settings.GetCrossSection();
        product.m_weights["crossSectionPerEventWeight"] = settings.GetCrossSection();
    } else {
        LOG(ERROR) << "No CrossSection information in the input found.";
    }
}
