
#pragma once

#include "Artus/Utility/interface/ArtusLogging.h"


/**
*/
class KappaEnumTypes {

public:

	enum class GenParticleType : int
	{
		NONE = -1,
		GENPARTICLE  = 0,
		GENELECTRON = 1,
		GENMUON = 2,
		GENTAU = 3
	};
	static GenParticleType ToGenParticleType(std::string const& genParticleName);
	
	enum class TauTauDecayMode : int
	{
		NONE = -1,
		TT   = 1,
		MT   = 2,
		ET   = 3,
		MM   = 4,
		EM   = 5,
		EE   = 6
	};

	enum class DiLeptonDecayMode : int
	{
		NONE = -1,
		EE = 0,
		MM = 1,
		TT = 2,
		LL = 3,
	};
	static DiLeptonDecayMode ToDiLeptonDecayMode(std::string const& diLeptonDecayMode);

	enum class GenMatchingCode : int
	{
		NONE             = -1,
		IS_ELE_PROMPT    = 1,
		IS_MUON_PROMPT   = 2,
		IS_ELE_FROM_TAU  = 3,
		IS_MUON_FROM_TAU = 4,
		IS_TAU_HAD_DECAY = 5,
		IS_FAKE          = 6
	};

	enum class ValidJetsInput : int
	{
		AUTO = 0,
		UNCORRECTED = 1,
		CORRECTED = 2,
	};
	static ValidJetsInput ToValidJetsInput(std::string const& validJetsInput);

	enum class JetIDVersion : int
	{
		ID2010 = 0,  // old run1 version (most run 1 analyses)
		ID2014 = 1,  // new run1 version (run 1 legacy: old version + muon fraction cut)
		             // first run 2 version identical to run 1 legacy version
		ID73X = 3,   // new run 2 version identical to ID2014 but change in cmssw 7.3.x fraction definitions
		ID73Xtemp = 4, // temporary recommendation for first run 2 events due to ID problems in the forward region
		ID73XnoHF = 5, // as temp but invalidate forward jets
		ID2015 = 6,  // new jet ID for run 2 updated on 2015-09-11
		ID2016 = 7,  // new jet ID for 2016 updated on 2017-03-24
		ID2017 = 8,  // new jet ID for 2017 updated on 2018-02-14
	};
	static JetIDVersion ToJetIDVersion(std::string const& jetIDVersion);

	enum class JetID : int
	{
		NONE,
		LOOSE,
		LOOSELEPVETO,
		MEDIUM,
		TIGHT,
		TIGHTLEPVETO
	};
	static JetID ToJetID(std::string const& jetID);

	enum class BTagScaleFactorMethod : int
	{
		NONE = -1,
		PROMOTIONDEMOTION = 0,
		OTHER = 1,
	};
	static BTagScaleFactorMethod ToBTagScaleFactorMethod(std::string const& bTagSFMethod);

	enum class GenCollectionToPrint : int
	{
		NONE = -1,
		ALL = 0,
		GEN = 1,
		LHE = 2,
	};
	static GenCollectionToPrint ToGenCollectionToPrint(std::string const& genCollectionToPrint);

	enum class JetEnergyUncertaintyShiftName : int
	{
		NONE,
		AbsoluteFlavMap,
		AbsoluteMPFBias,
		AbsoluteScale,
		AbsoluteStat,
		FlavorQCD,
		Fragmentation,
		PileUpDataMC,
		PileUpPtBB,
		PileUpPtEC1,
		PileUpPtEC2,
		PileUpPtHF,
		PileUpPtRef,
		RelativeBal,
		RelativeSample,
		RelativeFSR,
		RelativeJEREC1,
		RelativeJEREC2,
		RelativeJERHF,
		RelativePtBB,
		RelativePtEC1,
		RelativePtEC2,
		RelativePtHF,
		RelativeStatEC,
		RelativeStatFSR,
		RelativeStatHF,
		SinglePionECAL,
		SinglePionHCAL,
		TimePtEta,
		Total,
		Closure // individual uncertainties added in quadrature. to be compared to 'Total' for closure test
	};

