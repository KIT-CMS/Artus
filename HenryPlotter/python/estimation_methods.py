
# -*- coding: utf-8 -*-
import ROOT
import copy
from Artus.HenryPlotter.histogram import *
from Artus.HenryPlotter.cutstring import *

#base class for an estimation methd
class Estimation_method(object):

	def get_folder(self, systematic, folder):
		return systematic.channel + "_" + folder + "/ntuple"

	def get_name(self):
		return self.name

	@staticmethod
	def get_weights():
		return Weights(Weight("1.0", "constant"))

	def __init__(self, name, folder):
		self.folder = folder
		self.name = name


	# function parsing the datasets helper to return the files
	# overwrite this function
	def get_files():
		raise NotImplementedError

	# wrapper function for the Histogram creation performing the systematic shifts
	def apply_systematic_variations(self, systematic, histogram_settings):
		if systematic.syst !=None:
			new_settings = []
			for h_setting in histogram_settings:
				new_settings += systematic.syst.shifted_histograms(h_setting)
			return new_settings
		else:
			return histogram_settings

	# return a list of the histogram class. In the base class just reads out the signal region
	def define_histograms(self, systematic):
		histogram_settings = []
		histogram_settings.append({     "name" : systematic.get_name, 
						"inputfiles" : self.get_files,
						"folder" : [self.get_folder, systematic, self.folder],
						"cuts" : systematic.get_category().get_cuts,
						"weights" : self.get_weights,
	        	            		"variable" : systematic.category.get_variable,
						"nbins" : systematic.category.get_nbins, "xlow" : systematic.category.get_xlow, "xhigh" : systematic.category.get_xhigh})
		return histogram_settings

	def get_histograms(self, systematic):
		histogram_settings = self.define_histograms(systematic)

		# execute the underlying functions
		histogram_settings = self.apply_systematic_variations(systematic, histogram_settings)
		for setting in histogram_settings:
			for key, value in setting.iteritems():
				print key, value
				if isinstance(value, list):
					print value
					setting[key] = value[0](*value[1:])
				elif callable(value):
					setting[key] = value()

		histograms = []
		import pprint
		pprint.pprint(histogram_settings)
		for setting in histogram_settings:
			histograms.append( Histogram(**setting))
		return histograms
		

	# doing nothing, shape is exactly the histogram as default
	def do_estimation(self, systematic):
		 return systematic.input_histograms[0]

class Data(Estimation_method):
	@staticmethod
	def get_files():
		return ["/home/friese/artus/2017-01-02_longRun/SingleMuon_Run2016B_PromptRecov2_13TeV_MINIAOD/SingleMuon_Run2016B_PromptRecov2_13TeV_MINIAOD.root"]

class Ztt(Estimation_method):

	def __init__(self):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "ZTT"

	@staticmethod
	def get_weights():
		return Weights(Weight("zPtReweightWeight", "zPtReweightWeight"))

	@staticmethod
	def get_files():
		return ["/home/friese/artus/2017-01-02_longRun/DYJetsToLLM50_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8_ext1/DYJetsToLLM50_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8_ext1.root"]

class Wj(Estimation_method):

	def __init__(self):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "WJ"

	def define_histograms(self, systematic):
		# wj signal region
		histogram_settings = super(Wj, self).define_histograms(systematic)
		# high-mt os
		high_mt_os_cuts =  copy.deepcopy(systematic.get_category().get_cuts())
		high_mt_os_cuts.get("mt").invert().set_value(70)
		common_settings = {		"folder" : [self.get_folder, systematic, self.folder],
						"cuts" : high_mt_os_cuts,
	        	            		"variable" : systematic.category.get_variable,
						"nbins" : systematic.category.get_nbins, "xlow" : systematic.category.get_xlow, "xhigh" : systematic.category.get_xhigh}

		# wj high-mt os
		histogram_settings.append(dict({     "name" : [systematic.get_name, "wj_high_mt_os"],
						"inputfiles" : Wj.get_files,
						"weights" : self.get_weights}.items() + common_settings.items()))
		histogram_settings.append(dict({     "name" : [systematic.get_name, "ztt_high_mt_os"],
						"inputfiles" : Ztt.get_files,
						"weights" : Ztt.get_weights}.items() + common_settings.items()))
		histogram_settings.append(dict({     "name" : [systematic.get_name, "data_high_mt_os"],
						"inputfiles" : Data.get_files,
						"weights" : Data.get_weights}.items() + common_settings.items()))

		# high-mt ss
		common_settings["cuts"] = copy.deepcopy(high_mt_os_cuts)
		common_settings["cuts"].get("os").invert()
		histogram_settings.append(dict({     "name" : [systematic.get_name, "wj_high_mt_ss"],
						"inputfiles" : Wj.get_files,
						"weights" : self.get_weights}.items() + common_settings.items()))
		histogram_settings.append(dict({     "name" : [systematic.get_name, "ztt_high_mt_ss"],
						"inputfiles" : Ztt.get_files,
						"weights" : Ztt.get_weights}.items() + common_settings.items()))
		histogram_settings.append(dict({     "name" : [systematic.get_name, "data_high_mt_ss"],
						"inputfiles" : Data.get_files,
						"weights" : Data.get_weights}.items() + common_settings.items()))
		import pprint
		pprint.pprint(histogram_settings)
		return histogram_settings

	def do_estimation(self, systematic):
		for k, v in systematic.input_histograms.iteritems():
			print k
			print v

	@staticmethod
	def get_files():
		return ["/home/friese/artus/2017-01-02_longRun/WJetsToLNu_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8/WJetsToLNu_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8.root"]

	@staticmethod
	def get_weights():
		return Weights(Weight("1.0", "constant"))
