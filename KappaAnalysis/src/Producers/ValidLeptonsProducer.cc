
#include "Artus/KappaAnalysis/interface/Producers/ValidLeptonsProducer.h"


std::string ValidLeptonsProducer::GetProducerId() const {
    return "ValidLeptonsProducer";
}

void ValidLeptonsProducer::Produce(KappaEvent const &event, KappaProduct &product,
                                   KappaSettings const &settings) const {
    LOG(DEBUG) << "\n[" << this->GetProducerId() << "]";
    // start with empty vectors
    product.m_validLeptons.clear();
    product.m_invalidLeptons.clear();

    // copy vectors
    LOG(DEBUG) << "Inserting " << product.m_validElectrons.size() << " valid electrons.";
    product.m_validLeptons.insert(product.m_validLeptons.end(),
                                  product.m_validElectrons.begin(), product.m_validElectrons.end());

    LOG(DEBUG) << "Inserting " << product.m_validMuons.size() << " valid muons.";
    product.m_validLeptons.insert(product.m_validLeptons.end(),
                                  product.m_validMuons.begin(), product.m_validMuons.end());

    LOG(DEBUG) << "Inserting " << product.m_validTaus.size() << " valid taus.";
    product.m_validLeptons.insert(product.m_validLeptons.end(),
                                  product.m_validTaus.begin(), product.m_validTaus.end());

    LOG(DEBUG) << "Total of " << product.m_validLeptons.size() << " valid leptons.";

    LOG(DEBUG) << "Inserting " << product.m_invalidElectrons.size() << " invalid electrons.";
    product.m_invalidLeptons.insert(product.m_invalidLeptons.end(),
                                    product.m_invalidElectrons.begin(), product.m_invalidElectrons.end());

    LOG(DEBUG) << "Inserting " << product.m_invalidMuons.size() << " invalid muons.";
    product.m_invalidLeptons.insert(product.m_invalidLeptons.end(),
                                    product.m_invalidMuons.begin(), product.m_invalidMuons.end());

    LOG(DEBUG) << "Inserting " << product.m_invalidTaus.size() << " invalid taus.";
    product.m_invalidLeptons.insert(product.m_invalidLeptons.end(),
                                    product.m_invalidTaus.begin(), product.m_invalidTaus.end());

    LOG(DEBUG) << "Total of " << product.m_invalidLeptons.size() << " invalid leptons.";

    // sort vectors of leptons by pt
    std::sort(product.m_validLeptons.begin(), product.m_validLeptons.end(),
              [](KLepton const *lepton1, KLepton const *lepton2) -> bool {
                  return lepton1->p4.Pt() > lepton2->p4.Pt();
              });
    std::sort(product.m_invalidLeptons.begin(), product.m_invalidLeptons.end(),
              [](KLepton const *lepton1, KLepton const *lepton2) -> bool {
                  return lepton1->p4.Pt() > lepton2->p4.Pt();
              });
}

