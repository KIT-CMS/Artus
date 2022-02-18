
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"

#include "Kappa/DataFormats/interface/Kappa.h"

#define USE_JEC
#include "KappaTools/RootTools/interface/JECTools.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Utility/interface/Utility.h"

#include "JetMETCorrections/Modules/interface/JetResolution.h"

#include "TRandom3.h"

/**
   \brief Producer for jet corrections (mainly JEC)

   Required config tags:
   - JetEnergyCorrectionParameters (files containing the correction parameters in the right order)
   - JetEnergyCorrectionUncertaintyParameters (default: empty)
   - JetEnergyCorrectionUncertaintySource (default "")
   - JetEnergyCorrectionUncertaintyShift (default 0.0)

   Required packages (unfortunately, nobody knows a tag):
   git cms-addpkg CondFormats/JetMETObjects

   Documentation:
   https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetEnCorFWLite

   TODO: the code can be moved to a .cc file, if there are no external users using the TJet templated
         version
*/


template<class TJet>
class JetCorrectionsProducerBase: public KappaProducerBase
{

public:

	JetCorrectionsProducerBase(std::vector<TJet>* KappaEvent::*jets,
	                           std::vector<std::shared_ptr<TJet> > KappaProduct::*correctedJets) :
		KappaProducerBase(),
		m_basicJetsMember(jets),
		m_correctedJetsMember(correctedJets)
	{
	}

	~JetCorrectionsProducerBase()
	{
		delete factorizedJetCorrector;
	}