	static JetEnergyUncertaintyShiftName ToJetEnergyUncertaintyShiftName(std::string const& jetEnergyCorrectionUncertainty)
	{
		if (jetEnergyCorrectionUncertainty == "AbsoluteFlavMap") return JetEnergyUncertaintyShiftName::AbsoluteFlavMap;
		else if (jetEnergyCorrectionUncertainty == "AbsoluteMPFBias") return JetEnergyUncertaintyShiftName::AbsoluteMPFBias;
		else if (jetEnergyCorrectionUncertainty == "AbsoluteScale") return JetEnergyUncertaintyShiftName::AbsoluteScale;
		else if (jetEnergyCorrectionUncertainty == "AbsoluteStat") return JetEnergyUncertaintyShiftName::AbsoluteStat;
		else if (jetEnergyCorrectionUncertainty == "FlavorQCD") return JetEnergyUncertaintyShiftName::FlavorQCD;
		else if (jetEnergyCorrectionUncertainty == "Fragmentation") return JetEnergyUncertaintyShiftName::Fragmentation;
		else if (jetEnergyCorrectionUncertainty == "PileUpDataMC") return JetEnergyUncertaintyShiftName::PileUpDataMC;
		else if (jetEnergyCorrectionUncertainty == "PileUpPtBB") return JetEnergyUncertaintyShiftName::PileUpPtBB;
		else if (jetEnergyCorrectionUncertainty == "PileUpPtEC1") return JetEnergyUncertaintyShiftName::PileUpPtEC1;
		else if (jetEnergyCorrectionUncertainty == "PileUpPtEC2") return JetEnergyUncertaintyShiftName::PileUpPtEC2;
		else if (jetEnergyCorrectionUncertainty == "PileUpPtHF") return JetEnergyUncertaintyShiftName::PileUpPtHF;
		else if (jetEnergyCorrectionUncertainty == "PileUpPtRef") return JetEnergyUncertaintyShiftName::PileUpPtRef;
		else if (jetEnergyCorrectionUncertainty == "RelativeBal") return JetEnergyUncertaintyShiftName::RelativeBal;
		else if (jetEnergyCorrectionUncertainty == "RelativeSample") return JetEnergyUncertaintyShiftName::RelativeSample;
		else if (jetEnergyCorrectionUncertainty == "RelativeFSR") return JetEnergyUncertaintyShiftName::RelativeFSR;
		else if (jetEnergyCorrectionUncertainty == "RelativeJEREC1") return JetEnergyUncertaintyShiftName::RelativeJEREC1;
		else if (jetEnergyCorrectionUncertainty == "RelativeJEREC2") return JetEnergyUncertaintyShiftName::RelativeJEREC2;
		else if (jetEnergyCorrectionUncertainty == "RelativeJERHF") return JetEnergyUncertaintyShiftName::RelativeJERHF;
		else if (jetEnergyCorrectionUncertainty == "RelativePtBB") return JetEnergyUncertaintyShiftName::RelativePtBB;
		else if (jetEnergyCorrectionUncertainty == "RelativePtEC1") return JetEnergyUncertaintyShiftName::RelativePtEC1;
		else if (jetEnergyCorrectionUncertainty == "RelativePtEC2") return JetEnergyUncertaintyShiftName::RelativePtEC2;
		else if (jetEnergyCorrectionUncertainty == "RelativePtHF") return JetEnergyUncertaintyShiftName::RelativePtHF;
		else if (jetEnergyCorrectionUncertainty == "RelativeStatEC") return JetEnergyUncertaintyShiftName::RelativeStatEC;
		else if (jetEnergyCorrectionUncertainty == "RelativeStatFSR") return JetEnergyUncertaintyShiftName::RelativeStatFSR;
		else if (jetEnergyCorrectionUncertainty == "RelativeStatHF") return JetEnergyUncertaintyShiftName::RelativeStatHF;
		else if (jetEnergyCorrectionUncertainty == "SinglePionECAL") return JetEnergyUncertaintyShiftName::SinglePionECAL;
		else if (jetEnergyCorrectionUncertainty == "SinglePionHCAL") return JetEnergyUncertaintyShiftName::SinglePionHCAL;
		else if (jetEnergyCorrectionUncertainty == "TimePtEta") return JetEnergyUncertaintyShiftName::TimePtEta;
		else if (jetEnergyCorrectionUncertainty == "Total") return JetEnergyUncertaintyShiftName::Total;
		else if (jetEnergyCorrectionUncertainty == "Closure") return JetEnergyUncertaintyShiftName::Closure;
		else return JetEnergyUncertaintyShiftName::NONE;
	}
};

