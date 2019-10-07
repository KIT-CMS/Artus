
#include "Artus/KappaAnalysis/interface/Producers/GenBosonProducers.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/Utility.h"
#include "Artus/Utility/interface/DefaultValues.h"


std::string GenBosonFromGenParticlesProducer::GetProducerId() const {
	return "GenBosonFromGenParticlesProducer";
}

void GenBosonFromGenParticlesProducer::Init(KappaSettings const& settings)
{
	ProducerBase<KappaTypes>::Init(settings);
	if (!settings.GetMatchNMSSMBosons()) {
	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonParticleFound", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBosonParticle != nullptr);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("genBosonLV", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV;
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV.Pt();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV.Eta();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV.Phi();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV.M();
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonLVFound", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLVFound;
	});
	}
	else {
	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonParticleFound_h1", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBosonParticle_h1 != nullptr);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("genBosonLV_h1", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h1;
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonPt_h1", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h1.Pt();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonEta_h1", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h1.Eta();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonPhi_h1", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h1.Phi();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonMass_h1", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h1.M();
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonLVFound_h1", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLVFound_h1;
	});
	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonParticleFound_h2", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBosonParticle_h2 != nullptr);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("genBosonLV_h2", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h2;
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonPt_h2", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h2.Pt();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonEta_h2", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h2.Eta();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonPhi_h2", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h2.Phi();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonMass_h2", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h2.M();
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonLVFound_h2", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLVFound_h2;
	});
	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonParticleFound_h3", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBosonParticle_h3 != nullptr);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("genBosonLV_h3", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h3;
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonPt_h3", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h3.Pt();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonEta_h3", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h3.Eta();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonPhi_h3", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h3.Phi();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonMass_h3", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV_h3.M();
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonLVFound_h3", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLVFound_h3;
	});


	}
}

void GenBosonFromGenParticlesProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                               KappaSettings const& settings) const
{
	assert(event.m_genParticles);
	FindGenBoson(event, product, settings);
}

void GenBosonFromGenParticlesProducer::FindGenBoson(KappaEvent const& event, KappaProduct& product,
                                                    KappaSettings const& settings, unsigned int startIndex) const
{
	if (!settings.GetMatchNMSSMBosons()) {

		product.m_genBosonParticle = nullptr;
		product.m_genBosonLV = RMFLV();
		product.m_genBosonLVFound = false;
		
		for (unsigned int genParticleIndex = startIndex; genParticleIndex < event.m_genParticles->size(); ++genParticleIndex)
		{
			KGenParticle* genParticle = &(event.m_genParticles->at(genParticleIndex));
				if (Utility::Contains(settings.GetBosonPdgIds(), std::abs(genParticle->pdgId)) && Utility::Contains(settings.GetBosonStatuses(), genParticle->status()) && genParticle->isLastCopy())
				{
					//if (!genParticle->isLastCopy()) std::cout << genParticle->isLastCopy() << std::endl;
					product.m_genBosonParticle = genParticle;
					product.m_genBosonLV = genParticle->p4;
					product.m_genBosonLVFound = true;
					break;
				}
			}
	}
	else {
		product.m_genBosonParticle_h1 = nullptr;
		product.m_genBosonLV_h1 = RMFLV();
		product.m_genBosonLVFound_h1 = false;
		product.m_genBosonParticle_h2 = nullptr;
		product.m_genBosonLV_h2 = RMFLV();
		product.m_genBosonLVFound_h2 = false;
		product.m_genBosonParticle_h3 = nullptr;
		product.m_genBosonLV_h3 = RMFLV();
		product.m_genBosonLVFound_h3 = false;
		for (unsigned int genParticleIndex = startIndex; genParticleIndex < event.m_genParticles->size(); ++genParticleIndex)
		{
			KGenParticle* genParticle = &(event.m_genParticles->at(genParticleIndex));
				if ((std::abs(genParticle->pdgId) == 25) && Utility::Contains(settings.GetBosonStatuses(), genParticle->status()) && genParticle->isLastCopy())
				{
					//if (!genParticle->isLastCopy()) std::cout << genParticle->isLastCopy() << std::endl;
					product.m_genBosonParticle_h1 = genParticle;
					product.m_genBosonLV_h1 = genParticle->p4;
					product.m_genBosonLVFound_h1 = true;
					// std::cout << "Found GenBoson with status " << genParticle->status() << " and pdgId " << genParticle->pdgId << std::endl;
				}
				else if ((std::abs(genParticle->pdgId) == 35) && Utility::Contains(settings.GetBosonStatuses(), genParticle->status()) && genParticle->isLastCopy())
				{
					//if (!genParticle->isLastCopy()) std::cout << genParticle->isLastCopy() << std::endl;
					product.m_genBosonParticle_h2 = genParticle;
					product.m_genBosonLV_h2 = genParticle->p4;
					product.m_genBosonLVFound_h2 = true;
					// std::cout << "Found GenBoson with status " << genParticle->status() << " and pdgId " << genParticle->pdgId << std::endl;
				}
				else if ((std::abs(genParticle->pdgId) == 45) && Utility::Contains(settings.GetBosonStatuses(), genParticle->status()) && genParticle->isLastCopy())
				{
					//if (!genParticle->isLastCopy()) std::cout << genParticle->isLastCopy() << std::endl;
					product.m_genBosonParticle_h3 = genParticle;
					product.m_genBosonLV_h3 = genParticle->p4;
					product.m_genBosonLVFound_h3 = true;
					// std::cout << "Found GenBoson with status " << genParticle->status() << " and pdgId " << genParticle->pdgId << std::endl;
				}
			}
	}
}


