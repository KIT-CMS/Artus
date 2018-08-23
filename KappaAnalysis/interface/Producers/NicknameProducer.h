#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
   \brief Producer that offers writing out the nickname to the tree
   
   Required config tag:
   - Nickname (string)
*/
class NicknameProducer : public KappaProducerBase {
public:

	std::string GetProducerId() const override;

	void Init(KappaSettings const& settings) override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

private:
        bool isSingleMuon = false;
        bool isSingleElectron = false;
        bool isMuonEG = false;
        bool isTau = false;
        bool isDoubleEG = false;
        bool isDoubleMuon = false;
        bool isMC = false;
        bool isEmbedded = false;
        std::string nickname;
};
