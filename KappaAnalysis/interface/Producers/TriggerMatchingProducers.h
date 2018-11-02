
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <typeinfo>



/** Abstract Producer class for trigger matching valid objects
 *
 *	Needs to run after the valid object producers.
 */
template<class TValidObject>
class TriggerMatchingProducerBase: public KappaProducerBase
{

public:

	// reccommended access to product.m_detailedTriggerMatched.... by using only the HLT paths where all configured filters match with the valid object
	static std::vector<std::string> GetHltNamesWhereAllFiltersMatched(
			std::map<std::string, std::map<std::string, std::vector<KLV*> > > const& detailedTriggerMatchedObjects
	) {
		std::vector<std::string> hltNames;
		
		for (std::pair<std::string, std::map<std::string, std::vector<KLV*> > > hltName : detailedTriggerMatchedObjects)
		{
			bool allFiltersMatched = true;
			for (std::pair<std::string, std::vector<KLV*> > filterName : hltName.second)
			{
				if (filterName.second.size() == 0)
				{
					allFiltersMatched = false;
				}
			}
			
			if (allFiltersMatched)
			{
				hltNames.push_back(hltName.first);
			}
		}
		
		return hltNames;
	}
	
	TriggerMatchingProducerBase(std::map<TValidObject*, KLV> KappaProduct::*triggerMatchedObjects,
	                            std::map<TValidObject*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > > KappaProduct::*detailedTriggerMatchedObjects,
	                            std::map<TValidObject*, std::map<std::string, bool > > KappaProduct::*objectTriggerMatch,
	                            std::vector<TValidObject*> KappaProduct::*validObjects,
	                            std::vector<TValidObject*> KappaProduct::*invalidObjects,
	                            std::map<size_t, std::vector<std::string> > KappaProduct::*settingsObjectTriggerFiltersByIndex,
	                            std::map<std::string, std::vector<std::string> > KappaProduct::*settingsObjectTriggerFiltersByHltName,
	                            std::vector<std::string>& (KappaSettings::*GetObjectTriggerFilterNames)(void) const,
	                            float (KappaSettings::*GetDeltaRTriggerMatchingObjects)(void) const,
	                            bool (KappaSettings::*GetInvalidateNonMatchingObjects)(void) const) :
		m_triggerMatchedObjects(triggerMatchedObjects),
		m_detailedTriggerMatchedObjects(detailedTriggerMatchedObjects),
		m_objectTriggerMatch(objectTriggerMatch),
		m_validObjects(validObjects),
		m_invalidObjects(invalidObjects),
		m_settingsObjectTriggerFiltersByIndex(settingsObjectTriggerFiltersByIndex),
		m_settingsObjectTriggerFiltersByHltName(settingsObjectTriggerFiltersByHltName),
		GetObjectTriggerFilterNames(GetObjectTriggerFilterNames),
		GetDeltaRTriggerMatchingObjects(GetDeltaRTriggerMatchingObjects),
		GetInvalidateNonMatchingObjects(GetInvalidateNonMatchingObjects)
	{
	}