	void Init(KappaSettings const& settings) override
	{
		KappaProducerBase::Init(settings);
		// load correction parameters
		LOG(DEBUG) << "\tLoading JetCorrectorParameters from files...";
		std::vector<JetCorrectorParameters> jecParameters;
		for (std::vector<std::string>::const_iterator jecParametersFile = settings.GetJetEnergyCorrectionParameters().begin();
		     jecParametersFile != settings.GetJetEnergyCorrectionParameters().end(); ++jecParametersFile)
		{
			jecParameters.push_back(JetCorrectorParameters(*jecParametersFile));
			LOG(DEBUG) << "\t\t" << *jecParametersFile;
		}
		if (jecParameters.size() > 0)
		{
			factorizedJetCorrector = new FactorizedJetCorrector(jecParameters);
		}

		// initialise uncertainty calculation
		LOG(DEBUG) << "\tLoading JetCorrectionUncertainty from files...";
		if ((! settings.GetJetEnergyCorrectionUncertaintyParameters().empty()) &&
		    (settings.GetJetEnergyCorrectionUncertaintyShift() != 0.0))
		{
			JetCorrectorParameters* jecUncertaintyParameters = nullptr;
			if (!settings.GetJetEnergyCorrectionUncertaintySource().empty()) {
				jecUncertaintyParameters = new JetCorrectorParameters(
						settings.GetJetEnergyCorrectionUncertaintyParameters(),
						settings.GetJetEnergyCorrectionUncertaintySource()
				);
			}
			else {
				jecUncertaintyParameters = new JetCorrectorParameters(settings.GetJetEnergyCorrectionUncertaintyParameters());
			}
			if ((!jecUncertaintyParameters->isValid()) || (jecUncertaintyParameters->size() == 0))
				LOG(FATAL) << "Invalid definition " << settings.GetJetEnergyCorrectionUncertaintySource()
				           << " in file " << settings.GetJetEnergyCorrectionUncertaintyParameters();
			jetCorrectionUncertainty = new JetCorrectionUncertainty(*jecUncertaintyParameters);
			LOG(DEBUG) << "\t\t" << settings.GetJetEnergyCorrectionUncertaintySource();
			LOG(DEBUG) << "\t\t" << settings.GetJetEnergyCorrectionUncertaintyParameters();
		}

                uncertaintyFile = settings.GetJetEnergyCorrectionSplitUncertaintyParameters();
                individualUncertainties = settings.GetJetEnergyCorrectionSplitUncertaintyParameterNames();

                // make sure the necessary parameters are configured
                assert(uncertaintyFile != "");
                if (settings.GetUseGroupedJetEnergyCorrectionUncertainty()) assert(individualUncertainties.size() > 0);

                for (auto const& uncertainty : individualUncertainties)
                {
                        // only do string comparison once per uncertainty
                        KappaEnumTypes::JetEnergyUncertaintyShiftName individualUncertainty = KappaEnumTypes::ToJetEnergyUncertaintyShiftName(uncertainty);
                        if (individualUncertainty == KappaEnumTypes::JetEnergyUncertaintyShiftName::NONE)
                                continue;
                        individualUncertaintyEnums.push_back(individualUncertainty);

                        // create uncertainty map (only if shifts are to be applied)
                        if (settings.GetUseGroupedJetEnergyCorrectionUncertainty()
                                && settings.GetJetEnergyCorrectionUncertaintyShift() != 0.0
                                && individualUncertainty != KappaEnumTypes::JetEnergyUncertaintyShiftName::Closure)
                        {
                                JetCorrectorParameters const * jetCorPar = new JetCorrectorParameters(uncertaintyFile, uncertainty);
                                JetCorParMap[individualUncertainty] = jetCorPar;

                                JetCorrectionUncertainty * jecUnc(new JetCorrectionUncertainty(*JetCorParMap[individualUncertainty]));
                                JetUncMap[individualUncertainty] = jecUnc;
                        }
                }
                assert(settings.GetJetEnergyResolutionSource() != "");
				assert(settings.GetJetEnergyResolutionMethod() != "");
                assert(settings.GetJetEnergyResolutionSFSource() != "");
				if(settings.GetJetEnergyResolutionMethod()=="hybrid") JER_method =  hybrid;
				else if(settings.GetJetEnergyResolutionMethod()=="stochastic") JER_method = stochastic;
				else LOG(FATAL) << "Unknown JetEnergyResolutionCorrectionMethod: " << settings.GetJetEnergyResolutionMethod();

				m_jetResolution.reset(new JME::JetResolution(settings.GetJetEnergyResolutionSource()));
                m_jetResolutionScaleFactor.reset(new JME::JetResolutionScaleFactor(settings.GetJetEnergyResolutionSFSource()));
                if(settings.GetJetEnergyResolutionUncertaintyShift()==0.0) JER_shift = Variation::NOMINAL;
                else if(settings.GetJetEnergyResolutionUncertaintyShift()==1.0) JER_shift = Variation::UP;
                else if(settings.GetJetEnergyResolutionUncertaintyShift()==-1.0) JER_shift = Variation::DOWN;
                else LOG(FATAL) << "Invalid definition of JetEnergyResolutionUncertaintyShift: " << settings.GetJetEnergyResolutionUncertaintyShift();
	}

	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override
	{
                LOG(DEBUG) << "\nStarting " << this->GetProducerId() <<  ". Consider pipeline: " << settings.GetRootFileFolder();
		assert((event.*m_basicJetsMember));
		assert(event.m_pileupDensity);
		assert(event.m_vertexSummary);
                if (JER_method == hybrid) {
                    assert(event.m_genJets);
                }
		// create a copy of all jets in the event (first temporarily for the JEC)
		(product.*m_correctedJetsMember).clear();
		std::vector<TJet> correctJetsForJecTools((event.*m_basicJetsMember)->size());
		size_t jetIndex = 0;
		for (typename std::vector<TJet>::const_iterator jet = (event.*m_basicJetsMember)->begin();
			 jet != (event.*m_basicJetsMember)->end(); ++jet)
		{
			LOG(DEBUG) << "\tInitial jet p4: " << jet->p4;
			correctJetsForJecTools[jetIndex] = *jet;
			++jetIndex;
		}

                if (settings.GetUseGroupedJetEnergyCorrectionUncertainty() && settings.GetJetEnergyCorrectionUncertaintyShift() != 0.0)
                {
                        // run over all jets
                        for (typename std::vector<TJet>::iterator jet = correctJetsForJecTools.begin();
                                 jet != correctJetsForJecTools.end(); ++jet)
                        {
                                LOG(DEBUG) << "\tConsidering jet with p4 = " << jet->p4;
                                // shift corrected jets
                                double grouped_unc = 0.0;
				if(individualUncertaintyEnums.size() == 1)
				{

                                        if (std::abs(jet->p4.Eta()) < 5.2 && jet->p4.Pt() > 9.)
                                        {
                                                auto const& uncertainty = individualUncertaintyEnums.at(0);
                                                JetUncMap.at(uncertainty)->setJetEta(jet->p4.Eta());
                                                JetUncMap.at(uncertainty)->setJetPt(jet->p4.Pt());
                                                grouped_unc = JetUncMap.at(uncertainty)->getUncertainty(true);
                                        }
				}
				else
				{
					for (auto const& uncertainty : individualUncertaintyEnums)
					{
						double unc = 0.0;

						if (std::abs(jet->p4.Eta()) < 5.2 && jet->p4.Pt() > 9.)
						{
							JetUncMap.at(uncertainty)->setJetEta(jet->p4.Eta());
							JetUncMap.at(uncertainty)->setJetPt(jet->p4.Pt());
							unc = JetUncMap.at(uncertainty)->getUncertainty(true);
							grouped_unc += unc * unc;
						}
					}
					grouped_unc = sqrt(grouped_unc);
				}
                                if (jet->p4.Pt() * (1 + grouped_unc * settings.GetJetEnergyCorrectionUncertaintyShift())>15.0){ // requirement for type I corrections
                                        product.m_MET_shift.p4 += jet->p4;
                                        jet->p4 = jet->p4 * (1 + grouped_unc * settings.GetJetEnergyCorrectionUncertaintyShift());
                                        product.m_MET_shift.p4 -= jet->p4;
                                } else {
                                        jet->p4 = jet->p4 * (1 + grouped_unc * settings.GetJetEnergyCorrectionUncertaintyShift());
                                }
                                LOG(DEBUG) << "\tGrouped uncertainty applied: " << grouped_unc << " shifted p4: " << jet->p4;
                        }
                }
                else if (settings.GetJetEnergyCorrectionUncertaintyShift() != 0.0)
                {
		// apply jet energy corrections and uncertainty shift (if uncertainties are not to be splitted into individual contributions)
		float shift = settings.GetJetEnergyCorrectionSplitUncertainty() ? 0.0 : settings.GetJetEnergyCorrectionUncertaintyShift();
		correctJets(&correctJetsForJecTools, factorizedJetCorrector, jetCorrectionUncertainty,
		            event.m_pileupDensity->rho, event.m_vertexSummary->nVertices, -1,
		            shift);
                }
                // apply JER smearing
				// stochastic smearing only
				LOG(DEBUG) << "\t Applying JER smearing";
				if (JER_method == stochastic){
					LOG(DEBUG) << "\t Using stochastic JER smearing";
					for (typename std::vector<TJet>::iterator jet = correctJetsForJecTools.begin();
                                 jet != correctJetsForJecTools.end(); ++jet)
					{
							randm.SetSeed(static_cast<int>((jet->p4.Eta() + 5) * 1000) * 1000 + static_cast<int>((jet->p4.Phi() + 4) * 1000) + 10000);
							double jetResolution = m_jetResolution->getResolution({
								{JME::Binning::JetPt, jet->p4.Pt()},
								{JME::Binning::JetEta, jet->p4.Eta()},
								{JME::Binning::Rho, event.m_pileupDensity->rho}
							});
							double jetResolutionScaleFactor = m_jetResolutionScaleFactor->getScaleFactor({
								{JME::Binning::JetPt, jet->p4.Pt()},
								{JME::Binning::JetEta, jet->p4.Eta()}
							}, JER_shift);
                                                        double res_rndm = randm.Gaus(0, jetResolution);
							double shift = res_rndm * std::sqrt(std::max(jetResolutionScaleFactor * jetResolutionScaleFactor - 1, 0.0));
							if (shift < -1.0) shift = -1.0;
							if (((jet->p4*(1.0+shift)).Pt()>15.0)&&(JER_shift!=Variation::NOMINAL)) {
                                                            // MET is already corrected for nominal jer correction,
                                                            // so we need to only propagate the difference in corrections here
                                                            double jetResolutionScaleFactor_nom = m_jetResolutionScaleFactor->getScaleFactor({
                                                                    {JME::Binning::JetPt, jet->p4.Pt()},
                                                                    {JME::Binning::JetEta, jet->p4.Eta()}
                                                            }, Variation::NOMINAL);
                                                            double shift_nom = res_rndm * std::sqrt(std::max(jetResolutionScaleFactor_nom * jetResolutionScaleFactor_nom - 1, 0.0));
                                                            if (shift_nom < -1.0) shift_nom = -1.0;
                                                            product.m_MET_shift.p4 -= jet->p4*(shift-shift_nom); // requirement for type I corrections
                                                        }
							jet->p4 *= 1.0 + shift;
					}
				}
				// apply JER smearing
				// hybrid method = stochastic + scaling method
				// Details: https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution#Smearing_procedures
				// Based on https://github.com/cms-sw/cmssw/blob/CMSSW_10_2_X/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h implementaion
				else if (JER_method == hybrid){
					LOG(DEBUG) << "\t Using hybrid JER smearing";
					// for (auto jet: correctJetsForJecTools)
					for (typename std::vector<TJet>::iterator jet = correctJetsForJecTools.begin();
                                 jet != correctJetsForJecTools.end(); ++jet)
					{
							randm.SetSeed(static_cast<int>((jet->p4.Eta() + 5) * 1000) * 1000 + static_cast<int>((jet->p4.Phi() + 4) * 1000) + 10000);
							double jetResolution = m_jetResolution->getResolution({
								{JME::Binning::JetPt, jet->p4.Pt()},
								{JME::Binning::JetEta, jet->p4.Eta()},
								{JME::Binning::Rho, event.m_pileupDensity->rho}
							});
							double jetResolutionScaleFactor = m_jetResolutionScaleFactor->getScaleFactor({
								{JME::Binning::JetPt, jet->p4.Pt()},
								{JME::Binning::JetEta, jet->p4.Eta()}
							}, JER_shift);
							LOG(DEBUG) << "jet:  pt: " << jet->p4.Pt() << "  eta: " << jet->p4.Eta() << "  phi: " << jet->p4.Phi() << "  rho: " << event.m_pileupDensity->rho << "  e: " << jet->p4.energy();
                   			LOG(DEBUG) << "resolution: " << jetResolution;
                    		LOG(DEBUG) << "resolution scale factor: " << jetResolutionScaleFactor;
							double shift = 0.;
                                                        // Need to define this variable to have the correct random number for the nominal shift
                                                        // when calculating the MET later.
                                                        double res_rndm = 0;
							RMFLV recoJetp4 = jet->p4;
							// now try to find a matching genjet
							KGenJet* genJet = match_genJet_deltaR(recoJetp4, event, jet->p4.Pt() * jetResolution);
							if (genJet != NULL){
								// we found a matching genJet, now we apply a smearing based on the genJet pt
								double dPt = jet->p4.pt() - genJet->p4.pt();
                    			shift = (jetResolutionScaleFactor - 1.) * dPt / jet->p4.pt();
							}
							else if(jetResolutionScaleFactor > 1){
								// no matching genjet, smear based on gaussian variation
                                                                res_rndm = randm.Gaus(0, jetResolution);
								shift = res_rndm * std::sqrt(std::max(jetResolutionScaleFactor * jetResolutionScaleFactor - 1, 0.0));
							}
							else {
								LOG(DEBUG) << "Jet smearing of this jet is not possible ! - setting shift to zero";
								shift = 0.;
							}
							if (shift < -1.0) shift = -1.0;
							if (((jet->p4*(1.0+shift)).Pt()>15.0)&&(JER_shift!=Variation::NOMINAL)) {
                                                            // MET is already corrected for nominal jer correction,
                                                            // so we need to only propagate the difference in corrections here
                                                            //
                                                            // First we need to get the nominal scale factor and afterwards calculate the shift
                                                            // as it would have been for the nominal variation of the SF
                                                            double jetResolutionScaleFactor_nom = m_jetResolutionScaleFactor->getScaleFactor({
                                                                    {JME::Binning::JetPt, jet->p4.Pt()},
                                                                    {JME::Binning::JetEta, jet->p4.Eta()}
                                                            }, Variation::NOMINAL);
                                                            double shift_nom = 0.;
                                                            if (genJet != NULL){
                                                                    // we found a matching genJet, now we apply a smearing based on the genJet pt
                                                                    double dPt = jet->p4.pt() - genJet->p4.pt();
                                                                    shift_nom = (jetResolutionScaleFactor_nom - 1.) * dPt / jet->p4.pt();
                                                            }
                                                            else if(jetResolutionScaleFactor_nom > 1){
                                                                    // no matching genjet, smear based on gaussian variation
                                                                    shift_nom = res_rndm * std::sqrt(std::max(jetResolutionScaleFactor_nom * jetResolutionScaleFactor_nom - 1, 0.0));
                                                            }
                                                            else {
                                                                    shift_nom = 0.;
                                                            }
                                                            if (shift_nom < -1.0) shift_nom = -1.0;
                                                            product.m_MET_shift.p4 -= jet->p4*(shift-shift_nom); // requirement for type I corrections
                                                        }
							jet->p4 *= 1.0 + shift;
							LOG(DEBUG) << "smeared jet (" << shift << "):  pt: " << jet->p4.pt() << "  eta: " << jet->p4.eta() << "  phi: " << jet->p4.phi() << "  e: " << jet->p4.energy();
					}
				}

		// create the shared pointers to store in the product
		(product.*m_correctedJetsMember).clear();
		(product.*m_correctedJetsMember).resize(correctJetsForJecTools.size());
		jetIndex = 0;
		for (typename std::vector<TJet>::const_iterator jet = correctJetsForJecTools.begin();
			 jet != correctJetsForJecTools.end(); ++jet)
		{
			(product.*m_correctedJetsMember)[jetIndex] = std::shared_ptr<TJet>(new TJet(*jet));
			product.m_originalJets[(product.*m_correctedJetsMember)[jetIndex].get()] = &(*jet);
			++jetIndex;
		}

		// perform additional corrections on copied jets
		for (typename std::vector<std::shared_ptr<TJet> >::iterator jet = (product.*m_correctedJetsMember).begin();
			 jet != (product.*m_correctedJetsMember).end(); ++jet)
		{
			AdditionalCorrections(jet->get(), event, product, settings);
                        LOG(DEBUG) << "\tFinal jet p4: " << (*jet)->p4;
		}

		// sort vectors of corrected jets by pt
		std::sort((product.*m_correctedJetsMember).begin(), (product.*m_correctedJetsMember).end(),
		          [](std::shared_ptr<TJet> jet1, std::shared_ptr<TJet> jet2) -> bool
		          { return jet1->p4.Pt() > jet2->p4.Pt(); });
	}

