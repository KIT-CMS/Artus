#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

#include "Artus/Utility/interface/RoccoR.h"
#include "Artus/Utility/interface/rochcor2015.h"
#include "TRandom3.h"

#include <memory>  // Hinzufügen für Smart Pointers

/**
   \brief Producer for muon four momentum corrections.
   \Rochester Corrections are included
*/
class MuonCorrectionsProducer : public KappaProducerBase {

public:
    std::string GetProducerId() const override;

    void Init(setting_type const &settings) override;

    void Produce(KappaEvent const &event, KappaProduct &product,
                 KappaSettings const &settings) const override;

public:
    enum class MuonEnergyCorrection : int {
        NONE = -1,
        FALL2015 = 0,
        ROCHCORR2015 = 1,
        ROCHCORR2016 = 2,
        ROCHCORR2017 = 3,
        ROCHCORR2018 = 4,
        ROCHCORR2016UL = 5,
        ROCHCORR2017UL = 6,
        ROCHCORR2018UL = 7
    };

    static MuonEnergyCorrection ToMuonEnergyCorrection(std::string const &muonEnergyCorrection);

protected:

    // Can be overwritten for analysis-specific use cases
    virtual void AdditionalCorrections(KMuon *muon, KappaEvent const &event,
                                       KappaProduct &product, KappaSettings const &settings) const;

private:
    MuonEnergyCorrection muonEnergyCorrection = MuonEnergyCorrection::NONE;
    std::unique_ptr<rochcor2015> rmcor2015 = nullptr;
    std::unique_ptr<RoccoR> rmcor = nullptr;
    std::unique_ptr<TRandom3> random = nullptr;
};
