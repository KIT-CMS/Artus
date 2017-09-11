
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

	@staticmethod
	def get_cuts():
		return Cuts()

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
						"cuts" : systematic.get_category().get_cuts() + self.get_cuts(),
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
	def do_estimation(self, systematic, root_objects):
		if(len(systematic.input_root_objects.keys())) == 1:
			return systematic.input_root_objects.values()[0]

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
	def get_cuts():
		return Cuts(Cut("gen_match_2==5", "ztt_genmatch_mt"))

	@staticmethod
	def get_files():
		return ["/home/friese/artus/2017-01-02_longRun/DYJetsToLLM50_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8_ext1/DYJetsToLLM50_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8_ext1.root"]

class Zll(Ztt):

	def __init__(self):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "Zll"

	@staticmethod
	def get_cuts():
		return Cuts(Cut("(gen_match_2<5||gen_match_2==6)", "zll_genmatch_mt"))


class Wj(Estimation_method):

	def __init__(self):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "WJ"


	def get_root_objects(self, systematic):
		root_objects = super(Wj, self).get_root_objects(systematic)

		highmt_os_category = copy.deepcopy(systematic.get_category())
		highmt_os_category.get_cuts().get("mt").invert().set_value(70)
		highmt_os_category.name = "high_mt_os"

		highmt_ss_category = copy.deepcopy(systematic.get_category())
		highmt_ss_category.get_cuts().get("mt").invert().set_value(70)
		highmt_ss_category.get_cuts().get("os").invert().name = "ss"
		highmt_ss_category.name = "high_mt_ss"


		for category in [highmt_os_category, highmt_ss_category]:
				self.data_estimation = Data("data", folder="jecUncNom_tauEsNom")
				self.data = Systematic(category=category, process="data", channel="mt", analysis = "example", era="2017", mass=None, syst=None, estimation_method = self.data_estimation)
				root_objects += self.data_estimation.get_root_objects(self.data)

				self.ztt_estimation = Ztt()
				self.ztt  = Systematic(category=category, process="ztt",  channel="mt", analysis = "example", era="2017", mass=None, syst=None, estimation_method = self.ztt_estimation)
				root_objects += self.ztt_estimation.get_root_objects(self.ztt)

		print "root o"
		for r in root_objects:
			print r.get_name(), r.result
		return root_objects

	def define_root_objects(self, systematic):
		# wj signal region
		histogram_settings = super(Wj, self).define_root_objects(systematic)
		return histogram_settings

	def do_estimation(self, systematic, root_objects):
		return

	@staticmethod
	def get_files():
		return ["/home/friese/artus/2017-01-02_longRun/WJetsToLNu_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8/WJetsToLNu_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8.root"]

	@staticmethod
	def get_weights():
		return Weights(Weight("1.0", "constant"))


class TT(Estimation_method):

	def __init__(self):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "TT"

	@staticmethod
	def get_weights():
		return Weights(Weight("topPtReweightWeight", "topPtReweightWeight"))

	@staticmethod
	def get_files():
		return ["/home/friese/artus/2017-01-02_longRun/TT_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_powheg-pythia8_ext4/TT_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_powheg-pythia8_ext4.root"]

class VV(Estimation_method):

	def __init__(self):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "VV"

	@staticmethod
	def get_weights():
		return Weights()

	@staticmethod
	def get_files():
		path = "/home/friese/artus/2017-01-02_longRun/"
		files = ["WZTo1L1Nu2Q_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_amcatnlo-pythia8"]
		files.append("WZTo1L3Nu_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_amcatnlo-pythia8")
		files.append( "WZTo2L2Q_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_amcatnlo-pythia8")
		files.append("ZZTo2L2Q_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_amcatnlo-pythia8")
		files.append("WWTo1L1Nu2Q_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_amcatnlo-pythia8")

		return [ path + f + "/" + f + ".root" for f in files]

class QCD(Estimation_method):

	def __init__(self):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "QCD"


	def get_root_objects(self, systematic):
		ss_category = copy.deepcopy(systematic.get_category())
		ss_category.get_cuts().get("os").invert().name = "ss"
		ss_category.name = "ss"

		root_objects = []
		self.data_estimation = Data("data", folder="jecUncNom_tauEsNom")
		self.data = Systematic(category=ss_category, process="data", channel="mt", analysis = "example", era="2017", mass=None, syst=None, estimation_method = self.data_estimation)
		root_objects += self.data_estimation.get_root_objects(self.data)

		self.ztt_estimation = Ztt()
		self.ztt  = Systematic(category=ss_category, process="ztt",  channel="mt", analysis = "example", era="2017", mass=None, syst=None, estimation_method = self.ztt_estimation)
		root_objects += self.ztt_estimation.get_root_objects(self.ztt)

		self.zll_estimation = Zll()
		self.zll  = Systematic(category=ss_category, process="zll",  channel="mt", analysis = "example", era="2017", mass=None, syst=None, estimation_method = self.zll_estimation)
		root_objects += self.zll_estimation.get_root_objects(self.zll)

		self.tt_estimation = TT()
		self.tt  = Systematic(category=ss_category, process="tt",  channel="mt", analysis = "example", era="2017", mass=None, syst=None, estimation_method = self.tt_estimation)
		root_objects += self.tt_estimation.get_root_objects(self.tt)

		self.vv_estimation = VV()
		self.vv  = Systematic(category=ss_category, process="vv",  channel="mt", analysis = "example", era="2017", mass=None, syst=None, estimation_method = self.vv_estimation)
		root_objects += self.vv_estimation.get_root_objects(self.vv)

		self.data_hist = root_objects[0]
		self.mc_hist = root_objects[1:]
		print "root o"
		for r in root_objects:
			print r.get_name(), r.result
		return root_objects


	def do_estimation(self, systematic, root_objects):
		result_histo = self.data_hist
		for h in self.mc_hist:
			result_histo.result.Add(h.result, -1.0)
		result_histo.set_name(systematic.get_name())
		result_histo.save(root_objects.output_tree)


	@staticmethod
	def get_files():
		return ["/home/friese/artus/2017-01-02_longRun/WJetsToLNu_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8/WJetsToLNu_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8.root"]

	@staticmethod
	def get_weights():
		return Weights(Weight("1.0", "constant"))