	virtual KGenJet* match_genJet_deltaR(RMFLV recoJetp4, const KappaEvent event, double resolution) const{
		LOG(DEBUG) << "\t Trying to match recoJet " << recoJetp4;
		double min_dR = std::numeric_limits<double>::infinity();
		double m_dPt_max_factor = 3.;
		double max_dR = 0.4;
		KGenJet* matched_genJet = nullptr;
		for (std::vector<KGenJet>::iterator genJet = event.m_genJets->begin();
		 	genJet != event.m_genJets->end(); ++genJet)
		{
			double dR = ROOT::Math::VectorUtil::DeltaR(recoJetp4, genJet->p4);
			if (dR > min_dR)
				continue;
			if (dR < max_dR)
			{
				double dPt = std::abs(genJet->p4.pt() - recoJetp4.pt());
				if (dPt > m_dPt_max_factor * resolution)
					continue;
				LOG(DEBUG) << "\t matching genJet found. " << genJet->p4 << " dR = " << dR << " dPt = " << dPt;
				min_dR = dR;
				matched_genJet = &(*genJet);
			}
		}
		return matched_genJet;
	}

protected:
	// Can be overwritten for analysis-specific use cases
	virtual void AdditionalCorrections(TJet* jet, KappaEvent const& event,
	                                   KappaProduct& product, KappaSettings const& settings) const
	{
	}


private:
	std::vector<TJet>* KappaEvent::*m_basicJetsMember;
	std::vector<std::shared_ptr<TJet> > KappaProduct::*m_correctedJetsMember;

