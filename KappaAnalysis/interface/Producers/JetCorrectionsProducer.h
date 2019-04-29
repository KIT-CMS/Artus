
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
	}

	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override
	{
                LOG(DEBUG) << "\nStarting " << this->GetProducerId() <<  ". Consider pipeline: " << settings.GetRootFileFolder();
		assert((event.*m_basicJetsMember));
		assert(event.m_pileupDensity);
		assert(event.m_vertexSummary);
		// create a copy of all jets in the event (first temporarily for the JEC)
		(product.*m_correctedJetsMember).clear();
		std::vector<TJet> correctJetsForJecTools((event.*m_basicJetsMember)->size());
		size_t jetIndex = 0;
		for (typename std::vector<TJet>::const_iterator jet = (event.*m_basicJetsMember)->begin();
			 jet != (event.*m_basicJetsMember)->end(); ++jet)
		{
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
                                // add jet momentum to met shift and later subtract shifted momentum in order to get MET shift
                                product.m_MET_shift.p4 += jet->p4;
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
                                jet->p4 = jet->p4 * (1 + grouped_unc * settings.GetJetEnergyCorrectionUncertaintyShift());
                                product.m_MET_shift.p4 -= jet->p4;
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

	std::map<KappaEnumTypes::JetEnergyUncertaintyShiftName, JetCorrectorParameters const*> JetCorParMap;
	std::map<KappaEnumTypes::JetEnergyUncertaintyShiftName, JetCorrectionUncertainty *> JetUncMap;

	FactorizedJetCorrector* factorizedJetCorrector = nullptr;
	JetCorrectionUncertainty* jetCorrectionUncertainty = nullptr;
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


