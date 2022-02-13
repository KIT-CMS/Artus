
#include "Artus/KappaAnalysis/interface/Producers/GenParticleProducer.h"
#include "Artus/Utility/interface/Utility.h"

std::string GenParticleProducer::GetProducerId() const{
	return "GenParticleProducer";
}

void GenParticleProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);
	
	m_genParticleTypes.clear();
	for (std::vector<std::string>::const_iterator genParticleType = settings.GetGenParticleTypes().begin();
	     genParticleType != settings.GetGenParticleTypes().end(); ++genParticleType)
	{
		m_genParticleTypes.push_back(KappaEnumTypes::ToGenParticleType(*genParticleType));
	}
}

void GenParticleProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	assert(event.m_genParticles);
	LOG(DEBUG) << "\n[GenParticleProducer]";
	LOG(DEBUG) << "GenParticleTypes:";
	for (auto type_it = settings.GetGenParticleTypes().begin(); type_it != settings.GetGenParticleTypes().end(); ++type_it) {
		LOG(DEBUG) << *type_it;
	}

	// gen particles (can be used for quarks, W, Z, .., but also for leptons if needed)
	if (Utility::Contains(m_genParticleTypes, KappaEnumTypes::GenParticleType::GENPARTICLE))
	{
		LOG(DEBUG) << "Looking for general particle with GenParticlePdgIds:";
		for (auto id_it = settings.GetGenParticlePdgIds().begin(); id_it != settings.GetGenParticlePdgIds().end(); ++id_it) {
			LOG(DEBUG) << *id_it;
		}
		LOG(DEBUG) << "GenParticleStatus: " << settings.GetGenParticleStatus(); 
		
		for (KGenParticles::iterator part = event.m_genParticles->begin();
		     part != event.m_genParticles->end(); ++part)
		{
			if (std::find(settings.GetGenParticlePdgIds().begin(), settings.GetGenParticlePdgIds().end(), part->pdgId)
			    != settings.GetGenParticlePdgIds().end())
			{
				if ((settings.GetGenParticleStatus() == -1) || ( settings.GetGenParticleStatus() == part->status()))
				{
					if (settings.GetDebugVerbosity() > 1) {
						LOG(DEBUG) << "Found genParticle with pdgId " << part->pdgId
							<< " and status " << part->status();
					}
					product.m_genParticlesMap[part->pdgId].push_back(&(*part));
				}
			}
		}
		if (settings.GetDebugVerbosity() > 0) {
			for (auto id_it = settings.GetGenParticlePdgIds().begin();
				id_it != settings.GetGenParticlePdgIds().end(); id_it++) {
				LOG(DEBUG) << "Number m_genParticlesMap[" << *id_it << "]: "
					<< product.m_genParticlesMap[*id_it].size();
			}
		}
	}

	// gen electrons
	if (Utility::Contains(m_genParticleTypes, KappaEnumTypes::GenParticleType::GENELECTRON))
	{
		LOG(DEBUG) << "Looking for genElectron with status " << settings.GetGenElectronStatus();
		LOG(DEBUG) << "GenElectronFromTauDecay: (0:No | 1:Yes): " << settings.GetGenElectronFromTauDecay();
		for (KGenParticles::iterator part = event.m_genParticles->begin();
		     part != event.m_genParticles->end(); ++part)
		{
			if (std::abs(part->pdgId) == 11)
			{
				if ((settings.GetGenElectronStatus() == -1) || ( settings.GetGenElectronStatus() == part->status()))
				{
					if (settings.GetGenElectronFromTauDecay())
					{
						if (part->isDirectPromptTauDecayProduct())
						{
							product.m_genElectrons.push_back(&(*part));
						}
					}
					else
					{
						product.m_genElectrons.push_back(&(*part));
					}
				}
			}
		}
		LOG(DEBUG) << "Number m_genElectrons: " << product.m_genElectrons.size();
	}

	// gen muons
	if (Utility::Contains(m_genParticleTypes, KappaEnumTypes::GenParticleType::GENMUON))
	{
		LOG(DEBUG) << "Looking for genMuons with status " << settings.GetGenMuonStatus();
		LOG(DEBUG) << "GenMuonFromTauDecay (0:No | 1:Yes): " << settings.GetGenMuonFromTauDecay();
		for (KGenParticles::iterator part = event.m_genParticles->begin();
		     part != event.m_genParticles->end(); ++part)
		{
			if (std::abs(part->pdgId) == 13)
			{
				if ((settings.GetGenMuonStatus() == -1) || ( settings.GetGenMuonStatus() == part->status()))
				{
					if (settings.GetGenMuonFromTauDecay())
					{
						if (part->isDirectPromptTauDecayProduct())
						{
							product.m_genMuons.push_back(&(*part));
						}
					}
					else
					{
						product.m_genMuons.push_back(&(*part));
					}
				}
			}
		}
		LOG(DEBUG) << "Number m_genMuons: " << product.m_genMuons.size();
	}

	// gen taus
	if (Utility::Contains(m_genParticleTypes, KappaEnumTypes::GenParticleType::GENTAU))
	{
		LOG(DEBUG) << "Looking for genTaus with status " << settings.GetGenTauStatus();
		for (KGenParticles::iterator part = event.m_genParticles->begin();
		     part != event.m_genParticles->end(); ++part)
		{
			if (std::abs(part->pdgId) == 15)
			{
				if ((settings.GetGenTauStatus() == -1) || ( settings.GetGenTauStatus() == part->status()))
				{
					product.m_genTaus.push_back(&(*part));
				}
			}
		}
		LOG(DEBUG) << "Number m_genTaus: " << product.m_genTaus.size();
	}
}

