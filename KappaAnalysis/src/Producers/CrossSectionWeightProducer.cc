#include "Artus/KappaAnalysis/interface/Producers/CrossSectionWeightProducer.h"

std::string CrossSectionWeightProducer::GetProducerId() const {
    return "CrossSectionWeightProducer";
}

void CrossSectionWeightProducer::Produce(KappaEvent const &event, KappaProduct &product,
                                         KappaSettings const &settings) const {
    assert(event.m_genLumiInfo);
    LOG(DEBUG) << "\n[" << this->GetProducerId() << "]";

    if (!signbit(settings.GetCrossSection())) {
        LOG(DEBUG) << "Adding crossSectionPerEventWeight: " << settings.GetCrossSection();
        product.m_weights["crossSectionPerEventWeight"] = settings.GetCrossSection();
        if (!signbit(settings.GetCrossSectionUp()) && signbit(settings.GetCrossSectionDown())) {
            LOG(DEBUG) << "Adding crossSectionPerEventWeight Variations";
            product.m_optionalWeights["crossSectionPerEventWeightUp"] = settings.GetCrossSectionUp();
            product.m_optionalWeights["crossSectionPerEventWeightDown"] = settings.GetCrossSectionDown();
        }
    } else {
        LOG(FATAL) << "No valid CrossSection specified: " << settings.GetCrossSection();
    }
}
