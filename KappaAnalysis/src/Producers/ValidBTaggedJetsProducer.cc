
#include "Artus/KappaAnalysis/interface/Producers/ValidBTaggedJetsProducer.h"
#include "Artus/Utility/interface/SafeMap.h"


std::string ValidBTaggedJetsProducer::GetProducerId() const {
	return "ValidBTaggedJetsProducer";
}

void ValidBTaggedJetsProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);
	std::map<std::string, std::vector<float> > bTagWorkingPointsTmp = Utility::ParseMapTypes<std::string, float>(
			Utility::ParseVectorToMap(settings.GetBTaggerWorkingPoints())
	);

	BTagSF bTagSFBase(settings.GetBTagScaleFactorFile(), settings.GetBTagEfficiencyFile());
	for (std::pair<std::string, std::vector<float> > bTagWorkingPoint : bTagWorkingPointsTmp)
	{
		m_bTagWorkingPoints[bTagWorkingPoint.first] = bTagWorkingPoint.second.at(0);
		if (settings.GetApplyBTagSF() && !settings.GetInputIsData())
		{
			m_bTagSFMethod = KappaEnumTypes::ToBTagScaleFactorMethod(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetBTagSFMethod())));
			m_bTagSfMap[bTagWorkingPoint.first] = bTagSFBase;
			m_bTagSfMap[bTagWorkingPoint.first].initBtagwp(bTagWorkingPoint.first);
		}
		// define lambda expression for nbtag per working point
		std::string btagQuantity = std::string("n")+bTagWorkingPoint.first+std::string("btag");
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(btagQuantity, [bTagWorkingPoint](KappaEvent const& event, KappaProduct const& product) {
			auto it = product.m_bTaggedJetsByWp.find(bTagWorkingPoint.first);
			return it != product.m_bTaggedJetsByWp.end() ? product.m_bTaggedJetsByWp.at(bTagWorkingPoint.first).size() : 0;
		});
	}

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets20", [this](KappaEvent const& event, KappaProduct const& product) {
		return KappaProduct::GetNJetsAbovePtThreshold(product.m_bTaggedJets, 20.0);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets25", [this](KappaEvent const& event, KappaProduct const& product) {
                return KappaProduct::GetNJetsAbovePtThreshold(product.m_bTaggedJets, 25.0);
        });
        LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nlooseBJets25", [this](KappaEvent const& event, KappaProduct const& product) {
                return KappaProduct::GetNJetsAbovePtThreshold(product.m_loosebTaggedJets, 25.0);
        });
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets30", [this](KappaEvent const& event, KappaProduct const& product) {
		return KappaProduct::GetNJetsAbovePtThreshold(product.m_bTaggedJets, 30.0);
	});

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets30TruePassed", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return -1;
		int n(0);
		for (auto jet = product.m_bTaggedJets.begin(); jet != product.m_bTaggedJets.end(); ++jet)
		{
			KJet* tjet = static_cast<KJet*>(*jet);
			if (tjet->p4.pt() < 30 ) continue;
			if (tjet->hadronFlavour == 5) n++;
		}
		return n;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets30FalsePassed", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return -1;
		int n(0);
		for (auto jet = product.m_bTaggedJets.begin(); jet != product.m_bTaggedJets.end(); ++jet)
		{
			KJet* tjet = static_cast<KJet*>(*jet);
			if (tjet->p4.pt() < 30 ) continue;
			if (tjet->hadronFlavour != 5) n++;
		}
		return n;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets30TrueFailed", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return -1;
		int n(0);
		for (auto jet = product.m_nonBTaggedJets.begin(); jet != product.m_nonBTaggedJets.end(); ++jet)
		{
			KJet* tjet = static_cast<KJet*>(*jet);
			if (tjet->p4.pt() < 30 ) continue;
			if (tjet->hadronFlavour == 5) n++;
		}
		return n;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets30FalseFailed", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return -1;
		int n(0);
		for (auto jet = product.m_nonBTaggedJets.begin(); jet != product.m_nonBTaggedJets.end(); ++jet)
		{
			KJet* tjet = static_cast<KJet*>(*jet);
			if (tjet->p4.pt() < 30 ) continue;
			if (tjet->hadronFlavour != 5) n++;
		}
		return n;
	});

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets20TruePassed", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return -1;
		int n(0);
		for (auto jet = product.m_bTaggedJets.begin(); jet != product.m_bTaggedJets.end(); ++jet)
		{
			KJet* tjet = static_cast<KJet*>(*jet);
			if (tjet->p4.pt() < 20 ) continue;
			if (tjet->hadronFlavour == 5) n++;
		}
		return n;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets20FalsePassed", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return -1;
		int n(0);
		for (auto jet = product.m_bTaggedJets.begin(); jet != product.m_bTaggedJets.end(); ++jet)
		{
			KJet* tjet = static_cast<KJet*>(*jet);
			if (tjet->p4.pt() < 20 ) continue;
			if (tjet->hadronFlavour != 5) n++;
		}
		return n;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets20TrueFailed", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return -1;
		int n(0);
		for (auto jet = product.m_nonBTaggedJets.begin(); jet != product.m_nonBTaggedJets.end(); ++jet)
		{
			KJet* tjet = static_cast<KJet*>(*jet);
			if (tjet->p4.pt() < 20 ) continue;
			if (tjet->hadronFlavour == 5) n++;
		}
		return n;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets20FalseFailed", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return -1;
		int n(0);
		for (auto jet = product.m_nonBTaggedJets.begin(); jet != product.m_nonBTaggedJets.end(); ++jet)
		{
			KJet* tjet = static_cast<KJet*>(*jet);
			if (tjet->p4.pt() < 20 ) continue;
			if (tjet->hadronFlavour != 5) n++;
		}
		return n;
	});

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJetsTruePassed", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return -1;
		int n(0);
		for (auto jet = product.m_bTaggedJets.begin(); jet != product.m_bTaggedJets.end(); ++jet)
		{
			KJet* tjet = static_cast<KJet*>(*jet);
			if (tjet->hadronFlavour == 5) n++;
		}
		return n;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJetsFalsePassed", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return -1;
		int n(0);
		for (auto jet = product.m_bTaggedJets.begin(); jet != product.m_bTaggedJets.end(); ++jet)
		{
			KJet* tjet = static_cast<KJet*>(*jet);
			if (tjet->hadronFlavour != 5) n++;
		}
		return n;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJetsTrueFailed", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return -1;
		int n(0);
		for (auto jet = product.m_nonBTaggedJets.begin(); jet != product.m_nonBTaggedJets.end(); ++jet)
		{
			KJet* tjet = static_cast<KJet*>(*jet);
			if (tjet->hadronFlavour == 5) n++;
		}
		return n;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJetsFalseFailed", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return -1;
		int n(0);
		for (auto jet = product.m_nonBTaggedJets.begin(); jet != product.m_nonBTaggedJets.end(); ++jet)
		{
			KJet* tjet = static_cast<KJet*>(*jet);
			if (tjet->hadronFlavour != 5) n++;
		}
		return n;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJetPt", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJetMass", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.M() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet_bRegCorr", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->bjetRegCorr : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet_bRegRes", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->bjetRegRes : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJetEta", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJetPt_bReg", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 1 ? (product.m_bTaggedJets.at(0)->p4*product.m_bTaggedJets.at(0)->bjetRegCorr).Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJetMass_bReg", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 1 ? (product.m_bTaggedJets.at(0)->p4*product.m_bTaggedJets.at(0)->bjetRegCorr).M() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJetEta_bReg", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 1 ? (product.m_bTaggedJets.at(0)->p4*product.m_bTaggedJets.at(0)->bjetRegCorr).Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJetPhi_bReg", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 1 ? (product.m_bTaggedJets.at(0)->p4*product.m_bTaggedJets.at(0)->bjetRegCorr).Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("bJetIsTrueBJet", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return DefaultValues::UndefinedInt;
		if (product.m_bTaggedJets.size() >= 1) {
			if (product.m_bTaggedJets.at(0)->hadronFlavour == 5) {
				return 1;
			}
			else {
				return 0;
			}
		} 
		else return DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2Pt", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 2 ? product.m_bTaggedJets.at(1)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2Mass", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 2 ? product.m_bTaggedJets.at(1)->p4.M() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2_bRegCorr", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 2 ? product.m_bTaggedJets.at(1)->bjetRegCorr : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2_bRegRes", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 2 ? product.m_bTaggedJets.at(1)->bjetRegRes : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2Eta", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 2 ? product.m_bTaggedJets.at(1)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2Phi", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 2 ? product.m_bTaggedJets.at(1)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2Pt_bReg", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 2 ? (product.m_bTaggedJets.at(1)->p4*product.m_bTaggedJets.at(1)->bjetRegCorr).Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2Mass_bReg", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 2 ? (product.m_bTaggedJets.at(1)->p4*product.m_bTaggedJets.at(1)->bjetRegCorr).M() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2Eta_bReg", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 2 ? (product.m_bTaggedJets.at(1)->p4*product.m_bTaggedJets.at(1)->bjetRegCorr).Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2Phi_bReg", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 2 ? (product.m_bTaggedJets.at(1)->p4*product.m_bTaggedJets.at(1)->bjetRegCorr).Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("bJet2IsTrueBJet", [settings](KappaEvent const& event, KappaProduct const& product) {
		if (settings.GetInputIsData()) return DefaultValues::UndefinedInt;
		if (product.m_bTaggedJets.size() >= 2) {
			if (product.m_bTaggedJets.at(1)->hadronFlavour == 5) {
				return 1;
			}
			else {
				return 0;
			}
		} 
		else return DefaultValues::UndefinedInt;
	});

	std::string bTaggedJetCSVName = settings.GetBTaggedJetCombinedSecondaryVertexName();
	std::string jetPuJetIDName = settings.GetPuJetIDFullDiscrName();

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingBJetCSV",[bTaggedJetCSVName](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 1 ? static_cast<KJet*>(product.m_bTaggedJets.at(0))->getTag(bTaggedJetCSVName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingBJetPuID",[jetPuJetIDName](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 1 ? static_cast<KJet*>(product.m_bTaggedJets.at(0))->getTag(jetPuJetIDName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingBJetCSV",[bTaggedJetCSVName](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 2 ? static_cast<KJet*>(product.m_bTaggedJets.at(1))->getTag(bTaggedJetCSVName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingBJetPuID",[jetPuJetIDName](KappaEvent const& event, KappaProduct const& product) {
		return product.m_bTaggedJets.size() >= 2 ? static_cast<KJet*>(product.m_bTaggedJets.at(1))->getTag(jetPuJetIDName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	});
}

void ValidBTaggedJetsProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                       KappaSettings const& settings) const
{
	assert(event.m_jetMetadata);
	assert(settings.GetBTagWPs().size() > 0);

        auto btagwps = settings.GetBTagWPs();
        btagwps.push_back("loose");
	for (std::vector<std::string>::const_iterator workingPoint = btagwps.begin();
	     workingPoint != btagwps.end(); ++workingPoint)
	{
		for (std::vector<KBasicJet*>::iterator jet = product.m_validJets.begin(); jet != product.m_validJets.end(); ++jet)
		{
			bool validBJet = true;
			KJet* tjet = static_cast<KJet*>(*jet);

			float combinedSecondaryVertex = tjet->getTag(settings.GetBTaggedJetCombinedSecondaryVertexName(), event.m_jetMetadata);
			float bTagWorkingPoint = SafeMap::Get(m_bTagWorkingPoints, *workingPoint);

			if (combinedSecondaryVertex < bTagWorkingPoint ||
				std::abs(tjet->p4.eta()) > settings.GetBTaggedJetAbsEtaCut()) {
				validBJet = false;
			}

			validBJet = validBJet && AdditionalCriteria(tjet, event, product, settings);

			//entry point for Scale Factor (SF) of btagged jets
			if (settings.GetApplyBTagSF() && !settings.GetInputIsData())
			{
				//https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods#2a_Jet_by_jet_updating_of_the_b
				if (m_bTagSFMethod == KappaEnumTypes::BTagScaleFactorMethod::PROMOTIONDEMOTION) {

					int jetflavor = tjet->hadronFlavour;
					unsigned int btagSys = BTagSF::kNo;
					unsigned int bmistagSys = BTagSF::kNo;

					if (settings.GetBTagShift()<0)
						btagSys = BTagSF::kDown;
					if (settings.GetBTagShift()>0)
						btagSys = BTagSF::kUp;
					if (settings.GetBMistagShift()<0)
						bmistagSys = BTagSF::kDown;
					if (settings.GetBMistagShift()>0)
						bmistagSys = BTagSF::kUp;

					LOG_N_TIMES(1, DEBUG) << "Btagging shifts tag/mistag : " << settings.GetBTagShift() << " " << settings.GetBMistagShift();

					bool taggedBefore = validBJet;
					validBJet = m_bTagSfMap.at(*workingPoint).isbtagged(
							tjet->p4.pt(),
							tjet->p4.eta(),
							tjet->p4.phi(),
							combinedSecondaryVertex,
							jetflavor,
							btagSys,
							bmistagSys,
							settings.GetYear(),
							bTagWorkingPoint
					);

					if (taggedBefore != validBJet)
						LOG_N_TIMES(20, DEBUG) << "Promoted/demoted : " << validBJet;
				}

				else if (m_bTagSFMethod == KappaEnumTypes::BTagScaleFactorMethod::OTHER) {
					//todo
				}
			}

			if (validBJet)
				product.m_bTaggedJetsByWp[*workingPoint].push_back(tjet);
			else
				product.m_nonBTaggedJetsByWp[*workingPoint].push_back(tjet);
		}
	}
	product.m_bTaggedJets = product.m_bTaggedJetsByWp[settings.GetBTagWPs().at(0)];
        product.m_loosebTaggedJets = product.m_bTaggedJetsByWp[btagwps.at(1)];
	product.m_nonBTaggedJets = product.m_nonBTaggedJetsByWp[settings.GetBTagWPs().at(0)];
}

bool ValidBTaggedJetsProducer::AdditionalCriteria(KJet* jet, KappaEvent const& event,
                                KappaProduct& product, KappaSettings const& settings) const
{
	bool validBJet = true;
	return validBJet;
}

