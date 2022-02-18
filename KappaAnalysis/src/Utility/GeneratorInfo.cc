
#include "Artus/KappaAnalysis/interface/Utility/GeneratorInfo.h"

KappaEnumTypes::GenMatchingCode GeneratorInfo::GetGenMatchingCode(KGenParticle* genParticle)
{
	int pdgId = std::abs(genParticle->pdgId);
	
	if (pdgId == 11 && genParticle->p4.Pt() > 8. && genParticle->isPrompt())
	{
		return KappaEnumTypes::GenMatchingCode::IS_ELE_PROMPT;
	}
	else if (pdgId == 13 && genParticle->p4.Pt() > 8. && genParticle->isPrompt())
	{
		return KappaEnumTypes::GenMatchingCode::IS_MUON_PROMPT;
	}
	else if (pdgId == 11 && genParticle->p4.Pt() > 8. && genParticle->isDirectPromptTauDecayProduct())
	{
		return KappaEnumTypes::GenMatchingCode::IS_ELE_FROM_TAU;
	}
	else if (pdgId == 13 && genParticle->p4.Pt() > 8. && genParticle->isDirectPromptTauDecayProduct())
	{
		return KappaEnumTypes::GenMatchingCode::IS_MUON_FROM_TAU;
	}
	else if (pdgId == 15 && GetVisibleLV(genParticle)->Pt() > 15.)
	{
		return KappaEnumTypes::GenMatchingCode::IS_TAU_HAD_DECAY;
	}
	else
	{
		return KappaEnumTypes::GenMatchingCode::IS_FAKE;
	}
}

RMFLV* GeneratorInfo::GetVisibleLV(KGenParticle* genParticle)
{
	RMFLV* visibleLV = nullptr;
	if (genParticle)
	{
		KGenTau* genTau = dynamic_cast<KGenTau*>(genParticle);
		if (genTau)
		{
			visibleLV = &(genTau->visible.p4);
		}
		else
		{
			visibleLV = &(genParticle->p4);
		}
	}
	return visibleLV;
}

KGenParticle* GeneratorInfo::GetGenMatchedParticle(
		KLepton* lepton,
		std::map<KLepton*, KGenParticle*> const& leptonGenParticleMap,
		std::map<KLepton*, KGenTau*> const& leptonGenTauMap
)
{
	KGenParticle* defaultGenParticle = nullptr;
	KGenParticle* genParticle = SafeMap::GetWithDefault(leptonGenParticleMap, lepton, defaultGenParticle);

	KGenTau* defaultGenTau = nullptr;
	KGenTau* genTau = SafeMap::GetWithDefault(leptonGenTauMap, lepton, defaultGenTau);
	
	float deltaRTauGenTau = (genTau ? ROOT::Math::VectorUtil::DeltaR(lepton->p4, genTau->visible.p4) : std::numeric_limits<float>::max());
	float deltaRTauGenParticle = (genParticle ? ROOT::Math::VectorUtil::DeltaR(lepton->p4, genParticle->p4) : std::numeric_limits<float>::max());

	if (deltaRTauGenParticle <= deltaRTauGenTau)
	{
		return genParticle;
	}
	else
	{
		return genTau;
	}
}