	void Init(KappaSettings const& settings) override {
		KappaProducerBase::Init(settings);
		
		m_objectTriggerFiltersByIndexFromSettings = Utility::ParseMapTypes<size_t, std::string>(Utility::ParseVectorToMap((settings.*GetObjectTriggerFilterNames)()), m_objectTriggerFiltersByHltNameFromSettings);
	}

	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override
	{
		assert(event.m_triggerObjects);
		assert(event.m_triggerObjectMetadata);

		if ((product.*m_settingsObjectTriggerFiltersByIndex).empty())
		{
			(product.*m_settingsObjectTriggerFiltersByIndex).insert(m_objectTriggerFiltersByIndexFromSettings.begin(),
			                                                        m_objectTriggerFiltersByIndexFromSettings.end());
		}
		if ((product.*m_settingsObjectTriggerFiltersByHltName).empty())
		{
			(product.*m_settingsObjectTriggerFiltersByHltName).insert(m_objectTriggerFiltersByHltNameFromSettings.begin(),
			                                                          m_objectTriggerFiltersByHltNameFromSettings.end());
		}
		
		(product.*m_triggerMatchedObjects).clear();
		(product.*m_detailedTriggerMatchedObjects).clear();
		(product.*m_objectTriggerMatch).clear();

		// loop over all valid objects
		unsigned int triggerObjectsSize = event.m_triggerObjects->trgObjects.size();
		for (typename std::vector<TValidObject*>::iterator validObject = (product.*m_validObjects).begin();
		     validObject != (product.*m_validObjects).end(); ++validObject)
		{
                        LOG(DEBUG) << "Processing TO matching for " << (*validObject)->p4;
                        std::vector<std::pair<KLV,std::vector<std::string>>> deltaRMatchedTOs;
                        RMFLV vec;
                        // Take into account, that superCluster position must be taken for deltaR matching in case of electrons
                        if( typeid((*validObject)) == typeid(KElectron*))
                        {
                            KElectron* elCopy = dynamic_cast<KElectron*>((*validObject));
                            vec.SetCoordinates(elCopy->p4.Pt(), elCopy->superclusterPosition.Eta(), elCopy->superclusterPosition.Phi(), elCopy->p4.M());
                        }
                        else
                        {
                            vec = (*validObject)->p4;
                        }
                        // match the trigger objects to the reco object by deltaR
                        for (unsigned int i=0; i<triggerObjectsSize; i++)
                        {
                                if (ROOT::Math::VectorUtil::DeltaR(event.m_triggerObjects->trgObjects.at(i).p4, vec) < (settings.*GetDeltaRTriggerMatchingObjects)() && event.m_triggerObjects->trgObjects.at(i).p4.Pt() > settings.GetTriggerObjectLowerPtCut())
                                {
                                        deltaRMatchedTOs.push_back(std::pair<KLV,std::vector<std::string>>(event.m_triggerObjects->trgObjects.at(i), event.m_triggerObjects->filterLabels.at(i)));
                                }
                        }
                        LOG(DEBUG) << "Number of deltaR matched TOs: " << deltaRMatchedTOs.size();
                        LOG(DEBUG) << "Total number of TOs: " << triggerObjectsSize;
                        // Merge matched trigger objects with the same p4 together
                        std::map<std::array<float, 4>, std::vector<std::string>> mergedTOs; // filling of the array: pt, eta, phi, mass; needed to be able to order within map
                        for (auto to : deltaRMatchedTOs)
                        {
                            std::array<float, 4> momentum_array = {{to.first.p4.Pt(), to.first.p4.Eta(), to.first.p4.Phi(), to.first.p4.mass()}};
                            unsigned int initial_size = mergedTOs[momentum_array].size();
                            mergedTOs[momentum_array].insert(mergedTOs[momentum_array].end(),to.second.begin(), to.second.end());
                            if(mergedTOs[momentum_array].size() != initial_size && initial_size > 0)
                            {
                                LOG(DEBUG) << "Merging filters for (Pt, Eta, Phi, mass) = (" << momentum_array[0] << ", " << momentum_array[1] << ", " << momentum_array[2] << ", " <<  momentum_array[3] << ")";
                            }
                        }
                        deltaRMatchedTOs.clear();
                        // Filling the matched TOs again, this time with merged filtes
                        for (auto to : mergedTOs)
                        {
                            KLV mergedTO;
                            mergedTO.p4.SetCoordinates(to.first[0], to.first[1], to.first[2], to.first[3]);
                            deltaRMatchedTOs.push_back(std::pair<KLV,std::vector<std::string>>(mergedTO, to.second));
                        }
                        // Prepare HLT path <----> filter matches relation & match TOs
			for (std::map<std::string, std::vector<std::string>>::const_iterator objectTriggerFilterByHltName = (product.*m_settingsObjectTriggerFiltersByHltName).begin();
			     objectTriggerFilterByHltName != (product.*m_settingsObjectTriggerFiltersByHltName).end();
			     ++objectTriggerFilterByHltName)
			{
				LOG(DEBUG) << "HLT name = " << objectTriggerFilterByHltName->first;
                                for (auto triggerObject : deltaRMatchedTOs)
                                {
                                        LOG(DEBUG) << "\tProcessing trigger object " << triggerObject.first.p4;
                                        for (auto filterLabel : triggerObject.second)
                                        {
                                            LOG(DEBUG) << "\t\tavailable filters: " << filterLabel;
                                        }
                                        // loop over the filter regexp associated with the given hlt in the config
                                        unsigned int countFilterMatches = 0;
                                        for (std::vector<std::string>::const_iterator filterNames = objectTriggerFilterByHltName->second.begin();
                                             filterNames != objectTriggerFilterByHltName->second.end();
                                             ++filterNames)
                                        {
                                                LOG(DEBUG) << "\t\t\tcheck filterNames to be ORed = " << *filterNames;
                                                std::vector<std::string> filterNamesList;
                                                boost::split(filterNamesList, *filterNames, boost::is_any_of(","));
                                                bool matchedToOr = false;
                                                for (std::vector<std::string>::const_iterator filterName = objectTriggerFilterByHltName->second.begin();
                                                     filterName != objectTriggerFilterByHltName->second.end();
                                                     ++filterName)
                                                {
                                                        matchedToOr = (matchedToOr || (triggerObject.second.end() !=  std::find(triggerObject.second.begin(), triggerObject.second.end(), *filterName)));
                                                        if(matchedToOr) break;
                                                }
                                                if(matchedToOr) countFilterMatches++;
                                        }
                                        if (countFilterMatches ==  objectTriggerFilterByHltName->second.size())
                                        {
                                                LOG(DEBUG) << "\tFound HLT match! ";
                                                (product.*m_objectTriggerMatch)[*validObject][objectTriggerFilterByHltName->first] = true;
                                                (product.*m_triggerMatchedObjects)[*validObject] = triggerObject.first;
                                                break;
                                        }
                                }
                        }
		}
	}


private:
	std::map<TValidObject*, KLV> KappaProduct::*m_triggerMatchedObjects;
	std::map<TValidObject*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > > KappaProduct::*m_detailedTriggerMatchedObjects;
	std::map<TValidObject*, std::map<std::string, bool > > KappaProduct::*m_objectTriggerMatch;
	std::vector<TValidObject*> KappaProduct::*m_validObjects;
	std::vector<TValidObject*> KappaProduct::*m_invalidObjects;
	std::map<size_t, std::vector<std::string> > KappaProduct::*m_settingsObjectTriggerFiltersByIndex;
	std::map<std::string, std::vector<std::string> > KappaProduct::*m_settingsObjectTriggerFiltersByHltName;
	std::vector<std::string>& (KappaSettings::*GetObjectTriggerFilterNames)(void) const;
	float (KappaSettings::*GetDeltaRTriggerMatchingObjects)(void) const;
	bool (KappaSettings::*GetInvalidateNonMatchingObjects)(void) const;
	
