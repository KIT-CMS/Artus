
# -*- coding: utf-8 -*-
import copy

import os

from Artus.HenryPlotter.histogram import *
from Artus.HenryPlotter.cutstring import *
from Artus.HenryPlotter.systematics import *
from Artus.HenryPlotter.systematic_variations import *

import logging
logger = logging.getLogger(__name__)

#base class for an estimation methd
class Estimation_method(object):


	def get_folder(self, systematic, folder):
		return systematic.channel.get_name() + "_" + folder + "/ntuple"

	def get_name(self):
		return self.name

	@staticmethod
	def get_weights():
		return Weights(Weight("1.0", "constant"))

	@staticmethod
	def get_cuts():
		return Cuts()

	def __init__(self, name, folder, directory):
		self.directory = directory
		self.folder = folder
		self.name = name
		self.campaign = "RunIISummer16MiniAODv2"


	# function parsing the datasets helper to return the files
	# overwrite this function
	def get_files():
		raise NotImplementedError

	def artus_file_names(self, files):
		return [os.path.join(self.directory, f, "%s.root"%f) for f in files]

	# wrapper function for the Histogram creation performing the systematic shifts
	def apply_systematic_variations(self, systematic, settings):
		print systematic.syst_var
		return systematic.syst_var.shifted_root_objects(settings)

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

	def create_root_objects(self, systematic):
		root_object_settings = self.define_root_objects(systematic)

		# execute the underlying functions
		root_object_settings = self.apply_systematic_variations(systematic, root_object_settings)
		for setting in root_object_settings:
			for key, value in setting.iteritems():
				if isinstance(value, list):
					setting[key] = value[0](*value[1:])
				elif callable(value):
					setting[key] = value()

		self.root_objects = []
		#import pprint
		#pprint.pprint(root_object_settings)
		for setting in root_object_settings:
			self.root_objects.append( create_root_object(**setting))
		return self

	def get_root_objects(self):
		return self.root_objects
		

	# doing nothing, shape is exactly the histogram as default
	def do_estimation(self, systematic, root_objects):
		logger.debug("Calling do_estimation for the estimation method %s. ", self.get_name())
		if len(self.get_root_objects()) == 0:
			logger.warning("No histogram associated to %s with name %s. Using the first one as the result", self, self.get_name())
			raise Exception
		elif len(self.get_root_objects()) > 1:
			logger.warning("There is not exactly one histogram associated to %s with name %s. Using the first one as the result", self, self.get_name())
		print self.get_root_objects()[0]
		return self.get_root_objects()[0]
		

class Data_estimation(Estimation_method):
	def __init__(self, channel, era, directory):
		self.directory = directory
		self.name = "data"
		self.folder = "nominal"
		self.channel = channel
		self.era = era

	def get_files(self):
		return self.artus_file_names(self.era.data_files(self.channel))

	def get_cuts(self):
		return Cuts()

class Ztt_estimation(Estimation_method):

	def __init__(self, era, directory):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "ZTT"
		self.era = era

	def get_weights(self):
		return Weights(Weight("zPtReweightWeight", "zPtReweightWeight"), self.era.get_lumi_weight())

	@staticmethod
	def get_cuts():
		return Cuts(Cut("gen_match_2==5", "ztt_genmatch_mt"))

	@staticmethod
	def get_files():
		return ["/home/friese/artus/2017-01-02_longRun/DYJetsToLLM50_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8_ext1/DYJetsToLLM50_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8_ext1.root"]

class Zll_estimation(Ztt_estimation):

	def __init__(self, directory):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "Zll"

	@staticmethod
	def get_cuts():
		return Cuts(Cut("(gen_match_2<5||gen_match_2==6)", "zll_genmatch_mt"))

class Wj_estimation(Estimation_method):

	def __init__(self, directory):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "WJ"

	def get_weights():
		return Weights()

	@staticmethod
	def get_files():
		return ["/home/friese/artus/2017-01-02_longRun/WJetsToLNu_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8/WJetsToLNu_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_madgraph-pythia8.root"]


