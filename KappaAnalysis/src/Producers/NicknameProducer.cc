
#include "Artus/KappaAnalysis/interface/Producers/NicknameProducer.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"


std::string NicknameProducer::GetProducerId() const {
	return "NicknameProducer";
}

void NicknameProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddStringQuantity("nickname", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_nickname;
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isMC", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_isMC;
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isSingleMuon", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_isSingleMuon;
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isSingleElectron", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_isSingleElectron;
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isMuonEG", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_isMuonEG;
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isTau", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_isTau;
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isDoubleEG", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_isDoubleEG;
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isDoubleMuon", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_isDoubleMuon;
	});

	nickname = settings.GetNickname();
	if (nickname.find("RunII") != std::string::npos) isMC = true;
	else if (nickname.find("SingleMuon") != std::string::npos) isSingleMuon = true;
	else if (nickname.find("SingleElectron") != std::string::npos) isSingleElectron = true;
	else if (nickname.find("MuonEG") != std::string::npos) isMuonEG = true;
	else if (nickname.find("Tau") != std::string::npos) isTau = true;
	else if (nickname.find("DoubleEG") != std::string::npos) isDoubleEG = true;
	else if (nickname.find("DoubleMuon") != std::string::npos) isDoubleMuon = true;
}

void NicknameProducer::Produce(KappaEvent const& event, KappaProduct& product,
                               KappaSettings const& settings) const
{
	product.m_nickname = nickname;
	product.m_isSingleMuon = isSingleMuon;
	product.m_isSingleElectron = isSingleElectron;
	product.m_isMuonEG = isMuonEG;
	product.m_isTau = isTau;
	product.m_isDoubleEG = isDoubleEG;
	product.m_isDoubleMuon = isDoubleMuon;
	product.m_isMC = isMC;
}

