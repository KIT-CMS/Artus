#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"


std::map<std::string, std::function<bool(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonBoolQuantities
	= std::map<std::string, std::function<bool(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();

std::map<std::string, std::function<int(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonIntQuantities
	= std::map<std::string, std::function<int(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();

std::map<std::string, std::function<uint64_t(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonUInt64Quantities
	= std::map<std::string, std::function<uint64_t(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();

std::map<std::string, std::function<float(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonFloatQuantities
	= std::map<std::string, std::function<float(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();

std::map<std::string, std::function<double(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonDoubleQuantities
	= std::map<std::string, std::function<double(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();

std::map<std::string, std::function<ROOT::Math::PtEtaPhiMVector(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonPtEtaPhiMVectorQuantities
	= std::map<std::string, std::function<ROOT::Math::PtEtaPhiMVector(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();

std::map<std::string, std::function<RMFLV(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonRMFLVQuantities
	= std::map<std::string, std::function<RMFLV(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();

std::map<std::string, std::function<std::string(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonStringQuantities
	= std::map<std::string, std::function<std::string(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();

std::map<std::string, std::function<std::vector<double>(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonVDoubleQuantities
	= std::map<std::string, std::function<std::vector<double>(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();

std::map<std::string, std::function<std::vector<float>(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonVFloatQuantities
	= std::map<std::string, std::function<std::vector<float>(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();

std::map<std::string, std::function<std::vector<RMFLV>(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonVRMFLVQuantities
	= std::map<std::string, std::function<std::vector<RMFLV>(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();

std::map<std::string, std::function<std::vector<std::string>(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonVStringQuantities
	= std::map<std::string, std::function<std::vector<std::string>(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();

std::map<std::string, std::function<std::vector<int>(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >> LambdaNtupleQuantities::CommonVIntQuantities
	= std::map<std::string, std::function<std::vector<int>(EventBase const& ev, ProductBase const& pd, SettingsBase const& settings) >>();



