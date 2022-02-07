/* ValidJetsProducer
 * Last update:
 * refactored passesJetID function and updated cuts according to:
 * https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13TeVUL
 * current status: July 2021
 */

#pragma once

#include <algorithm>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/Utility/interface/Utility.h"
#include "Artus/KappaAnalysis/interface/KappaProduct.h"

/**
   \brief Producer for valid jets.

   Valid jets pass the particle flow JetID ("tight", "medium" or "loose") by JetMET. There are no
   pileupJetID requirements applied at the moment.

   This producer requires the "JetID" config tag to be set to "tight", "medium" or "loose".

   This producer should be run after the ValidElectronsProducer, ValidMuonsProducer and ValidTausProducer,
   because it cleans the list of jets according to the valid leptons.

   This is a templated base version. Use the actual versions for KBasicJets or KJets
   at the end of this file.
*/


template<class TJet, class TValidJet>
class ValidJetsProducerBase: public KappaProducerBase, public ValidPhysicsObjectTools<KappaTypes, TValidJet>
{

public:

	ValidJetsProducerBase(std::vector<TJet>* KappaEvent::*jets,
	                      std::vector<std::shared_ptr<TJet> > KappaProduct::*correctJets,
	                      std::vector<TValidJet*> KappaProduct::*validJets) :
		KappaProducerBase(),
		ValidPhysicsObjectTools<KappaTypes, TValidJet>(&KappaSettings::GetJetLowerPtCuts,
		                                               &KappaSettings::GetJetUpperAbsEtaCuts,
		                                               validJets),
		m_basicJetsMember(jets),
		m_correctedJetsMember(correctJets)
	{
	}

