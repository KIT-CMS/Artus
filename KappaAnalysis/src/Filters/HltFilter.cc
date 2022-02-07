#include "Artus/KappaAnalysis/interface/Filters/HltFilter.h"


/** Filter events for that a previously selected HLT trigger has fired.
 *
 *  The selected HLT trigger has to be defined in the product by the HltProducer.
 *  Therefore this filter cannot meaningfully run as a global pre-filter
 *  which gets an empty product.
 */
std::string HltFilter::GetFilterId() const {
    return "HltFilter";
}

bool HltFilter::DoesEventPass(KappaEvent const &event, KappaProduct const &product,
                              KappaSettings const &settings) const {
    LOG(DEBUG) << "\n[HltFilter]";

    if (settings.GetNoHltFiltering()) {
        LOG(DEBUG) << "No Hlt filtering applied! (cfg: 'NoHltFiltering')";
        return true;
    }
    if (!product.m_selectedHltNames.empty()) {
        LOG(DEBUG) << "Hlt Filter passed!\n";
    } else {
        LOG(DEBUG) << "Hlt Filter not passed!\n";
    }
    return (!product.m_selectedHltNames.empty());
}