std::string GenBosonProductionProducer::GetProducerId() const {
	return "GenBosonProductionProducer";
}

void GenBosonProductionProducer::Init(KappaSettings const& settings)
{
	GenBosonFromGenParticlesProducer::Init(settings);
}

void GenBosonProductionProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                         KappaSettings const& settings) const
{
	assert(product.m_genBosonParticle != nullptr);
	
	// search for boson index
	unsigned int bosonIndex = 0;
	for (KGenParticles::const_iterator genParticle = event.m_genParticles->begin();
		 genParticle != event.m_genParticles->end(); ++genParticle)
	{
		if (product.m_genBosonParticle == &(*genParticle))
		{
			break;
		}
		++bosonIndex;
	}
	
	product.m_genParticlesProducingBoson = FindMothersWithDifferentPdgId(event.m_genParticles, bosonIndex, product.m_genBosonParticle->pdgId);
}

std::vector<KGenParticle*> GenBosonProductionProducer::FindMothersWithDifferentPdgId(
		KGenParticles* genParticles,
		unsigned int currentIndex,
		int currentPdgId) const
{
	std::vector<KGenParticle*> mothers;
	
	unsigned int index = 0;
	for (KGenParticles::iterator genParticle = genParticles->begin();
		 genParticle != genParticles->end(); ++genParticle)
	{
		if (Utility::Contains(genParticle->daughterIndices, currentIndex))
		{
			if (genParticle->pdgId == currentPdgId)
			{
				std::vector<KGenParticle*> tmpMothers = FindMothersWithDifferentPdgId(genParticles, index, currentPdgId);
				mothers.insert(mothers.end(), tmpMothers.begin(), tmpMothers.end());
			}
			else
			{
				mothers.push_back(&(*genParticle));
			}
		}
		++index;
	}
	
	return mothers;
}


std::string GenBosonDiLeptonDecayModeProducer::GetProducerId() const {
	return "GenBosonDiLeptonDecayModeProducer";
}