class Wj_from_SS_OS_estimation(Wj_estimation):

	def __init__(self, directory):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "WJ"


	def get_root_objects(self, systematic):
		root_objects = super(Wj, self).get_root_objects(systematic)
		self.wj_signal_shape = root_objects[0]
		print self.wj_signal_shape
		print "------------------------------------------"
		print "hier", root_objects

		self.wj_ss_os_category = copy.deepcopy(systematic.get_category())
		self.wj_ss_os_category.get_cuts().remove("os").get("mt").invert().set_value(70)
		self.wj_ss_os_category.variable = "q_1*q_2"
		self.wj_ss_os_category.nbins = 2
		self.wj_ss_os_category.xlow= -1.1
		self.wj_ss_os_category.xhigh= 1.1
		self.wj_ss_os_category.name = "inclusive"
		self.wj_ss_os = Systematic(category=self.wj_ss_os_category, process="wj",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = Wj())
		root_objects += self.wj_ss_os.get_root_objects().values()


		self.highmt_os_category = copy.deepcopy(systematic.get_category())
		self.highmt_os_category.get_cuts().get("mt").invert().set_value(70)
		self.highmt_os_category.name = "high_mt_os"

		self.highmt_ss_category = copy.deepcopy(systematic.get_category())
		self.highmt_ss_category.get_cuts().get("mt").invert().set_value(70)
		self.highmt_ss_category.get_cuts().get("os").invert().name = "ss"
		self.highmt_ss_category.name = "high_mt_ss"
		
		for category in [self.highmt_os_category, self.highmt_ss_category]:
				c_name = category.get_name()
				setattr(self, c_name, [])
				print "test"
				print getattr(self, c_name)
				getattr(self, c_name).append(Systematic(category=category, process="data", channel="mt", analysis = "example", era="2017", mass=None, syst_var=Nominal(), estimation_method = Data("data", folder="jecUncNom_tauEsNom")))

				getattr(self, c_name).append(Systematic(category=category, process="ztt",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = Ztt()))
				getattr(self, c_name).append(Systematic(category=category, process="zll",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = Zll()))
				getattr(self, c_name).append(Systematic(category=category, process="tt",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = TT()))
				getattr(self, c_name).append(Systematic(category=category, process="vv",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = VV()))
				getattr(self, c_name).append(Systematic(category=category, process="wj",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = Wj()))

				for systematic in getattr(self, c_name):
					root_objects += systematic.get_root_objects().values()

		return root_objects

	def define_root_objects(self, systematic):
		# wj signal region
		histogram_settings = super(Wj, self).define_root_objects(systematic)
		return histogram_settings

	def do_estimation(self, systematic, root_objects):

		# call the estimation method of the individual estimations
		for category in [self.highmt_os_category, self.highmt_ss_category]:
				c_name = category.get_name()
				for systematic in getattr(self, c_name):
					systematic.do_estimation(root_objects)

		# N_Fake^OS
		n_fake_os = getattr(self, self.highmt_os_category.get_name())[0].shape.result.Integral() - sum([h.shape.result.Integral() for h in getattr(self, self.highmt_os_category.get_name())][1:-1])
		n_fake_ss = getattr(self, self.highmt_ss_category.get_name())[0].shape.result.Integral() - sum([h.shape.result.Integral() for h in getattr(self, self.highmt_ss_category.get_name())][1:-1])

		f_W_os_ss = getattr(self, self.highmt_os_category.get_name())[-1].shape.result.Integral() / getattr(self, self.highmt_ss_category.get_name())[-1].shape.result.Integral()
		print f_W_os_ss, "1st"

		self.wj_ss_os.do_estimation(root_objects)
		f_W_os_ss = self.wj_ss_os.shape.result.GetBinContent(1) / self.wj_ss_os.shape.result.GetBinContent(2)
		print f_W_os_ss, "2nd"

		# hard-coded number with no more motivation than "it works"
		f_QCD_os_ss = 1.2

		scale_factor = (n_fake_os - f_QCD_os_ss * n_fake_ss)/(1+f_QCD_os_ss/f_W_os_ss)

		print n_fake_os
		print n_fake_ss
		print f_W_os_ss
		print scale_factor

		f_W_high_low = getattr(self, self.highmt_os_category.get_name())[-1].shape.result.Integral()  / self.wj_signal_shape.result.Integral()
		#nothing happens atm with these scale factors
		return self.wj_signal_shape.result


	def get_weights():
		return Weights(Weight("1.0", "constant"))


class TT_estimation(Estimation_method):

	def __init__(self, directory):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "TT"

	def get_weights():
		return Weights(Weight("topPtReweightWeight", "topPtReweightWeight"))

	@staticmethod
	def get_files():
		return ["/home/friese/artus/2017-01-02_longRun/TT_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_powheg-pythia8_ext4/TT_RunIISpring16MiniAODv2_PUSpring16_13TeV_MINIAOD_powheg-pythia8_ext4.root"]

class VV_estimation(Estimation_method):

	def __init__(self, directory):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "VV"

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

class QCD_estimation(Estimation_method):

	def __init__(self, directory):
		self.folder = "jecUncNom_tauEsNom"
		self.name = "QCD"


	def get_root_objects(self, systematic):
		ss_category = copy.deepcopy(systematic.get_category())
		ss_category.get_cuts().get("os").invert().name = "ss"
		ss_category.name = "ss"

		root_objects = []
		self.data_estimation = Data("data", folder="jecUncNom_tauEsNom")
		custom_nom = Nominal(direction=systematic.syst_var.direction)
		custom_nom.name = systematic.syst_var.name
		self.data = Systematic(category=ss_category, process="data", channel="mt", analysis = "example", era="2017", mass=None, syst_var=custom_nom, estimation_method = self.data_estimation)
		root_objects += self.data_estimation.get_root_objects(self.data)

		self.ztt_estimation = Ztt()
		self.ztt  = Systematic(category=ss_category, process="ztt",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = self.ztt_estimation)
		root_objects += self.ztt_estimation.get_root_objects(self.ztt)

		self.zll_estimation = Zll()
		self.zll  = Systematic(category=ss_category, process="zll",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = self.zll_estimation)
		root_objects += self.zll_estimation.get_root_objects(self.zll)

		self.tt_estimation = TT()
		self.tt  = Systematic(category=ss_category, process="tt",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = self.tt_estimation)
		root_objects += self.tt_estimation.get_root_objects(self.tt)

		self.vv_estimation = VV()
		self.vv  = Systematic(category=ss_category, process="vv",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = self.vv_estimation)
		root_objects += self.vv_estimation.get_root_objects(self.vv)

		self.data_hist = root_objects[0]
		self.mc_hist = root_objects[1:]
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

	def get_weights():
		return Weights(Weight("1.0", "constant"))