// This matching algorithm is used by the Wisconsin group and was provided by Cecile.
KappaEnumTypes::GenMatchingCode GeneratorInfo::GetGenMatchingCodeUW(
		KappaTypes::event_type const& event,
		KLepton* lepton
)
{
	LOG(DEBUG) << "----- Generator Matching (UW) -----";
	LOG(DEBUG) << "Processing run:lumi:event " << event.m_eventInfo->nRun << ":" << event.m_eventInfo->nLumi << ":" << event.m_eventInfo->nEvent;
        LOG(DEBUG) << "Considered reco lepton p4: " << lepton->p4;
	if(event.m_genParticles && event.m_genParticles->size() > 0)
	{
		KGenParticle closest = event.m_genParticles->at(0);
		double closestDR = 999;
		std::vector<RMFLV> genTaus = BuildGenTausUW(event);

		//find closest lepton fulfilling the requirements
		for (typename std::vector<KGenParticle>::iterator genParticle = event.m_genParticles->begin();
			genParticle != event.m_genParticles->end(); ++genParticle)
		{
			int pdgId = std::abs(genParticle->pdgId);
			if (genParticle->p4.Pt() > 8. && (pdgId == 11 || pdgId == 13) && (genParticle->isPrompt() || genParticle->isDirectPromptTauDecayProduct())){
				double tmpDR = ROOT::Math::VectorUtil::DeltaR(lepton->p4, genParticle->p4);
                                LOG(DEBUG) << "\tcomputed deltaR, Pt: " << tmpDR << ", " << genParticle->p4.Pt() << " for paticle " << pdgId;
				if (tmpDR < closestDR)
				{
					closest = *genParticle;
					closestDR = tmpDR;
				}
			}
		}
                LOG(DEBUG) << "Closest electron or muon found with deltaR = " << closestDR;
		//check whether there are closer tau jets within a 0.2 cone
                LOG(DEBUG) << "Processing visible hadronic gen taus";
		for(auto genTau : genTaus)
		{
			double tauDR = ROOT::Math::VectorUtil::DeltaR(lepton->p4, genTau);
                         LOG(DEBUG) << "\tcomputed deltaR, Pt: " << tauDR << ", " << genTau.Pt();
			if (genTau.Pt() > 15. && tauDR < 0.2 && tauDR < closestDR) return KappaEnumTypes::GenMatchingCode::IS_TAU_HAD_DECAY;
		}
                LOG(DEBUG) << "No visible tau_h found with deltaR < 0.2 && with Pt > 15";
		//since there are no closer tau jets check whether dR < 0.2 is fulfilled and return lepton type
		int pdgId = std::abs(closest.pdgId);
		if (closestDR < 0.2){
			if (pdgId == 11 && closest.isPrompt()) return KappaEnumTypes::GenMatchingCode::IS_ELE_PROMPT;
			if (pdgId == 13 && closest.isPrompt()) return KappaEnumTypes::GenMatchingCode::IS_MUON_PROMPT;
			if (pdgId == 11 && closest.isDirectPromptTauDecayProduct()) return KappaEnumTypes::GenMatchingCode::IS_ELE_FROM_TAU;
			if (pdgId == 13 && closest.isDirectPromptTauDecayProduct()) return KappaEnumTypes::GenMatchingCode::IS_MUON_FROM_TAU;
		}
                LOG(DEBUG) << "Closest lepton at deltaR >= 0.2";
		return KappaEnumTypes::GenMatchingCode::IS_FAKE;
	}
	return KappaEnumTypes::GenMatchingCode::NONE;
}

float GeneratorInfo::GetGenMatchedParticlePtUW(
		KappaTypes::event_type const& event,
		KLepton* lepton
)
{
	LOG(DEBUG) << "----- Generator Matching (UW) -----";
	LOG(DEBUG) << "Processing run:lumi:event " << event.m_eventInfo->nRun << ":" << event.m_eventInfo->nLumi << ":" << event.m_eventInfo->nEvent;
        LOG(DEBUG) << "Considered reco lepton p4: " << lepton->p4;
	if(event.m_genParticles && event.m_genParticles->size() > 0)
	{
		KGenParticle closest = event.m_genParticles->at(0);
		double closestDR = 999;
		std::vector<RMFLV> genTaus = BuildGenTausUW(event);

		//find closest lepton fulfilling the requirements
		for (typename std::vector<KGenParticle>::iterator genParticle = event.m_genParticles->begin();
			genParticle != event.m_genParticles->end(); ++genParticle)
		{
			int pdgId = std::abs(genParticle->pdgId);
			if (genParticle->p4.Pt() > 8. && (pdgId == 11 || pdgId == 13) && (genParticle->isPrompt() || genParticle->isDirectPromptTauDecayProduct())){
				double tmpDR = ROOT::Math::VectorUtil::DeltaR(lepton->p4, genParticle->p4);
                LOG(DEBUG) << "\tcomputed deltaR, Pt: " << tmpDR << ", " << genParticle->p4.Pt() << " for paticle " << pdgId;
				if (tmpDR < closestDR)
				{
					closest = *genParticle;
					closestDR = tmpDR;
				}
			}
		}
		for(auto genTau : genTaus)
		{
			double tauDR = ROOT::Math::VectorUtil::DeltaR(lepton->p4, genTau);
			if (genTau.Pt() > 15. && tauDR < 0.2 && tauDR < closestDR){
				LOG(DEBUG) << "Found gentau - dR: " << tauDR << ", pt: " << genTau.Pt();
				return genTau.Pt();
			}
		}
		LOG(DEBUG) << "Found closest genParticle - dR: " << closestDR << ", pt: " << closest.p4.Pt();
		if (closestDR > 0.2) return -10.0;
		else return closest.p4.Pt();
	}
	return  -10.0;

}

