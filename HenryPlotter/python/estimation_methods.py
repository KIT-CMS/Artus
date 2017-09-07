
# -*- coding: utf-8 -*-
import ROOT
from Artus.HenryPlotter.histogram import *
from Artus.HenryPlotter.cutstring import *

#base class for an estimation methd
class Estimation_method(object):

	def get_folder(self, systematic, folder):
		return systematic.channel + "_" + folder + "/ntuple"

	def get_name(self):
		return self.name

	def get_weights(self):
		return self.weights

	def __init__(self, name, folder, weights):
		self.folder = folder
		self.weights = weights
		self.name = name


	# function parsing the datasets helper to return the files
	# overwrite this function
	def get_files(self):
		return [""]

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
	def get_histograms(self, systematic):
		histogram_settings = []
		histogram_settings.append({     "name" : systematic.get_name, 
						"inputfiles" : self.get_files,
						"folder" : [self.get_folder, systematic, self.folder],
						"cuts" : systematic.get_cuts,
						"weights" : self.get_weights,
	        	            		"variable" : systematic.category.get_variable,
						"nbins" : systematic.category.get_nbins, "xlow" : systematic.category.get_xlow, "xhigh" : systematic.category.get_xhigh})

		# execute the underlying functions
		histogram_settings = self.apply_systematic_variations(systematic, histogram_settings)
		for setting in histogram_settings:
			for key, value in setting.iteritems():
				if isinstance(value, list):
					print value
					setting[key] = value[0](*value[1:])
				else:
					setting[key] = value()

		histograms = []
		for setting in histogram_settings:
			histograms.append( Histogram(**setting))
		return histograms
		

	# doing nothing, shape is exactly the histogram as default
	def do_estimation(self, systematic):
		 return systematic.input_histograms[0]

class Data(Estimation_method):
	def get_files(self):
		return ["/home/friese/artus/2017-01-02_longRun/SingleMuon_Run2016B_PromptRecov2_13TeV_MINIAOD/SingleMuon_Run2016B_PromptRecov2_13TeV_MINIAOD.root"]

class Ztt(Estimation_method):

	def __init__(self):
		self.folder = "jecUncNom_tauEsNom"
		self.weights = Weights(Weight("zPtReweightWeight", "zPtReweightWeight"))
		self.name = "ZTT"

	def get_files(self):
		return ["/home/friese/artus/2017-01-02_longRun/DYJetsToLLM50_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8_ext1/DYJetsToLLM50_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8_ext1.root"]
