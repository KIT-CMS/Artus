
# -*- coding: utf-8 -*-
import ROOT
import copy
from Artus.HenryPlotter.histogram import *
from Artus.HenryPlotter.cutstring import *
from Artus.HenryPlotter.systematics import *

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
	def apply_systematic_variations(self, systematic, settings):
		print "apply"
		print systematic.get_name(), systematic.syst
		print "end"
		if (systematic.syst):
			new_settings = []
			for setting in settings:
				new_settings += systematic.syst.shifted_root_objects(setting)
			return new_settings
		else:
			return settings

	def define_root_objects(self, systematic):
		histogram_settings = []
		histogram_settings.append({     "name" : systematic.get_name, 
						"inputfiles" : self.get_files,
						"folder" : [self.get_folder, systematic, self.folder],
						"cuts" : systematic.get_category().get_cuts,
						"weights" : self.get_weights,
	        	            		"variable" : systematic.category.get_variable,
						"nbins" : systematic.category.get_nbins, "xlow" : systematic.category.get_xlow, "xhigh" : systematic.category.get_xhigh})
		return histogram_settings

	def get_root_objects(self, systematic):
		root_object_settings = self.define_root_objects(systematic)

		# execute the underlying functions
		root_object_settings = self.apply_systematic_variations(systematic, root_object_settings)
		for setting in root_object_settings:
			for key, value in setting.iteritems():
				if isinstance(value, list):
					setting[key] = value[0](*value[1:])
				elif callable(value):
					setting[key] = value()

		root_objects = []
		#import pprint
		#pprint.pprint(root_object_settings)
		for setting in root_object_settings:
			root_objects.append( create_root_object(**setting))
		return root_objects
		

	# doing nothing, shape is exactly the histogram as default
	def do_estimation(self, systematic):
		 return systematic.input_root_objects[0]

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


	def get_root_objects(self, systematic):
		self.data_estimation = Data("data", folder="jecUncNom_tauEsNom")
		highmt_category = copy.deepcopy(systematic.get_category())
		highmt_category.get_cuts().get("mt").invert().set_value(70)
		highmt_category.name = "high_mt_os"
		self.data = Systematic(category=highmt_category, process="data", channel="mt", analysis = "boaexample", era="2017", mass=None, syst=None, estimation_method = self.data_estimation)
		#self.data_systematic = copy.deepcopy(systematic)
		#self.data_systematic.syst = None
		root_objects = super(Wj, self).get_root_objects(systematic)
		root_objects += self.data_estimation.get_root_objects(self.data)
		print "root o"
		for r in root_objects:
			print r.get_name(), r.result
		return root_objects

	def define_root_objects(self, systematic):
		# wj signal region
		histogram_settings = super(Wj, self).define_root_objects(systematic)
		# high-mt os
#		high_mt_os_cuts =  copy.deepcopy(systematic.get_category().get_cuts())
#		high_mt_os_cuts.get("mt").invert().set_value(70)
#		common_settings = {		"folder" : [self.get_folder, systematic, self.folder],
#						"cuts" : high_mt_os_cuts}
#	        	            		"variable" : systematic.category.get_variable}
#						"nbins" : systematic.category.get_nbins, "xlow" : systematic.category.get_xlow, "xhigh" : systematic.category.get_xhigh}

		# wj high-mt os
#		histogram_settings.append(dict({     "name" : [systematic.get_name, "wj_high_mt_os"],
#						"inputfiles" : Wj.get_files,
#						"weights" : self.get_weights}.items() + common_settings.items()))
#		histogram_settings.append(dict({     "name" : [systematic.get_name, "ztt_high_mt_os"],
#						"inputfiles" : Ztt.get_files,
#						"weights" : Ztt.get_weights}.items() + common_settings.items()))
#		histogram_settings.append(dict({     "name" : [systematic.get_name, "data_high_mt_os"],
#						"inputfiles" : Data.get_files,
#						"weights" : Data.get_weights}.items() + common_settings.items()))

		# high-mt ss
#		common_settings["cuts"] = copy.deepcopy(high_mt_os_cuts)
#		common_settings["cuts"].get("os").invert()
#		histogram_settings.append(dict({     "name" : [systematic.get_name, "wj_high_mt_ss"],
#						"inputfiles" : Wj.get_files,
#						"weights" : self.get_weights}.items() + common_settings.items()))
#		histogram_settings.append(dict({     "name" : [systematic.get_name, "ztt_high_mt_ss"],
#						"inputfiles" : Ztt.get_files,
#						"weights" : Ztt.get_weights}.items() + common_settings.items()))
#		histogram_settings.append(dict({     "name" : [systematic.get_name, "data_high_mt_ss"],
#						"inputfiles" : Data.get_files,
#						"weights" : Data.get_weights}.items() + common_settings.items()))
		#import pprint
		#pprint.pprint(histogram_settings)
		return histogram_settings

	def do_estimation(self, systematic):
		for k, v in systematic.input_root_objects.iteritems():
			print k
			print v.result
			print v

	@staticmethod
	def get_files():
		return ["/home/friese/artus/2017-01-02_longRun/WJetsToLNu_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8/WJetsToLNu_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8.root"]

	@staticmethod
	def get_weights():
		return Weights(Weight("1.0", "constant"))