	void Init(KappaSettings const& settings) override
	{
		KappaProducerBase::Init(settings);
		ValidPhysicsObjectTools<KappaTypes, TValidJet>::Init(settings);

		validJetsInput = KappaEnumTypes::ToValidJetsInput(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetValidJetsInput())));
		jetIDVersion = KappaEnumTypes::ToJetIDVersion(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetJetIDVersion())));
		jetID = KappaEnumTypes::ToJetID(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetJetID())));

		if (jetID == KappaEnumTypes::JetID::MEDIUM && jetIDVersion != KappaEnumTypes::JetIDVersion::ID2010)
			LOG(WARNING) << "Since 2012, the medium jet ID is not supported officially any longer.";
		if (jetID != KappaEnumTypes::JetID::NONE && (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2010 || jetIDVersion == KappaEnumTypes::JetIDVersion::ID2014))
			LOG(WARNING) << "This jet ID version is not valid for 73X samples.";

		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size();
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets20",[this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 20.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets30",[this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 30.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets50",[this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 50.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets80",[this](KappaEvent const& event, KappaProduct const& product) {
			return KappaProduct::GetNJetsAbovePtThreshold(product.m_validJets, 80.0);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nJets20Eta2p4",[this](KappaEvent const& event, KappaProduct const& product) {
            std::vector<TValidJet*> filteredJets;
			for (typename std::vector<TValidJet*>::const_iterator jet = (product.m_validJets).begin();
				 jet != (product.m_validJets).end(); ++jet)
			{
				if ((*jet)->p4.Eta() < 2.4) filteredJets.push_back(new TValidJet(*(*jet)));
			}
			return KappaProduct::GetNJetsAbovePtThreshold(filteredJets, 20.0);
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("leadingJetLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 1 ? product.m_validJets.at(0)->p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("trailingJetLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 2 ? product.m_validJets.at(1)->p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("thirdJetLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdJetMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 3 ? product.m_validJets.at(2)->p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("fourthJetLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthJetMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 4 ? product.m_validJets.at(3)->p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("fifthJetLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fifthJetMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 5 ? product.m_validJets.at(4)->p4.mass() : DefaultValues::UndefinedFloat;
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("sixthJetLV", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4 : DefaultValues::UndefinedRMFLV;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthJetPt", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4.Pt() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthJetEta", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4.Eta() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthJetPhi", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4.Phi() : DefaultValues::UndefinedFloat;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("sixthJetMass", [](KappaEvent const& event, KappaProduct const& product) {
			return product.m_validJets.size() >= 6 ? product.m_validJets.at(5)->p4.mass() : DefaultValues::UndefinedFloat;
		});
	}

	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override
	{
		assert((event.*m_basicJetsMember));
		LOG(DEBUG) << "\n[ValidTaggedJetsProducer] or [ValidJetsProducer]\n";
		// select input source
		std::vector<TJet*> jets;
		if ((validJetsInput == KappaEnumTypes::ValidJetsInput::AUTO && ((product.*m_correctedJetsMember).size() > 0)) ||
			(validJetsInput == KappaEnumTypes::ValidJetsInput::CORRECTED)) {
			LOG(DEBUG) << "Use correctedJets as input.";
			jets.resize((product.*m_correctedJetsMember).size());
			LOG(DEBUG) << "N jets: " << (product.*m_correctedJetsMember).size();
			size_t jetIndex = 0;
			for (typename std::vector<std::shared_ptr<TJet> >::iterator jet = (product.*m_correctedJetsMember).begin();
					jet != (product.*m_correctedJetsMember).end(); ++jet) {
				jets[jetIndex] = jet->get();
				++jetIndex;
			}
		} else {
			LOG(DEBUG) << "Use basic jets as input.";
			LOG(DEBUG) << "N jets: " << (event.*m_basicJetsMember)->size();
			jets.resize((event.*m_basicJetsMember)->size());
			size_t jetIndex = 0;
			for (typename std::vector<TJet>::iterator jet = (event.*m_basicJetsMember)->begin();
                        		jet != (event.*m_basicJetsMember)->end(); ++jet) {
				jets[jetIndex] = &(*jet);
				++jetIndex;
			}
		}
		
		if (product.m_validLeptons.size() < 1 ) {
			LOG(WARNING) << "Number of valid leptons:" << product.m_validLeptons.size()
                         << "! Forgot to add the ValidLeptonsProducer?";
		} else {
			LOG(DEBUG) << "Number of valid leptons used for Lepton removal: " << product.m_validLeptons.size();
		}

		if (settings.GetDebugVerbosity() > 0 && jetID == KappaEnumTypes::JetID::NONE) {
			LOG(WARNING) << "Object-based JetID skipped!";
		}

		for (typename std::vector<TJet*>::iterator jet = jets.begin(); jet != jets.end(); ++jet) {
			bool validJet = true;
			validJet = validJet && passesJetID(*jet, jetIDVersion, jetID, settings);
			if( settings.GetDebugVerbosity() > 0) {
				LOG(DEBUG) << "\nJet pt: " << (*jet)->p4.Pt() << " jet eta: " << (*jet)->p4.eta() << " jet phi: " << (*jet)->p4.phi();
				if (passesJetID(*jet, jetIDVersion, jetID, settings)) {
					LOG(DEBUG) << "Object-based JetID passed (or skipped).";
				} else {
					LOG(DEBUG) << "Jet does not pass JetID.";
				}
			}

			// remove valid leptons from list of jets via simple DeltaR isolation
			for (std::vector<KLepton*>::const_iterator lepton = product.m_validLeptons.begin();
				     validJet && lepton != product.m_validLeptons.end(); ++lepton) {
				validJet = validJet && ROOT::Math::VectorUtil::DeltaR((*jet)->p4, (*lepton)->p4) > settings.GetJetLeptonLowerDeltaRCut();
				if (settings.GetDebugVerbosity() > 1) {
					LOG(DEBUG) << "Check lepton with pt: " << (*lepton)->p4.Pt() << "eta: " << (*lepton)->p4.Pt() << " phi: " 
						<< (*lepton)->p4.Pt() << " Delta R: " << ROOT::Math::VectorUtil::DeltaR((*jet)->p4, (*lepton)->p4);
					if (validJet == false) {
					LOG(DEBUG) << "Jet invalidated and removed due to delta R cut (" << settings.GetJetLeptonLowerDeltaRCut() << ")";
					}
				}
			}

			// kinematic cuts
			validJet = validJet && this->PassKinematicCuts(*jet, event, product);
			if (this->PassKinematicCuts(*jet, event, product) == false) {
				LOG(WARNING) << "Jet does not pass (OLD) KinematicCuts. Please verify!!\n";
			}	
			// check possible analysis-specific criteria
			validJet = validJet && AdditionalCriteria(*jet, event, product, settings);
			if (AdditionalCriteria(*jet, event, product, settings) == false) {
				LOG(DEBUG) << "Jet does not pass AdditionalCriteria. Please verify!\n";
			}
			if (validJet) {
				product.m_validJets.push_back(*jet);
			} else {
				product.m_invalidJets.push_back(*jet);
			}
		}
		LOG(DEBUG) << "\n===== Jet Summary: =====";
		if(settings.GetDebugVerbosity() > 0) {
			LOG(DEBUG) << "Valid Jets:";
			for (auto jet = product.m_validJets.begin(); jet != product.m_validJets.end(); ++jet) {
				std::cout << (*jet)->p4.Pt() << " eta: " << (*jet)->p4.eta() << " phi: " << (*jet)->p4.phi() << std::endl;
			}
		
			LOG(DEBUG) << "\nInvalid Jets:";
			for (auto jet = product.m_invalidJets.begin(); jet != product.m_invalidJets.end(); ++jet) {
				std::cout << (*jet)->p4.Pt() << " eta: " << (*jet)->p4.eta() << " phi: " << (*jet)->p4.phi() << std::endl;
			}
		}
		LOG(DEBUG) << "\nn ValidJets: " << product.m_validJets.size() << ", n invalidJets: " << product.m_invalidJets.size();
	}

	// check on jet type added and cut logic refactored
	static bool passesJetID(TJet* jet, KappaEnumTypes::JetIDVersion jetIDVersion,
                            KappaEnumTypes::JetID jetID, KappaSettings const& settings)
	{
		// if applying jet ID is not requested, return immediately
		if (jetID == KappaEnumTypes::JetID::NONE) {
			return true;
		}

	// -- stage 1: check for TaggedJets
            // -- stage 2: check for jetIDVersion
                // -- stage 3: different eta ranges
                    // -- stage 4: check for JetID (deprecated for UL)

        if (settings.GetTaggedJets().find("CHS") != std::string::npos) { // cuts for CHS
            // 2016: https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2016
            if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2016) {
                if (std::abs(jet->p4.eta()) <= 2.7f) {
                    if (jetID == KappaEnumTypes::JetID::TIGHTLEPVETO) {  // recommended!
                        float maxNHFraction = 0.90f;
                        float maxNEMFraction = 0.90f;
                        float minNumConstituents = 1;  // has to be greater than 1
                        float maxMuFraction = 0.8f;  // only for TIGHTLEPVETO
                        if (std::abs(jet->p4.eta()) <= 2.4f) {  // additional for <= 2.4
                            float minCHFraction = 0; // has to be greater than 0
                            float minChargedMult = 0; // has to be greater than 0
                            float maxCEMFraction = 0.90f;

                            // check cuts: return false, if jet does not survive the cuts
                            if (!(jet->neutralHadronFraction < maxNHFraction &&
                                (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                                jet->nConstituents > minNumConstituents &&
                                jet->muonFraction < maxMuFraction &&
                                jet->chargedHadronFraction > minCHFraction &&
                                jet->nCharged > minChargedMult &&
                                jet->electronFraction < maxCEMFraction)) {
                            	   return false;
			    }
                        } else {
                            if (!(jet->neutralHadronFraction < maxNHFraction &&
                                (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                                jet->nConstituents > minNumConstituents &&
                                jet->muonFraction < maxMuFraction)) {
                             	   return false;
			    }
                        }
                    } else if (jetID == KappaEnumTypes::JetID::TIGHT) {
                        float maxNHFraction = 0.90f;
                        float maxNEMFraction = 0.90f;
                        float minNumConstituents = 1;
                        if (std::abs(jet->p4.eta()) <= 2.4f) {
                            float minCHFraction = 0;
                            float minChargedMult = 0;
                            float maxCEMFraction = 0.99f;

                            if (!(jet->neutralHadronFraction < maxNHFraction &&
                                (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                                jet->nConstituents > minNumConstituents &&
                                jet->chargedHadronFraction > minCHFraction &&
                                jet->nCharged > minChargedMult &&
                                jet->electronFraction < maxCEMFraction)) {
                                    return false;
			    }
                        } else {
                            if (!(jet->neutralHadronFraction < maxNHFraction &&
                                (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                                jet->nConstituents > minNumConstituents)) {
                                    return false;
			    }
                        }
                    } else if (jetID == KappaEnumTypes::JetID::LOOSE) {
                        float maxNHFraction = 0.99f;
                        float maxNEMFraction = 0.99f;
                        float minNumConstituents = 1;

                        if (std::abs(jet->p4.eta()) <= 2.4f) {
                            float minCHFraction = 0;
                            float minChargedMult = 0;
                            float maxCEMFraction = 0.99f;

                            if (!(jet->neutralHadronFraction < maxNHFraction &&
                                (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                                jet->nConstituents > minNumConstituents &&
                                jet->chargedHadronFraction > minCHFraction &&
                                jet->nCharged > minChargedMult &&
                                jet->electronFraction < maxCEMFraction)) {
                            	    return false;
			    }
                        } else { // 2.4 <= 2.7
                            if (!(jet->neutralHadronFraction < maxNHFraction &&
                                (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                                jet->nConstituents > minNumConstituents)) {
                                    return false;
			    }
                        }
                    } else {
                        // non-official: if this is still necessary, pls check the pull request from 19.07.2021 /TODO
                        if (jetID == KappaEnumTypes::JetID::MEDIUM || jetID == KappaEnumTypes::JetID::LOOSELEPVETO) {
                            LOG(FATAL) << "+++ JetID = MEDIUM or LOOSELEPVETO not implemented +++" << std::endl;
			}

                        LOG(FATAL) << "+++ Unknown JetID +++ please check your configuration!" << std::endl;
                    }
                } else if (std::abs(jet->p4.eta()) > 2.7f && std::abs(jet->p4.eta()) <= 3.0f) {
                    if (jetID == KappaEnumTypes::JetID::TIGHT){
                        float maxNHFraction = 0.98f;
                        float minNEMFraction = 0.01f;
                        float minNeutral = 2; // Number of neutral particles

                        if (!(jet->neutralHadronFraction < maxNHFraction &&
                              (jet->photonFraction + jet->hfEMFraction) > minNEMFraction &&
                              (jet->nConstituents - jet->nCharged) > minNeutral)) { // NumNeutralParticles = pfjet->neutralMultiplicity()
                            // not implemented in Kappa
                                return false;
			}
                    } else if (jetID == KappaEnumTypes::JetID::LOOSE) {
                        float maxNHFraction = 0.98f;
                        float minNEMFraction = 0.01f;
                        float minNeutral = 2;

                        if (!(jet->neutralHadronFraction < maxNHFraction &&
                              (jet->photonFraction + jet->hfEMFraction) > minNEMFraction &&
                              (jet->nConstituents - jet->nCharged) > minNeutral)) {
                            // not implemented in Kappa
                        	return false;
			}
                    } else {
                        LOG(FATAL) << "+++ Unknown JetID or no valid cuts available for this eta region! +++" << std::endl;
                    }
                } else if (std::abs(jet->p4.eta()) > 3.0f) {
                    if (jetID == KappaEnumTypes::JetID::TIGHT){
                        float maxNEMFraction = 0.90f;
                        float minNeutral = 10;

                        if (!((jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                            (jet->nConstituents - jet->nCharged) > minNeutral)) {
	                	return false;
			}
                    } else if (jetID == KappaEnumTypes::JetID::LOOSE) {
                        float maxNEMFraction = 0.90f;
                        float minNeutral = 10;

                        if (!((jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                            (jet->nConstituents - jet->nCharged) > minNeutral)) {
                        	return false;
			}
                    } else {
                        LOG(FATAL) << "+++ No valid cuts available for this eta region! +++" << std::endl;
                    }

                }
            } 
            // 2017: https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13TeVRun2017
            else if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2017) {
                if (std::abs(jet->p4.eta()) <= 2.7f) {
                    if (jetID == KappaEnumTypes::JetID::TIGHTLEPVETO) {
                        // recommended!
                        float maxNHFraction = 0.90f;
                        float maxNEMFraction = 0.90f;
                        float minNumConstituents = 1;
                        float maxMuFraction = 0.8f;
                        if (std::abs(jet->p4.eta()) <= 2.4f) {
                            // additional for <= 2.4
                            float minCHFraction = 0;
                            float minChargedMult = 0;
                            float maxCEMFraction = 0.80f;

                            if (!(jet->neutralHadronFraction < maxNHFraction &&
                                (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                                jet->nConstituents > minNumConstituents &&
                                jet->muonFraction < maxMuFraction &&
                                jet->chargedHadronFraction > minCHFraction &&
                                jet->nCharged > minChargedMult &&
                                jet->electronFraction < maxCEMFraction)) {
                            	    return false;
			    }
                        } else {
                            if (!(jet->neutralHadronFraction < maxNHFraction &&
                                (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                                jet->nConstituents > minNumConstituents &&
                                jet->muonFraction < maxMuFraction)) {
	    	                    return false;
			    }
                        }
                    } else if (jetID == KappaEnumTypes::JetID::TIGHT) {
                        float maxNHFraction = 0.90f;
                        float maxNEMFraction = 0.90f;
                        float minNumConstituents = 1;
                        if (std::abs(jet->p4.eta()) <= 2.4f) {
                            float minCHFraction = 0;
                            float minChargedMult = 0;

                            if (!(jet->neutralHadronFraction < maxNHFraction &&
                                (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                                jet->nConstituents > minNumConstituents &&
                                jet->chargedHadronFraction > minCHFraction &&
                                jet->nCharged > minChargedMult)) {
                                    return false;
			    }
                        } else {  // 2.4 <= eta <= 2.7
                            if (!(jet->neutralHadronFraction < maxNHFraction &&
                                (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                                jet->nConstituents > minNumConstituents)) {
                                    return false;
			    }
                        }
                    } else if (jetID == KappaEnumTypes::JetID::LOOSE ) {
                        LOG(WARNING) << "+++ JetID = LOOSE not recommended anymore! +++" << std::endl;
                    } else {
                        LOG(FATAL) << "+++ Unknown JetID +++ please check your configuration!" << std::endl;
                    }
                } else if (std::abs(jet->p4.eta()) > 2.7f && std::abs(jet->p4.eta()) <= 3.0f) {
                    if (jetID == KappaEnumTypes::JetID::TIGHT) {
                        float minNEMFraction = 0.02f;
                        float maxNEMFraction = 0.99f;
                        float minNeutral = 2;

                        if (!((jet->photonFraction + jet->hfEMFraction) > minNEMFraction &&
                            (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                            (jet->nConstituents - jet->nCharged) > minNeutral)) {
                                return false;
			}
                    } else if (jetID == KappaEnumTypes::JetID::LOOSE) {
                        LOG(WARNING) << " JetID = LOOSE not recommended anymore!" << std::endl;
                    } else {
                        LOG(FATAL) << "+++ Unknown JetID or no valid cuts available for this eta region! +++" << std::endl;
                    }
                } else if (std::abs(jet->p4.eta()) > 3.0f) {
                    if (jetID == KappaEnumTypes::JetID::TIGHT) {
                        float maxNEMFraction = 0.90f;
                        float minNHFraction = 0.02;
                        float minNeutral = 10;

                        if (!((jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                            jet->neutralHadronFraction > minNHFraction &&
                            (jet->nConstituents - jet->nCharged) > minNeutral)) {
                                return false;
			}
                    } else if (jetID == KappaEnumTypes::JetID::LOOSE) {
                        LOG(WARNING) << " JetID = LOOSE not recommended anymore!" << std::endl;
                    } else {
                        LOG(FATAL) << "+++ No valid cuts available for this eta region! +++" << std::endl;
                    }
                }
            }
            // 2018: https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2018
            // Note: there is no "loose" JetID anymore for 2018; standard is "tight"
            else if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2018) {
                if (std::abs(jet->p4.eta()) <= 2.6f) {
                    float maxNHFraction = 0.90f;
                    float maxNEMFraction = 0.90f;
                    float minNumConstituents = 1;
                    float maxMuFraction = 0.8f;  // for LepVeto (see twiki)
                    float minCHFraction = 0;
                    float minChargedMult = 0;
                    float maxCEMFraction = 0.80f;
                    if (!(jet->neutralHadronFraction < maxNHFraction &&
                         (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                         jet->nConstituents > minNumConstituents &&
                         jet->muonFraction < maxMuFraction &&
                         jet->chargedHadronFraction > minCHFraction &&
                         jet->nCharged > minChargedMult &&
                         jet->electronFraction < maxCEMFraction)) {
                             return false;
		    }
                } else if (std::abs(jet->p4.eta()) > 2.6f && std::abs(jet->p4.eta()) <= 2.7f) {
                    float maxNHFraction = 0.90f;
                    float maxNEMFraction = 0.99f;
                    float maxMuFraction = 0.8f;  // for LepVeto (see twiki)
                    float minChargedMult = 0;
                    float maxCEMFraction = 0.80f;
                    if (!(jet->neutralHadronFraction < maxNHFraction &&
                         (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                         jet->muonFraction < maxMuFraction &&
                         jet->nCharged > minChargedMult &&
                         jet->electronFraction < maxCEMFraction)) {
                             return false;
		    }
                } else if (std::abs(jet->p4.eta()) > 2.7f && std::abs(jet->p4.eta()) <= 3.0f) {
                    float minNEMFraction = 0.02f;
                    float maxNEMFraction = 0.99f;
                    float minNeutral = 2;
                    if (!((jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                         (jet->photonFraction + jet->hfEMFraction) > minNEMFraction &&
                         (jet->nConstituents - jet->nCharged) > minNeutral)) {
                             return false;
		    }
                } else if (std::abs(jet->p4.eta()) > 3.0f && std::abs(jet->p4.eta()) <= 5.0f) {
                    float minNHFraction = 0.20f;
                    float maxNEMFraction = 0.90f;
                    float minNeutral = 10;
                    if (!(jet->neutralHadronFraction > minNHFraction &&
                        (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                        (jet->nConstituents - jet->nCharged) > minNeutral)) {
                            return false;
		    }
                }
            }
            // Ultra Legacy: https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVUL
            else if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2016UL) {
                if (std::abs(jet->p4.eta()) <= 2.4f) {
                    float maxNHFraction = 0.90f;
                    float maxNEMFraction = 0.90f;
                    float minNumConstituents = 1;
                    float maxMuFraction = 0.8f;
                    float minCHFraction = 0;
                    float minChargedMult = 0;
                    float maxCEMFraction = 0.80f;
                    if (!(jet->neutralHadronFraction < maxNHFraction &&
                        (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                        jet->nConstituents > minNumConstituents &&
                        jet->muonFraction < maxMuFraction &&
                        jet->chargedHadronFraction > minCHFraction &&
                        jet->nCharged > minChargedMult &&
                        jet->electronFraction < maxCEMFraction)) {
                            return false;
		    }
                } else if (std::abs(jet->p4.eta()) < 2.4f && std::abs(jet->p4.eta()) <= 2.7f) {
                    float maxNHFraction = 0.90f;
                    float maxNEMFraction = 0.99f;
                    if (!(jet->neutralHadronFraction < maxNHFraction &&
                        (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction)) {
                             return false;
		    }
                } else if (std::abs(jet->p4.eta()) < 2.7f && std::abs(jet->p4.eta()) <= 3.0f) {
                    float maxNHFraction = 0.90f;
                    float minNEMFraction = 0.0f;
                    float maxNEMFraction = 0.99f;
                    float minNeutral = 1;
                    if (!(jet->neutralHadronFraction < maxNHFraction &&
                        (jet->photonFraction + jet->hfEMFraction) > minNEMFraction &&
                        (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                        (jet->nConstituents - jet->nCharged) > minNeutral )) {
                             return false;
		    }
                } else if (std::abs(jet->p4.eta()) < 3.0f && std::abs(jet->p4.eta()) <= 5.0f) {
                    float minNHFraction = 0.20f;
                    float maxNEMFraction = 0.90f;
                    float minNeutral = 10;
                    if (!(jet->neutralHadronFraction > minNHFraction &&
                        (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                        (jet->nConstituents - jet->nCharged) > minNeutral)) {
                            return false;
		    }
                }
            } else if (jetIDVersion == KappaEnumTypes::JetIDVersion::ID2017UL || jetIDVersion == KappaEnumTypes::JetIDVersion::ID2018UL) {
                if (std::abs(jet->p4.eta()) <= 2.6f) {
                    float maxNHFraction = 0.90f;
                    float maxNEMFraction = 0.90f;
                    float minNumConstituents = 1;
                    float maxMuFraction = 0.8f;
                    float minCHFraction = 0;
                    float minChargedMult = 0;
                    float maxCEMFraction = 0.80f;

                    if (!(jet->neutralHadronFraction < maxNHFraction &&
                        (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                        jet->nConstituents > minNumConstituents &&
                        jet->muonFraction < maxMuFraction &&
                        jet->chargedHadronFraction > minCHFraction &&
                        jet->nCharged > minChargedMult &&
                        jet->electronFraction < maxCEMFraction)) {
                            return false;
		    }
                } else if (std::abs(jet->p4.eta()) < 2.6f && std::abs(jet->p4.eta()) <= 2.7f) {
                    float maxNHFraction = 0.90f;
                    float maxNEMFraction = 0.99f;
                    float maxMuFraction = 0.8f;
                    float minChargedMult = 0;
                    float maxCEMFraction = 0.80f;

                    if (!(jet->neutralHadronFraction < maxNHFraction &&
                        (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                        jet->muonFraction < maxMuFraction &&
                        jet->nCharged > minChargedMult &&
                        jet->electronFraction < maxCEMFraction)) {
                            return false;
		    }
                } else if (std::abs(jet->p4.eta()) < 2.7f && std::abs(jet->p4.eta()) <= 3.0f) {
                    float minNEMFraction = 0.01f;
                    float maxNEMFraction = 0.99f;
                    float minNeutral = 1;
                    if (!((jet->photonFraction + jet->hfEMFraction) > minNEMFraction &&
                        (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                        (jet->nConstituents - jet->nCharged) > minNeutral)) {
                            return false;
		    }
                } else if (std::abs(jet->p4.eta()) < 3.0f && std::abs(jet->p4.eta()) <= 5.0f) {
                    float minNHFraction = 0.20f;
                    float maxNEMFraction = 0.90f;
                    float minNeutral = 10;
                    if (!(jet->neutralHadronFraction > minNHFraction &&
                        (jet->photonFraction + jet->hfEMFraction) < maxNEMFraction &&
                        (jet->nConstituents - jet->nCharged) > minNeutral)) {
                            return false;
		    }
                }
            } else { // no valid jetIDVersion selected
            	LOG(FATAL) << "No valid jetIDVersion selected!" << std::endl;
            }
        } else if (settings.GetTaggedJets().find("PUPPI") != std::string::npos) { // for future; PUPPI jets not used yet
            // Note that the jet ID requirements for |eta|>2.7 are not recommended for PUPPI jets
            LOG(FATAL) << "PUPPI not implemented yet!" << std::endl;
        } else {  // unknown TaggedJets
            LOG(FATAL) << "Unknown TaggedJets; Please verify your configuration!" << std::endl;
        }

        // all cuts passed!
	    return true;
	}


protected:
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(TJet* jet, KappaEvent const& event,
	                                KappaProduct& product, KappaSettings const& settings) const
	{
	    return true;
	}


private:
	std::vector<TJet>* KappaEvent::*m_basicJetsMember;
	std::vector<std::shared_ptr<TJet> > KappaProduct::*m_correctedJetsMember;

	KappaEnumTypes::ValidJetsInput validJetsInput;
	KappaEnumTypes::JetIDVersion jetIDVersion;
	KappaEnumTypes::JetID jetID;
};



/**
   \brief Producer for valid jets (simple PF jets).

   Operates on the vector event.m_basicJets.
*/
class ValidJetsProducer: public ValidJetsProducerBase<KBasicJet, KBasicJet>
{
public:
	ValidJetsProducer();

	std::string GetProducerId() const override;
};



/**
   \brief Producer for valid jets (tagged PF jets).

   Operates on the vector event.m_tjets.

   Required config tags:
   - PuJetIDs
   - JetTaggerLowerCuts
*/
class ValidTaggedJetsProducer: public ValidJetsProducerBase<KJet, KBasicJet>
{
public:
	ValidTaggedJetsProducer();
	std::string GetProducerId() const override;
	void Init(KappaSettings const& settings) override;

protected:
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KJet* jet, KappaEvent const& event,
	                                KappaProduct& product, KappaSettings const& settings) const;

private:
	std::map<size_t, std::vector<std::string> > puJetIdsByIndex;
	std::map<std::string, std::vector<std::string> > puJetIdsByHltName;
	std::map<std::string, std::vector<float> > jetTaggerLowerCutsByTaggerName;
	std::map<std::string, std::vector<float> > jetTaggerUpperCutsByTaggerName;

	bool PassPuJetIds(KJet* jet, std::vector<std::string> const& puJetIds, KJetMetadata* taggerMetadata) const;
};