void GenBosonDiLeptonDecayModeProducer::Init(KappaSettings const& settings)
{
	GenBosonFromGenParticlesProducer::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	for (size_t leptonIndex = 0; leptonIndex < 2; ++leptonIndex)
	{
		std::string lepQuantityNameBase = "genBosonLep" + std::to_string(leptonIndex+1);
		std::string tauQuantityNameBase = "genBosonTau" + std::to_string(leptonIndex+1);
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(lepQuantityNameBase+"LV", [leptonIndex](event_type const& event, product_type const& product)
		{
			if (product.m_genLeptonsFromBosonDecay.size() > leptonIndex)
			{
				return product.m_genLeptonsFromBosonDecay.at(leptonIndex)->p4;
			}
			else
			{
				return DefaultValues::UndefinedRMFLV;
			}
		});
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(tauQuantityNameBase+"LV", [leptonIndex](event_type const& event, product_type const& product)
		{
			if (product.m_genLeptonsFromBosonDecay.size() > leptonIndex)
			{
				return ((std::abs(product.m_genLeptonsFromBosonDecay.at(leptonIndex)->pdgId) == DefaultValues::pdgIdTau) ? product.m_genLeptonsFromBosonDecay.at(leptonIndex)->p4 : DefaultValues::UndefinedRMFLV);
			}
			else
			{
				return DefaultValues::UndefinedRMFLV;
			}
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(tauQuantityNameBase+"VisibleLV", [leptonIndex](event_type const& event, product_type const& product)
		{
			if ((product.m_genLeptonsFromBosonDecay.size() > leptonIndex) &&
			    (std::abs(product.m_genLeptonsFromBosonDecay.at(leptonIndex)->pdgId) == DefaultValues::pdgIdTau))
			{
				KGenTau* genTau = SafeMap::GetWithDefault(product.m_validGenTausMap, product.m_genLeptonsFromBosonDecay.at(leptonIndex), static_cast<KGenTau*>(nullptr));
				return (genTau ? genTau->visible.p4 : DefaultValues::UndefinedRMFLV);
			}
			else
			{
				return DefaultValues::UndefinedRMFLV;
			}
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(tauQuantityNameBase+"DecayMode", [leptonIndex](event_type const& event, product_type const& product)
		{
			if ((product.m_genLeptonsFromBosonDecay.size() > leptonIndex) &&
			    (std::abs(product.m_genLeptonsFromBosonDecay.at(leptonIndex)->pdgId) == DefaultValues::pdgIdTau))
			{
				KGenTau* genTau = SafeMap::GetWithDefault(product.m_validGenTausMap, product.m_genLeptonsFromBosonDecay.at(leptonIndex), static_cast<KGenTau*>(nullptr));
				return (genTau ? genTau->genDecayMode() : DefaultValues::UndefinedInt);
			}
			else
			{
				return DefaultValues::UndefinedInt;
			}
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(tauQuantityNameBase+"NProngs", [leptonIndex](event_type const& event, product_type const& product)
		{
			if ((product.m_genLeptonsFromBosonDecay.size() > leptonIndex) &&
			    (std::abs(product.m_genLeptonsFromBosonDecay.at(leptonIndex)->pdgId) == DefaultValues::pdgIdTau))
			{
				KGenTau* genTau = SafeMap::GetWithDefault(product.m_validGenTausMap, product.m_genLeptonsFromBosonDecay.at(leptonIndex), static_cast<KGenTau*>(nullptr));
				return (genTau ? genTau->nProngs : DefaultValues::UndefinedInt);
			}
			else
			{
				return DefaultValues::UndefinedInt;
			}
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(tauQuantityNameBase+"NPi0s", [leptonIndex](event_type const& event, product_type const& product)
		{
			if ((product.m_genLeptonsFromBosonDecay.size() > leptonIndex) &&
			    (std::abs(product.m_genLeptonsFromBosonDecay.at(leptonIndex)->pdgId) == DefaultValues::pdgIdTau))
			{
				KGenTau* genTau = SafeMap::GetWithDefault(product.m_validGenTausMap, product.m_genLeptonsFromBosonDecay.at(leptonIndex), static_cast<KGenTau*>(nullptr));
				return (genTau ? genTau->nPi0s : DefaultValues::UndefinedInt);
			}
			else
			{
				return DefaultValues::UndefinedInt;
			}
		});
	}
}

void GenBosonDiLeptonDecayModeProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                                KappaSettings const& settings) const
{
	FindGenDiLeptons(event, product, settings);
}

void GenBosonDiLeptonDecayModeProducer::FindGenDiLeptons(KappaEvent const& event, KappaProduct& product,
                                                         KappaSettings const& settings) const
{
	product.m_genLeptonsFromBosonDecay.clear();
	
	// If no boson has been found in the event, try to reconstruct it from the first two decay
	// products available in the list of gen. particles
	// https://hypernews.cern.ch/HyperNews/CMS/get/generators/2802/1.html
	if (product.m_genBosonParticle == nullptr)
	{
		size_t iDaughter = 0;
		RMFLV genBosonLV;
		
		for (KGenParticles::iterator genParticle = event.m_genParticles->begin();
		     genParticle != event.m_genParticles->end() && (iDaughter < 2); ++genParticle)
		{
			if ((std::abs(genParticle->pdgId) == DefaultValues::pdgIdElectron) ||
			    (std::abs(genParticle->pdgId) == DefaultValues::pdgIdMuon) ||
			    (std::abs(genParticle->pdgId) == DefaultValues::pdgIdTau))
			// if (genParticle->isPrompt() && genParticle->isPromptDecayed())
			{
				genBosonLV += (*genParticle).p4;
				product.m_genLeptonsFromBosonDecay.push_back(&(*genParticle));
				++iDaughter;
			}
		}
		
		product.m_genBosonLV = genBosonLV;
		product.m_genBosonLVFound = (iDaughter == 2);
	}
	else
	{
		bool rerun = false;
		for (std::vector<unsigned int>::const_iterator decayParticleIndex = product.m_genBosonParticle->daughterIndices.begin();
		     decayParticleIndex != product.m_genBosonParticle->daughterIndices.end(); ++decayParticleIndex)
		{
			int pdgId = std::abs(event.m_genParticles->at(*decayParticleIndex).pdgId);
			if ((pdgId == DefaultValues::pdgIdElectron) ||
			    (pdgId == DefaultValues::pdgIdMuon) ||
			    (pdgId == DefaultValues::pdgIdTau)||
			    (pdgId == DefaultValues::pdgIdNuTau)||
			    (pdgId == DefaultValues::pdgIdNuMu)||
			    (pdgId == DefaultValues::pdgIdNuE))
			{
				product.m_genLeptonsFromBosonDecay.push_back(&(event.m_genParticles->at(*decayParticleIndex)));
			}
			/*else
			{
				rerun = true;
			}*/
		}
		if (product.m_genLeptonsFromBosonDecay.size() < 2)
		{
			rerun = true;
		}
		
		if (rerun)
		{
			// search for boson index
			unsigned int bosonIndex = 0;
			for (KGenParticles::const_iterator genParticle = event.m_genParticles->begin();
				 genParticle != event.m_genParticles->end(); ++genParticle)
			{
				if (product.m_genBosonParticle == &(*genParticle))
				{
					break;
				}
				++bosonIndex;
			}
			
			// search for next boson
			FindGenBoson(event, product, settings, bosonIndex+1);
			
			// restart search for leptons with next boson
			FindGenDiLeptons(event, product, settings);
		}
	}
}