	std::map<size_t, std::vector<std::string> > m_objectTriggerFiltersByIndexFromSettings;
	std::map<std::string, std::vector<std::string> > m_objectTriggerFiltersByHltNameFromSettings;

};


/** Producer for trigger matched electrons
 *  Required config tags:
 *  - DeltaRTriggerMatchingElectrons (default provided)
 *  - InvalidateNonMatchingElectrons (default provided)
 *  - ElectronTriggerFilterNames
 */
class ElectronTriggerMatchingProducer: public TriggerMatchingProducerBase<KElectron>
{

public:
	
	std::string GetProducerId() const override;
	
	ElectronTriggerMatchingProducer();
	
	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

};


/** Producer for trigger matched muons
 *  Required config tags:
 *  - DeltaRTriggerMatchingMuons (default provided)
 *  - InvalidateNonMatchingMuons (default provided)
 *  - MuonTriggerFilterNames
 */
class MuonTriggerMatchingProducer: public TriggerMatchingProducerBase<KMuon>
{

public:
	
	std::string GetProducerId() const override;
	
	MuonTriggerMatchingProducer();
	
	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

};


/** Producer for trigger matched taus
 *  Required config tags:
 *  - DeltaRTriggerMatchingTaus (default provided)
 *  - InvalidateNonMatchingTaus (default provided)
 *  - TauTriggerFilterNames
 */
class TauTriggerMatchingProducer: public TriggerMatchingProducerBase<KTau>
{

public:
	
	std::string GetProducerId() const override;
	
	TauTriggerMatchingProducer();
	
	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

};


/** Producer for trigger matched jets
 *  Required config tags:
 *  - DeltaRTriggerMatchingJets (default provided)
 *  - InvalidateNonMatchingJets (default provided)
 *  - JetTriggerFilterNames
 */
class JetTriggerMatchingProducer: public TriggerMatchingProducerBase<KBasicJet>
{

public:
	
	std::string GetProducerId() const override;
	
	JetTriggerMatchingProducer();

};

