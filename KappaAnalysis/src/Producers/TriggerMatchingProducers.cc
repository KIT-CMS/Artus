
#include "Artus/KappaAnalysis/interface/Producers/TriggerMatchingProducers.h"


std::string ElectronTriggerMatchingProducer::GetProducerId() const
{
	return "ElectronTriggerMatchingProducer";
}

ElectronTriggerMatchingProducer::ElectronTriggerMatchingProducer() :
	TriggerMatchingProducerBase<KElectron>(&KappaProduct::m_triggerMatchedElectrons,
	                                       &KappaProduct::m_detailedTriggerMatchedElectrons,
	                                       &KappaProduct::m_electronTriggerMatch,
	                                       &KappaProduct::m_validElectrons,
	                                       &KappaProduct::m_invalidElectrons,
	                                       &KappaProduct::m_settingsElectronTriggerFiltersByIndex,
	                                       &KappaProduct::m_settingsElectronTriggerFiltersByHltName,
	                                       &KappaSettings::GetElectronTriggerFilterNames,
	                                       &KappaSettings::GetDeltaRTriggerMatchingElectrons,
	                                       &KappaSettings::GetInvalidateNonMatchingElectrons)
{
}

void ElectronTriggerMatchingProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                              KappaSettings const& settings) const
{
	TriggerMatchingProducerBase<KElectron>::Produce(event, product, settings);
	
	for (std::map<KElectron*, KLV*>::iterator it = product.m_triggerMatchedElectrons.begin();
	     it != product.m_triggerMatchedElectrons.end(); ++it)
	{
		product.m_triggerMatchedLeptons[&(*(it->first))] = &(*(it->second));
	}
	
	for (std::map<KElectron*, std::map<std::string, std::map<std::string, std::vector<KLV> > > >::iterator it = product.m_detailedTriggerMatchedElectrons.begin();
	     it != product.m_detailedTriggerMatchedElectrons.end(); ++it)
	{
		product.m_detailedTriggerMatchedLeptons[&(*(it->first))] = &(it->second);
	}
	
	for (std::map<KElectron*, std::map<std::string, bool > >::iterator it = product.m_electronTriggerMatch.begin();
	     it != product.m_electronTriggerMatch.end(); ++it)
	{
		product.m_leptonTriggerMatch[&(*(it->first))] = &(it->second);
	}
}


std::string MuonTriggerMatchingProducer::GetProducerId() const
{
	return "MuonTriggerMatchingProducer";
}

MuonTriggerMatchingProducer::MuonTriggerMatchingProducer() :
	TriggerMatchingProducerBase<KMuon>(&KappaProduct::m_triggerMatchedMuons,
	                                   &KappaProduct::m_detailedTriggerMatchedMuons,
                                           &KappaProduct::m_muonTriggerMatch,
	                                   &KappaProduct::m_validMuons,
	                                   &KappaProduct::m_invalidMuons,
	                                   &KappaProduct::m_settingsMuonTriggerFiltersByIndex,
	                                   &KappaProduct::m_settingsMuonTriggerFiltersByHltName,
	                                   &KappaSettings::GetMuonTriggerFilterNames,
	                                   &KappaSettings::GetDeltaRTriggerMatchingMuons,
	                                   &KappaSettings::GetInvalidateNonMatchingMuons)
{
}

void MuonTriggerMatchingProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                              KappaSettings const& settings) const
{
	TriggerMatchingProducerBase<KMuon>::Produce(event, product, settings);
	
	for (std::map<KMuon*, KLV*>::iterator it = product.m_triggerMatchedMuons.begin();
	     it != product.m_triggerMatchedMuons.end(); ++it)
	{
		product.m_triggerMatchedLeptons[&(*(it->first))] = &(*(it->second));
	}
	
	for (std::map<KMuon*, std::map<std::string, std::map<std::string, std::vector<KLV> > > >::iterator it = product.m_detailedTriggerMatchedMuons.begin();
	     it != product.m_detailedTriggerMatchedMuons.end(); ++it)
	{
		product.m_detailedTriggerMatchedLeptons[&(*(it->first))] = &(it->second);
	}

	for (std::map<KMuon*, std::map<std::string, bool > >::iterator it = product.m_muonTriggerMatch.begin();
	     it != product.m_muonTriggerMatch.end(); ++it)
	{
		product.m_leptonTriggerMatch[&(*(it->first))] = &(it->second);
	}
}


