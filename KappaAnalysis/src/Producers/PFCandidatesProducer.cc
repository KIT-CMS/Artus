#include "Artus/KappaAnalysis/interface/Producers/PFCandidatesProducer.h"

void PFCandidatesProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFChargedHadrons", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_pfChargedHadrons.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFNeutralHadrons", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_pfNeutralHadrons.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFElectrons", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_pfElectrons.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFMuons", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_pfMuons.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFPhotons", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_pfPhotons.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFHadronicHF", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_pfHadronicHF.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFElectromagneticHF", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_pfElectromagneticHF.size();
	});
}

void PFCandidatesProducer::Produce(KappaEvent const& event, KappaProduct& product, KappaSettings const& settings) const
{
    LOG(DEBUG) << "\n[" << this->GetProducerId() << "]";
	// Sane memroy allocation based on a few DY events
	product.m_pfChargedHadrons.reserve(512);
	product.m_pfNeutralHadrons.reserve(128);
	product.m_pfPhotons.reserve(128);
	product.m_pfElectromagneticHF.reserve(264);
	for (KPFCandidates::const_iterator pfCandidate = event.m_packedPFCandidates->begin();
		pfCandidate != event.m_packedPFCandidates->end(); ++pfCandidate)
	{
		if (std::abs(pfCandidate->pdgId) == 211)
			fill_pfCandidate(product.m_pfChargedHadrons, product.m_pfChargedHadronsFromFirstPV,
				product.m_pfChargedHadronsNotFromFirstPV, &(*pfCandidate));
		else if (std::abs(pfCandidate->pdgId) == 130)
			fill_pfCandidate(product.m_pfNeutralHadrons, product.m_pfNeutralHadronsFromFirstPV,
				product.m_pfNeutralHadronsNotFromFirstPV, &(*pfCandidate));
		else if (std::abs(pfCandidate->pdgId) == 11)
			product.m_pfElectrons.push_back(&(*pfCandidate));
		else if (std::abs(pfCandidate->pdgId) == 13)
			product.m_pfMuons.push_back(&(*pfCandidate));
		else if (std::abs(pfCandidate->pdgId) == 22)
			fill_pfCandidate(product.m_pfPhotons, product.m_pfPhotonsFromFirstPV,
				product.m_pfPhotonsNotFromFirstPV, &(*pfCandidate));
		else if (std::abs(pfCandidate->pdgId) == 1)
			product.m_pfHadronicHF.push_back(&(*pfCandidate));
		else if (std::abs(pfCandidate->pdgId) == 2)
			product.m_pfElectromagneticHF.push_back(&(*pfCandidate));
		else
			LOG(WARNING) << "Unknown PFCandidate! Has pdgId: " << pfCandidate->pdgId;
	}
    LOG(DEBUG) << "Size m_pfChargedHadrons: " << product.m_pfChargedHadrons.size();
    LOG(DEBUG) << "Size m_pfNeutralHadrons: " << product.m_pfNeutralHadrons.size();
    LOG(DEBUG) << "Size m_pfElectrons: " << product.m_pfElectrons.size();
    LOG(DEBUG) << "Size m_pfMuons: " << product.m_pfMuons.size();
    LOG(DEBUG) << "Size m_pfPhotons: " << product.m_pfPhotons.size();
    LOG(DEBUG) << "Size m_pfHadronicHF: " << product.m_pfHadronicHF.size();
    LOG(DEBUG) << "Size m_pfElectromagneticHF: " << product.m_pfElectromagneticHF.size();
}


void PFCandidatesProducer::fill_pfCandidate(std::vector<const KPFCandidate*>& full, std::vector<const KPFCandidate*>& fromFirstPV, std::vector<const KPFCandidate*>& notFromFirstPV, const KPFCandidate* currentCandidate) const
{
	full.push_back(currentCandidate);
	if (currentCandidate->fromFirstPVFlag > 1) fromFirstPV.push_back(currentCandidate);
	else notFromFirstPV.push_back(currentCandidate);
}