	std::string uncertaintyFile;
	std::vector<std::string> individualUncertainties;
	std::vector<KappaEnumTypes::JetEnergyUncertaintyShiftName> individualUncertaintyEnums;
	enum JER_methods {stochastic, hybrid};
	JER_methods JER_method;
	std::map<KappaEnumTypes::JetEnergyUncertaintyShiftName, JetCorrectorParameters const*> JetCorParMap;
	std::map<KappaEnumTypes::JetEnergyUncertaintyShiftName, JetCorrectionUncertainty *> JetUncMap;

	FactorizedJetCorrector* factorizedJetCorrector = nullptr;
	JetCorrectionUncertainty* jetCorrectionUncertainty = nullptr;

	mutable TRandom3 randm = TRandom3(0);
	std::unique_ptr<JME::JetResolution> m_jetResolution;
	std::unique_ptr<JME::JetResolutionScaleFactor> m_jetResolutionScaleFactor;
	Variation JER_shift;
};


/**
   \brief Producer for Jet Energy Correction (JEC)

   Operates on the vector event.m_basicJets and product::m_correctedJets.
*/
class JetCorrectionsProducer: public JetCorrectionsProducerBase<KBasicJet>
{
public:
	JetCorrectionsProducer();

	std::string GetProducerId() const override;
};



/**
   \brief Producer for Jet Energy Correction (JEC)

   Operates on the vector event.m_tjets and product::m_correctedTaggedJets.
*/
class TaggedJetCorrectionsProducer: public JetCorrectionsProducerBase<KJet>
{
public:

	TaggedJetCorrectionsProducer();

	std::string GetProducerId() const override;
};
