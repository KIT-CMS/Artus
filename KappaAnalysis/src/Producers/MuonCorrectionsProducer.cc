#include "Artus/KappaAnalysis/interface/Producers/MuonCorrectionsProducer.h"
#include "Artus/KappaAnalysis/interface/Utility/GeneratorInfo.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/Utility/interface/Utility.h"
#include "TLorentzVector.h"

MuonCorrectionsProducer::MuonEnergyCorrection MuonCorrectionsProducer::ToMuonEnergyCorrection(std::string const& muonEnergyCorrection)
{
	std::string muonEnergyCorrection_lower = boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(muonEnergyCorrection));
	if (muonEnergyCorrection_lower == "fall2015") return MuonCorrectionsProducer::MuonEnergyCorrection::FALL2015;
	else if (muonEnergyCorrection_lower == "rochcorr2015") return MuonCorrectionsProducer::MuonEnergyCorrection::ROCHCORR2015;
	else if (muonEnergyCorrection_lower == "rochcorr2016") return MuonCorrectionsProducer::MuonEnergyCorrection::ROCHCORR2016;
	else if (muonEnergyCorrection_lower == "rochcorr2016ul") return MuonCorrectionsProducer::MuonEnergyCorrection::ROCHCORR2016UL;
	else if (muonEnergyCorrection_lower == "rochcorr2017") return MuonCorrectionsProducer::MuonEnergyCorrection::ROCHCORR2017;
	else if (muonEnergyCorrection_lower == "rochcorr2017ul") return MuonCorrectionsProducer::MuonEnergyCorrection::ROCHCORR2017UL;
	else if (muonEnergyCorrection_lower == "rochcorr2018") return MuonCorrectionsProducer::MuonEnergyCorrection::ROCHCORR2018;
	else if (muonEnergyCorrection_lower == "rochcorr2018ul") return MuonCorrectionsProducer::MuonEnergyCorrection::ROCHCORR2018UL;
	else if (muonEnergyCorrection_lower == "none") return MuonCorrectionsProducer::MuonEnergyCorrection::NONE;
	else LOG(FATAL) << "No correct 'MuonEnergyCorrection' config entry was given!";
	return MuonCorrectionsProducer::MuonEnergyCorrection::NONE;
}

std::string MuonCorrectionsProducer::GetProducerId() const {
	return "MuonCorrectionsProducer";
}

void MuonCorrectionsProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);
	muonEnergyCorrection = ToMuonEnergyCorrection(settings.GetMuonEnergyCorrection());
	if (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2015) {
		rmcor2015 = std::make_unique<rochcor2015>(settings.GetMuonRochesterCorrectionsFile());
	}
	if ((muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2016) || (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2017) || (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2018) || (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2016UL) || (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2017UL) || (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2018UL))	{
		rmcor = std::make_unique<RoccoR>(settings.GetMuonRochesterCorrectionsFile());
	}
	random = std::make_unique<TRandom3>();
}

void MuonCorrectionsProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	assert(event.m_muons);
	LOG(DEBUG) << "\n[MuonCorrectionsProducer]";
	LOG(DEBUG) << "Using correction " << settings.GetMuonEnergyCorrection();
	LOG(DEBUG) << "Number of m_muons: " << event.m_muons->size() << "\n";

	// create a copy of all muons in the event
	product.m_correctedMuons.clear();
	product.m_correctedMuons.resize(event.m_muons->size());
	size_t muonIndex = 0;
	for (KMuons::const_iterator muon = event.m_muons->begin();
		 muon != event.m_muons->end(); ++muon) {
		product.m_correctedMuons[muonIndex] = std::shared_ptr<KMuon>(new KMuon(*muon));
		product.m_originalLeptons[product.m_correctedMuons[muonIndex].get()] = &(*muon);
		++muonIndex;
	}

	// perform corrections on copied muons
	for (std::vector<std::shared_ptr<KMuon> >::iterator muon = product.m_correctedMuons.begin();
		 muon != product.m_correctedMuons.end(); ++muon) {
		// Check whether corrections should be applied at all
		bool isRealMuon = false;
		if (settings.GetCorrectOnlyRealMuons()) {
			KappaEnumTypes::GenMatchingCode genMatchingCode = KappaEnumTypes::GenMatchingCode::NONE;
			if (settings.GetUseUWGenMatching()) {
				genMatchingCode = GeneratorInfo::GetGenMatchingCodeUW(event, const_cast<KLepton*>(product.m_originalLeptons[muon->get()]));
			} else {
				KGenParticle* genParticle = GeneratorInfo::GetGenMatchedParticle(const_cast<KLepton*>(product.m_originalLeptons[muon->get()]), 
						product.m_genParticleMatchedLeptons, product.m_genTauMatchedLeptons);
				if (genParticle) {
					genMatchingCode = GeneratorInfo::GetGenMatchingCode(genParticle);
				}
			}

			if (genMatchingCode == KappaEnumTypes::GenMatchingCode::IS_MUON_PROMPT ||
					genMatchingCode == KappaEnumTypes::GenMatchingCode::IS_MUON_FROM_TAU) {
				isRealMuon = true;
			}
		}

		// No general correction available

		// perform possible analysis-specific corrections
		if (muonEnergyCorrection == MuonEnergyCorrection::FALL2015) {
		muon->get()->p4 = muon->get()->p4 * (1.0);
		} else if (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2015) {
			TLorentzVector mu;
			mu.SetPtEtaPhiM(muon->get()->p4.Pt(),muon->get()->p4.Eta(),muon->get()->p4.Phi(),muon->get()->p4.mass());

			int q = muon->get()->charge();
			float qter = 1.0;

			if (settings.GetInputIsData()) {
				rmcor2015->momcor_data(mu, q, 0, qter);
				muon->get()->p4.SetPxPyPzE(mu.Px(),mu.Py(),mu.Pz(),mu.E());
			} else {
			int ntrk = muon->get()->track.nTrackerLayers();
				rmcor2015->momcor_mc(mu, q, ntrk, qter);
				muon->get()->p4.SetPxPyPzE(mu.Px(),mu.Py(),mu.Pz(),mu.E());
			}
		} else if ((muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2016) || (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2017) ||
			       	(muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2018) || (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2016UL) ||
			       	(muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2017UL) || (muonEnergyCorrection == MuonEnergyCorrection::ROCHCORR2018UL)) {
			int q = muon->get()->charge();
			float pt = muon->get()->p4.Pt();
			float eta = muon->get()->p4.Eta();
			float phi = muon->get()->p4.Phi();

			float scaleFactor = 1.0;
			LOG(DEBUG) << "Muon properties: q: " << q << " pt: " << pt << " eta: " << eta << " phi: " << phi; 
			if (settings.GetInputIsData()) {
				scaleFactor = rmcor->kScaleDT(q, pt, eta, phi);
				LOG(DEBUG) << "scaleFactor (kScaleDT used): " << scaleFactor;
			} else {
				int ntrk = muon->get()->track.nTrackerLayers();
				if (settings.GetRecoMuonMatchingGenParticleMatchAllMuons() &&
						&(*product.m_genParticleMatchedMuons[static_cast<KMuon*>(const_cast<KLepton*>(product.m_originalLeptons[muon->get()]))]) != nullptr) {
					KGenParticle* genMuon = &(*product.m_genParticleMatchedMuons[static_cast<KMuon*>(const_cast<KLepton*>(product.m_originalLeptons[muon->get()]))]);
					float genPt = genMuon->p4.Pt();
					scaleFactor = rmcor->kSpreadMC(q, pt, eta, phi, genPt);
					LOG(DEBUG) << "scaleFactor (kSpreadMC used), recommended: " << scaleFactor;
				} else {
					double u1 = random->Rndm();
					scaleFactor = rmcor->kSmearMC(q, pt, eta, phi, ntrk, u1);
					LOG(DEBUG) << "scaleFactor (kSmearMC used): " << scaleFactor;
				}
			}

			// scale only three dimensional momentum
			// -> need to manually calculate energy
			float muonMass = 0.105658;
			float scaledPx = muon->get()->p4.Px() * scaleFactor;
			float scaledPy = muon->get()->p4.Py() * scaleFactor;
			float scaledPz = muon->get()->p4.Pz() * scaleFactor;
			float scaledE = TMath::Sqrt(TMath::Power(scaledPx,2) + TMath::Power(scaledPy,2) + TMath::Power(scaledPz,2) + TMath::Power(muonMass,2));
			muon->get()->p4.SetPxPyPzE(scaledPx, scaledPy, scaledPz, scaledE);
            LOG(DEBUG) << "Corrected: " << "scaledPx: " << muon->get()->p4.Px() << " scaledPy: " << muon->get()->p4.Py() << " scaledPz: "
                       << muon->get()->p4.Pz() << " scaledE: " << muon->get()->p4.E() << " Pt: " << muon->get()->p4.Pt();

		} else if (muonEnergyCorrection == MuonEnergyCorrection::NONE) {
			LOG(DEBUG) << "Muon energy correction skipped.";
		} else {
			LOG(FATAL) << "Unknown muonEnergyCorrection and not none.";
		}

		if (!settings.GetCorrectOnlyRealMuons() || (settings.GetCorrectOnlyRealMuons() && isRealMuon)) {
			AdditionalCorrections(muon->get(), event, product, settings);
		}
			

		// make sure to also save the corrected lepton and the matched genParticle in the map
		// if we match genParticles to all leptons
		if (settings.GetRecoMuonMatchingGenParticleMatchAllMuons())
		{
			LOG(DEBUG) << "Updating MatchedLeptons.";
			product.m_genParticleMatchedMuons[muon->get()] =  &(*product.m_genParticleMatchedMuons[static_cast<KMuon*>(const_cast<KLepton*>(product.m_originalLeptons[muon->get()]))]);
			product.m_genParticleMatchedLeptons[muon->get()] = &(*product.m_genParticleMatchedLeptons[const_cast<KLepton*>(product.m_originalLeptons[muon->get()])]);
		}
		if (settings.GetMatchAllMuonsGenTau())
		{
			LOG(DEBUG) << "Updating genTauMatchedLeptons.";
			product.m_genTauMatchedMuons[muon->get()] = &(*product.m_genTauMatchedMuons[static_cast<KMuon*>(const_cast<KLepton*>(product.m_originalLeptons[muon->get()]))]);
			product.m_genTauMatchedLeptons[muon->get()] = &(*product.m_genTauMatchedLeptons[const_cast<KLepton*>(product.m_originalLeptons[muon->get()])]);
		}
	}

	// sort vectors of corrected muons by pt
	std::sort(product.m_correctedMuons.begin(), product.m_correctedMuons.end(),
	          [](std::shared_ptr<KMuon> muon1, std::shared_ptr<KMuon> muon2) -> bool
	          { return muon1->p4.Pt() > muon2->p4.Pt(); });
	if(settings.GetDebugVerbosity() > 0) {
		LOG(DEBUG) << "List of all corrected muons:";
		for(auto mu = product.m_correctedMuons.begin(); mu != product.m_correctedMuons.end(); mu++) {
			std::cout << "Pt: " << (*mu)->p4.Pt() << " eta: " << (*mu)->p4.Eta() << " phi: " << (*mu)->p4.Phi() << "\n";
		}
	}
}


// Can be overwritten for analysis-specific use cases
void MuonCorrectionsProducer::AdditionalCorrections(KMuon* muon, KappaEvent const& event,
                                   KappaProduct& product, KappaSettings const& settings) const
{
	// LOG(DEBUG) << "Additional Corrections applied! Please verify that this is correct ...\n";
}