float GeneratorInfo::GetGenMatchedParticleEtaUW(
		KappaTypes::event_type const& event,
		KLepton* lepton
)
{
	LOG(DEBUG) << "----- Generator Matching (UW) -----";
	LOG(DEBUG) << "Processing run:lumi:event " << event.m_eventInfo->nRun << ":" << event.m_eventInfo->nLumi << ":" << event.m_eventInfo->nEvent;
        LOG(DEBUG) << "Considered reco lepton p4: " << lepton->p4;
	if(event.m_genParticles && event.m_genParticles->size() > 0)
	{
		KGenParticle closest = event.m_genParticles->at(0);
		double closestDR = 999;
		std::vector<RMFLV> genTaus = BuildGenTausUW(event);

		//find closest lepton fulfilling the requirements
		for (typename std::vector<KGenParticle>::iterator genParticle = event.m_genParticles->begin();
			genParticle != event.m_genParticles->end(); ++genParticle)
		{
			int pdgId = std::abs(genParticle->pdgId);
			if (genParticle->p4.Pt() > 8. && (pdgId == 11 || pdgId == 13) && (genParticle->isPrompt() || genParticle->isDirectPromptTauDecayProduct())){
				double tmpDR = ROOT::Math::VectorUtil::DeltaR(lepton->p4, genParticle->p4);
                LOG(DEBUG) << "\tcomputed deltaR, Pt: " << tmpDR << ", " << genParticle->p4.Pt() << " for paticle " << pdgId;
				if (tmpDR < closestDR)
				{
					closest = *genParticle;
					closestDR = tmpDR;
				}
			}
		}
		for(auto genTau : genTaus)
		{
			double tauDR = ROOT::Math::VectorUtil::DeltaR(lepton->p4, genTau);
			if (genTau.Pt() > 15. && tauDR < 0.2 && tauDR < closestDR){
				LOG(DEBUG) << "Found gentau - dR: " << tauDR << ", eta: " << genTau.Eta();
				return genTau.Eta();
			}
		}
		LOG(DEBUG) << "Found closest genParticle - dR: " << closestDR << ", eta: " << closest.p4.Eta();
		if (closestDR > 0.2) return -10.0;
		else return closest.p4.Eta();
	}
	return  -10.0;

}

std::vector<RMFLV> GeneratorInfo::BuildGenTausUW(
		KappaTypes::event_type const& event
)
{
	std::vector<RMFLV> genTaus;

	if(event.m_genParticles && event.m_genParticles->size() > 0)
	{
		for (typename std::vector<KGenParticle>::iterator genParticle = event.m_genParticles->begin();
			genParticle != event.m_genParticles->end(); ++genParticle)
		{
			if (std::abs(genParticle->pdgId) == 15)
			{
				if (genParticle->isPrompt())
				{
					if (genParticle->nDaughters() > 0)
					{
						bool has_tau_daughter = false;
						bool has_lepton_daughter = false;
						for (unsigned dau = 0; dau < genParticle->nDaughters(); ++dau)
						{
							assert(genParticle->daughterIndex(dau) >= 0);
							int pdgId = std::abs(event.m_genParticles->at(genParticle->daughterIndex(dau)).pdgId);
							if (pdgId == 15) has_tau_daughter = true;
							if (pdgId == 11 || pdgId == 13) has_lepton_daughter = true;
						}
						if (has_tau_daughter) continue;
						if (has_lepton_daughter) continue;

						RMFLV genTau;
						for (size_t dau = 0; dau < genParticle->nDaughters(); ++dau)
						{
							assert(genParticle->daughterIndex(dau) >= 0);
							int pdgId = std::abs(event.m_genParticles->at(genParticle->daughterIndex(dau)).pdgId);
							if (pdgId == 12 || pdgId == 14 || pdgId == 16) continue;
							genTau += event.m_genParticles->at(genParticle->daughterIndex(dau)).p4;
						}
						genTaus.push_back(genTau);
					}
				}
			}
		}
	}
	return genTaus;
}
