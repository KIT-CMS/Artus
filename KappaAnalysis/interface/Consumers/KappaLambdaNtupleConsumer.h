#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/Utility.h"

#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"



template<class TTypes>
class KappaLambdaNtupleConsumer: public LambdaNtupleConsumer<TTypes> {

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	std::string GetConsumerId() const override
	{
		return "KappaLambdaNtupleConsumer";
	}

	void Init(setting_type const& settings) override
	{
		// add possible quantities for the lambda ntuples consumers
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("input", [](event_type const& event, product_type const& product, setting_type const& settings)
		{
			return static_cast<int>(event.m_input);
		});
		LambdaNtupleConsumer<TTypes>::AddUInt64Quantity("run", [](event_type const& event, product_type const& product, setting_type const& settings) -> uint64_t
		{
			return event.m_eventInfo->nRun;
		});
		LambdaNtupleConsumer<TTypes>::AddUInt64Quantity("lumi", [](event_type const& event, product_type const& product, setting_type const& settings) -> uint64_t
		{
			return event.m_eventInfo->nLumi;
		});
		LambdaNtupleConsumer<TTypes>::AddUInt64Quantity("event", [](event_type const& event, product_type const& product, setting_type const& settings) -> uint64_t
		{
			return event.m_eventInfo->nEvent;
		});
		LambdaNtupleConsumer<TTypes>::AddUInt64Quantity("nbx", [](event_type const& event, product_type const& product, setting_type const& settings) -> uint64_t
		{
			return event.m_eventInfo->nBX;
		});
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("npv", [](event_type const& event, product_type const& product, setting_type const& settings)
		{
			return event.m_vertexSummary->nVertices;
		});

		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("firstPV_X", [](event_type const& event, product_type const& product, setting_type const& settings)
		{
			return event.m_vertexSummary->pv.position.X();
		});

		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("firstPV_Y", [](event_type const& event, product_type const& product, setting_type const& settings)
		{
			return event.m_vertexSummary->pv.position.Y();
		});

		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("firstPV_Z", [](event_type const& event, product_type const& product, setting_type const& settings)
		{
			return event.m_vertexSummary->pv.position.Z();
		});

		bool bInpData = settings.GetInputIsData();
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("npuMean", [bInpData](event_type const& event, product_type const& product, setting_type const& settings)
		{
			if (bInpData)
				return DefaultValues::UndefinedFloat;
			return static_cast<KGenEventInfo*>(event.m_eventInfo)->nPUMean;
		});

		LambdaNtupleConsumer<TTypes>::AddIntQuantity("npu", [bInpData](event_type const& event, product_type const& product, setting_type const& settings)
		{
			if (bInpData)
				return DefaultValues::UndefinedInt;
			return static_cast<int>(static_cast<KGenEventInfo*>(event.m_eventInfo)->nPU);
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("x1", [bInpData](event_type const& event, product_type const& product, setting_type const& settings)
		{
			return (bInpData) ? DefaultValues::UndefinedFloat : float(static_cast<KGenEventInfo*>(event.m_eventInfo)->x1);
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("x2", [bInpData](event_type const& event, product_type const& product, setting_type const& settings)
		{
			return (bInpData) ? DefaultValues::UndefinedFloat : float(static_cast<KGenEventInfo*>(event.m_eventInfo)->x2);
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("qScale", [bInpData](event_type const& event, product_type const& product, setting_type const& settings)
		{
			return (bInpData) ? DefaultValues::UndefinedFloat : float(static_cast<KGenEventInfo*>(event.m_eventInfo)->qScale);
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("rho", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_pileupDensity->rho;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("PFMet", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_met->p4.Pt();
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("trackmet", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_trackMet->p4.Pt();
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("trackmetphi", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_trackMet->p4.Phi();
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("trackmetsumet", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_trackMet->sumEt;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("pumet", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_puMet->p4.Pt();
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("pumetphi", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_puMet->p4.Phi();
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("pumetsumet", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_puMet->sumEt;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("nopumet", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_noPuMet->p4.Pt();
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("nopumetphi", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_noPuMet->p4.Phi();
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("nopumetsumet", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_noPuMet->sumEt;
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("pucormet", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_puCorMet->p4.Pt();
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("pucormetphi", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_puCorMet->p4.Phi();
		});
		LambdaNtupleConsumer<TTypes>::AddFloatQuantity("pucormetsumet", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return event.m_puCorMet->sumEt;
		});
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("genNPartons", [](event_type const& event, product_type const& product, setting_type const& settings) {
			return product.m_genNPartons;
		});
		LambdaNtupleConsumer<TTypes>::AddIntQuantity("NPFCandidates", [](event_type const& event, product_type const& product, setting_type const& settings)
		{
			return event.m_packedPFCandidates->size();
		});

		// loop over all quantities containing "weight" (case-insensitive)
		// and try to find them in the weights map to write them out
		for (auto const & quantity : settings.GetQuantities())
		{
			if (boost::algorithm::icontains(quantity, "weight") &&
			    (LambdaNtupleConsumer<TTypes>::GetFloatQuantities().count(quantity) == 0) &&
			    (LambdaNtupleConsumer<TTypes>::GetDoubleQuantities().count(quantity) == 0))
			{
				LOG(DEBUG) << "\tQuantity \"" << quantity << "\" is tried to be taken from product.m_weights or product.m_optionalWeights.";
				LambdaNtupleConsumer<TTypes>::AddFloatQuantity( quantity, [quantity](event_type const& event, product_type const& product, setting_type const& settings)
				{
					return SafeMap::GetWithDefault(product.m_weights, quantity, SafeMap::GetWithDefault(product.m_optionalWeights, quantity, 1.0));
				} );
			}
			if ((boost::algorithm::icontains(quantity, "filter") || boost::algorithm::icontains(quantity, "cut")) &&
			   (LambdaNtupleConsumer<TTypes>::GetFloatQuantities().count(quantity) == 0))
			{
				LOG(DEBUG) << "\tQuantity \"" << quantity << "\" is tried to be taken from product.fres (FilterResult).";
				LambdaNtupleConsumer<TTypes>::AddIntQuantity( quantity, [quantity](event_type const& event, product_type const& product, setting_type const& settings)
				{
					if (product.fres.GetDecisionEntry(quantity) != nullptr)
					{
						return (product.fres.GetDecisionEntry(quantity)->filterDecision == FilterResult::Decision::Passed) ? 1 : 0;
					}
					return -1;
				} );
			}
		}

		// need to be called at last
		LambdaNtupleConsumer<TTypes>::Init(settings);
	}
};