std::string TauTriggerMatchingProducer::GetProducerId() const
{
	return "TauTriggerMatchingProducer";
}

TauTriggerMatchingProducer::TauTriggerMatchingProducer() :
	TriggerMatchingProducerBase<KTau>(&KappaProduct::m_triggerMatchedTaus,
	                                  &KappaProduct::m_detailedTriggerMatchedTaus,
                                          &KappaProduct::m_tauTriggerMatch,
	                                  &KappaProduct::m_validTaus,
	                                  &KappaProduct::m_invalidTaus,
	                                  &KappaProduct::m_settingsTauTriggerFiltersByIndex,
	                                  &KappaProduct::m_settingsTauTriggerFiltersByHltName,
	                                  &KappaSettings::GetTauTriggerFilterNames,
	                                  &KappaSettings::GetDeltaRTriggerMatchingTaus,
	                                  &KappaSettings::GetInvalidateNonMatchingTaus)
{
}

void TauTriggerMatchingProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                              KappaSettings const& settings) const
{
	TriggerMatchingProducerBase<KTau>::Produce(event, product, settings);
	
	for (std::map<KTau*, KLV*>::iterator it = product.m_triggerMatchedTaus.begin();
	     it != product.m_triggerMatchedTaus.end(); ++it)
	{
		product.m_triggerMatchedLeptons[&(*(it->first))] = &(*(it->second));
	}
	
	for (std::map<KTau*, std::map<std::string, std::map<std::string, std::vector<KLV> > > >::iterator it = product.m_detailedTriggerMatchedTaus.begin();
	     it != product.m_detailedTriggerMatchedTaus.end(); ++it)
	{
		product.m_detailedTriggerMatchedLeptons[&(*(it->first))] = &(it->second);
	}

	for (std::map<KTau*, std::map<std::string, bool > >::iterator it = product.m_tauTriggerMatch.begin();
	     it != product.m_tauTriggerMatch.end(); ++it)
	{
		product.m_leptonTriggerMatch[&(*(it->first))] = &(it->second);
	}

        if (settings.GetTauTriggerCheckL1Match().size() > 0)
        {
                for (std::map<KTau*, std::map<std::string, bool > >::iterator it = product.m_tauTriggerMatch.begin();
                     it != product.m_tauTriggerMatch.end(); ++it)
                {
                        float deltaRMax = 0.5;
                        float bestL1Pt = -1.0;
                        bool l1matched = false;
                        for (std::vector<KL1Tau>::iterator l1o = event.m_l1taus->begin(); l1o != event.m_l1taus->end(); ++l1o)
                        {
                                float currentDeltaR = ROOT::Math::VectorUtil::DeltaR((it->first)->p4, l1o->p4);
                                if(currentDeltaR < deltaRMax)
                                {
                                        bestL1Pt = l1o->p4.Pt();
                                        deltaRMax = currentDeltaR;
                                }
                        }
                        if(bestL1Pt >= 32.0) l1matched = true;
                        std::map<std::string, bool> hlt_to_l1;
                        for (std::vector<std::string>::iterator hlt = settings.GetTauTriggerCheckL1Match().begin(); hlt != settings.GetTauTriggerCheckL1Match().end(); ++hlt)
                        {
                                hlt_to_l1[*hlt] = l1matched;
                        }
                        product.m_detailedL1MatchedLeptons[&(*(it->first))] = hlt_to_l1;
                }
        }
}


std::string JetTriggerMatchingProducer::GetProducerId() const {
	return "JetTriggerMatchingProducer";
}

JetTriggerMatchingProducer::JetTriggerMatchingProducer() :
	TriggerMatchingProducerBase<KBasicJet>(&KappaProduct::m_triggerMatchedJets,
	                                       &KappaProduct::m_detailedTriggerMatchedJets,
                                               &KappaProduct::m_jetTriggerMatch,
	                                       &KappaProduct::m_validJets,
	                                       &KappaProduct::m_invalidJets,
	                                       &KappaProduct::m_settingsJetTriggerFiltersByIndex,
	                                       &KappaProduct::m_settingsJetTriggerFiltersByHltName,
	                                       &KappaSettings::GetJetTriggerFilterNames,
	                                       &KappaSettings::GetDeltaRTriggerMatchingJets,
	                                       &KappaSettings::GetInvalidateNonMatchingJets)
{
}

