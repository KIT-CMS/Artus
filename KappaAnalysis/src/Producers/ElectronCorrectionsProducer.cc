
#include "Artus/KappaAnalysis/interface/Producers/ElectronCorrectionsProducer.h"
#include "Artus/KappaAnalysis/interface/Utility/GeneratorInfo.h"

std::string ElectronCorrectionsProducer::GetProducerId() const {
	return "ElectronCorrectionsProducer";
}

void ElectronCorrectionsProducer::Init(setting_type const& settings)
{
	KappaProducerBase::Init(settings);

	m_requestedNames = settings.GetElectronEnergyCorrectionTags();
}

void ElectronCorrectionsProducer::OnLumi(KappaEvent const& event, KappaSettings const& settings)
{
	assert(event.m_electronMetadata);

	// On first event, fill the electronCorrectionNamesMap
	if(m_electronCorrectionNamesMap.empty())
	{
		auto tmp_electronCorrectionNamesMap = event.m_electronMetadata->getIdNamesMap(m_requestedNames);
		for(const auto& electronCorrectionNamePair: tmp_electronCorrectionNamesMap)
		{
			m_electronCorrectionNamesMap[electronCorrectionNamePair.first] = electronCorrectionNamePair.second;
		}
	}

	if(m_electronCorrectionNamesMap.size() > 1)
	{
		LOG(FATAL) << "[ElectronCorrectionsProducer] The current implementation doesn't support the treatment of more than one correction at a time. Please reconfigure!";
	}
}

void ElectronCorrectionsProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                          KappaSettings const& settings) const
{
	assert(event.m_electrons);

	// create a copy of all electrons in the event
	product.m_correctedElectrons.clear();
	product.m_correctedElectrons.resize(event.m_electrons->size());
	size_t electronIndex = 0;
	for (KElectrons::const_iterator electron = event.m_electrons->begin();
		 electron != event.m_electrons->end(); ++electron)
	{
		product.m_correctedElectrons[electronIndex] = std::shared_ptr<KElectron>(new KElectron(*electron));
		product.m_originalLeptons[product.m_correctedElectrons[electronIndex].get()] = &(*electron);
		++electronIndex;
	}
	
	// perform corrections on copied electrons
	for (std::vector<std::shared_ptr<KElectron> >::iterator electron = product.m_correctedElectrons.begin();
		 electron != product.m_correctedElectrons.end(); ++electron)
	{
		// Check whether corrections should be applied at all
		bool isRealElectron = false;
		if (settings.GetCorrectOnlyRealElectrons())
		{
			KappaEnumTypes::GenMatchingCode genMatchingCode = KappaEnumTypes::GenMatchingCode::NONE;
			if (settings.GetUseUWGenMatching())
			{
				genMatchingCode = GeneratorInfo::GetGenMatchingCodeUW(event, const_cast<KLepton*>(product.m_originalLeptons[electron->get()]));
			}
			else
			{
				KGenParticle* genParticle = GeneratorInfo::GetGenMatchedParticle(const_cast<KLepton*>(product.m_originalLeptons[electron->get()]), product.m_genParticleMatchedLeptons, product.m_genTauMatchedLeptons);
				if (genParticle)
				{
					genMatchingCode = GeneratorInfo::GetGenMatchingCode(genParticle);
				}
			}

			if (genMatchingCode == KappaEnumTypes::GenMatchingCode::IS_ELE_PROMPT ||
				genMatchingCode == KappaEnumTypes::GenMatchingCode::IS_ELE_FROM_TAU)
				isRealElectron = true;
		}

		// No general correction available
	
		// perform possible analysis-specific corrections
		if ((!settings.GetCorrectOnlyRealElectrons() || (settings.GetCorrectOnlyRealElectrons() && isRealElectron)) && settings.GetApplyElectronEnergyCorrections())
			AdditionalCorrections(electron->get(), event, product, settings);

		// make sure to also save the corrected lepton and the matched genParticle in the map
		// if we match genParticles to all leptons
		if (settings.GetRecoElectronMatchingGenParticleMatchAllElectrons())
		{
			product.m_genParticleMatchedElectrons[electron->get()] =  &(*product.m_genParticleMatchedElectrons[static_cast<KElectron*>(const_cast<KLepton*>(product.m_originalLeptons[electron->get()]))]);
			product.m_genParticleMatchedLeptons[electron->get()] = &(*product.m_genParticleMatchedLeptons[const_cast<KLepton*>(product.m_originalLeptons[electron->get()])]);
		}
		if (settings.GetMatchAllElectronsGenTau())
		{
			product.m_genTauMatchedElectrons[electron->get()] = &(*product.m_genTauMatchedElectrons[static_cast<KElectron*>(const_cast<KLepton*>(product.m_originalLeptons[electron->get()]))]);
			product.m_genTauMatchedLeptons[electron->get()] = &(*product.m_genTauMatchedLeptons[const_cast<KLepton*>(product.m_originalLeptons[electron->get()])]);
		}
	}
	
	// sort vectors of corrected electrons by pt
	std::sort(product.m_correctedElectrons.begin(), product.m_correctedElectrons.end(),
	          [](std::shared_ptr<KElectron> electron1, std::shared_ptr<KElectron> electron2) -> bool
	          { return electron1->p4.Pt() > electron2->p4.Pt(); });
}

void ElectronCorrectionsProducer::AdditionalCorrections(KElectron* electron, KappaEvent const& event,
                                                        KappaProduct& product, KappaSettings const& settings) const
{
	//size_t iter = 0;
	for(const auto& electronCorrectionNamePair: m_electronCorrectionNamesMap)
	{
		float correctionFactor = electron->getId(electronCorrectionNamePair.second) / electron->p4.E();
		electron->p4 = electron->p4 * correctionFactor;
	}
}

