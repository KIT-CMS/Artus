
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"
#include <boost/algorithm/string.hpp>


KappaEnumTypes::GenParticleType KappaEnumTypes::ToGenParticleType(std::string const& genParticleName)
{
	if (genParticleName == "genParticle") return KappaEnumTypes::GenParticleType::GENPARTICLE;
	else if (genParticleName == "genElectron") return KappaEnumTypes::GenParticleType::GENELECTRON;
	else if (genParticleName == "genMuon") return KappaEnumTypes::GenParticleType::GENMUON;
	else if (genParticleName == "genTau") return KappaEnumTypes::GenParticleType::GENTAU;
	else return KappaEnumTypes::GenParticleType::NONE;
}

KappaEnumTypes::DiLeptonDecayMode KappaEnumTypes::ToDiLeptonDecayMode(std::string const& diLeptonDecayMode)
{
	if (diLeptonDecayMode == "ee") return KappaEnumTypes::DiLeptonDecayMode::EE;
	else if (diLeptonDecayMode == "mm") return KappaEnumTypes::DiLeptonDecayMode::MM;
	else if (diLeptonDecayMode == "tt") return KappaEnumTypes::DiLeptonDecayMode::TT;
	else if (diLeptonDecayMode == "ll") return KappaEnumTypes::DiLeptonDecayMode::LL;
	return KappaEnumTypes::DiLeptonDecayMode::NONE;
}

KappaEnumTypes::ValidJetsInput KappaEnumTypes::ToValidJetsInput(std::string const& validJetsInput)
{
	std::string validJetsInput_lower = boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(validJetsInput));
	if (validJetsInput == "uncorrected") return KappaEnumTypes::ValidJetsInput::UNCORRECTED;
	else if (validJetsInput == "corrected") return KappaEnumTypes::ValidJetsInput::CORRECTED;
	else return KappaEnumTypes::ValidJetsInput::AUTO;
}

KappaEnumTypes::JetIDVersion KappaEnumTypes::ToJetIDVersion(std::string const& jetIDVersion)
{
	std::string jetIDVersionLower = boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(jetIDVersion));
	if (jetIDVersionLower == "2010") return KappaEnumTypes::JetIDVersion::ID2010;
	else if (jetIDVersionLower == "2014") return KappaEnumTypes::JetIDVersion::ID2014;
	else if (jetIDVersionLower == "73x") return KappaEnumTypes::JetIDVersion::ID73X;
	else if (jetIDVersionLower == "73xtemp") return KappaEnumTypes::JetIDVersion::ID73Xtemp;
	else if (jetIDVersionLower == "73xnohf") return KappaEnumTypes::JetIDVersion::ID73XnoHF;
	else if (jetIDVersionLower == "2015") return KappaEnumTypes::JetIDVersion::ID2015;
	else if (jetIDVersionLower == "2016") return KappaEnumTypes::JetIDVersion::ID2016;
	else if (jetIDVersionLower == "2017") return KappaEnumTypes::JetIDVersion::ID2017;
	else if (jetIDVersionLower == "2018") return KappaEnumTypes::JetIDVersion::ID2018;
	else if ((jetIDVersionLower == "2016ul") || (jetIDVersionLower == "ul2016")) return KappaEnumTypes::JetIDVersion::ID2016UL;
	else if ((jetIDVersionLower == "2017ul") || (jetIDVersionLower == "ul2017")) return KappaEnumTypes::JetIDVersion::ID2017UL;
	else if ((jetIDVersionLower == "2018ul") || (jetIDVersionLower == "ul2018")) return KappaEnumTypes::JetIDVersion::ID2018UL;
	else if (jetIDVersionLower == "none") return KappaEnumTypes::JetIDVersion::NONE;
	else {
	    LOG(FATAL) << "[KappaEnumTypes] Jet ID version '" << jetIDVersion << "' is not available";
	    return KappaEnumTypes::JetIDVersion::NONE; // necessary but never happens!
	}
}

KappaEnumTypes::JetID KappaEnumTypes::ToJetID(std::string const& jetID)
{
	std::string jetIDLower = boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(jetID));
	if (jetIDLower == "loose") return KappaEnumTypes::JetID::LOOSE;
	else if (jetIDLower == "looselepveto") return KappaEnumTypes::JetID::LOOSELEPVETO;
	else if (jetIDLower == "medium") return KappaEnumTypes::JetID::MEDIUM;
	else if (jetIDLower == "tight") return KappaEnumTypes::JetID::TIGHT;
	else if (jetIDLower == "tightlepveto") return KappaEnumTypes::JetID::TIGHTLEPVETO;
	else if (jetIDLower == "none") return KappaEnumTypes::JetID::NONE;
	else {
	    LOG(FATAL) << "Jet ID of type '" << jetID << "' not implemented!";
	    return KappaEnumTypes::JetID::NONE; // necessary but never happens!
	}
}

KappaEnumTypes::BTagScaleFactorMethod KappaEnumTypes::ToBTagScaleFactorMethod(std::string const& bTagSFMethod)
{
	if (bTagSFMethod == "promotiondemotion") return KappaEnumTypes::BTagScaleFactorMethod::PROMOTIONDEMOTION;
	else if (bTagSFMethod == "other") return KappaEnumTypes::BTagScaleFactorMethod::OTHER;
	else return KappaEnumTypes::BTagScaleFactorMethod::NONE;
}

KappaEnumTypes::GenCollectionToPrint KappaEnumTypes::ToGenCollectionToPrint(std::string const& genCollectionToPrint)
{
	if (genCollectionToPrint == "all") return KappaEnumTypes::GenCollectionToPrint::ALL;
	else if (genCollectionToPrint == "gen") return KappaEnumTypes::GenCollectionToPrint::GEN;
	else if (genCollectionToPrint == "lhe") return KappaEnumTypes::GenCollectionToPrint::LHE;
	else return KappaEnumTypes::GenCollectionToPrint::NONE;
}

