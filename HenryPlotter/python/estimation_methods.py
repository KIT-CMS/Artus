
# -*- coding: utf-8 -*-
import copy

import os

from Artus.HenryPlotter.histogram import *
from Artus.HenryPlotter.cutstring import *
from Artus.HenryPlotter.systematics import *
from Artus.HenryPlotter.systematic_variations import *

from  Kappa.Skimming.datasetsHelperTwopz import datasetsHelperTwopz
datasetsHelper = datasetsHelperTwopz(os.path.join(os.environ.get("CMSSW_BASE"),"src/Kappa/Skimming/data/datasets.json"))

import logging
logger = logging.getLogger(__name__)

#base class for an estimation methd
class Estimation_method(object):


	def get_folder(self, systematic, folder):
		return systematic.category.get_channel_name() + "_" + folder + "/ntuple"

	def get_name(self):
		return self.name

	@staticmethod
	def get_weights():
		return Weights(Weight("1.0", "constant"))

	@staticmethod
	def get_cuts():
		return Cuts()

	def __init__(self, name, folder, era, directory, channel):
		self.directory = directory
		self.folder = folder
		self.name = name
		self.mc_campaign = "RunIISummer16MiniAODv2"
		self.channel = channel
		self.era = era


	# function parsing the datasets helper to return the files
	# overwrite this function
	def get_files():
		raise NotImplementedError

	def artus_file_names(self, files):
		return [os.path.join(self.directory, f, "%s.root"%f) for f in files]

	# wrapper function for the Histogram creation performing the systematic shifts
	def apply_systematic_variations(self, systematic, settings):
		return systematic.syst_var.shifted_root_objects(settings)

	def define_root_objects(self, systematic):
		histogram_settings = []
		histogram_settings.append({     "name" : systematic.get_name, 
						"inputfiles" : self.get_files,
						"folder" : [self.get_folder, systematic, self.folder],
						"cuts" : systematic.get_category().get_cuts() + self.get_cuts(),
						"weights" : self.get_weights,
       	        		"variable" : systematic.category.get_variable})
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
		return self.get_root_objects()[0]

	def log_query(self, query, files):
		logger.debug("Query from get_files() method in %s ,directory %s", self.get_name(), self.directory)
		logger.debug("Query : %s", query)
		logger.debug("Files: %s", files)
		

class Data_estimation(Estimation_method):
	def __init__(self, era, directory, channel):
		super(Data_estimation, self).__init__("data_obs", "nominal", era, directory, channel)
		self.channel = channel

	def get_files(self):
		return self.artus_file_names(self.era.data_files(self.channel))

	def get_cuts(self):
		return Cuts()

class Ztt_estimation(Estimation_method):

	def __init__(self, era, directory, channel):
		super(Ztt_estimation, self).__init__("ZTT", "nominal", era, directory, channel)

	def get_weights(self):
		return Weights(Weight("eventWeight", "eventWeight"),
		               Weight("zPtReweightWeight", "zPtReweightWeight"),
		               Weight("((gen_match_2 == 5)*0.95 + (gen_match_2 != 5))", "hadronic_tau_sf"),
		               Weight("((((genbosonmass >= 150.0 && (npartons == 0 || npartons >= 5))*3.95423374e-5) + ((genbosonmass >= 150.0 && npartons == 1)*1.27486147e-5) + ((genbosonmass >= 150.0 && npartons == 2)*1.3012785e-5) + ((genbosonmass >= 150.0 && npartons == 3)*1.33802133e-5) + ((genbosonmass >= 150.0 && npartons == 4)*1.09698723e-5)+((genbosonmass >= 50.0 && genbosonmass < 150.0 && (npartons == 0 || npartons >= 5))*3.95423374e-5) + ((genbosonmass >= 50.0 && genbosonmass < 150.0 && npartons == 1)*1.27486147e-5) + ((genbosonmass >= 50.0 && genbosonmass < 150.0 && npartons == 2)*1.3012785e-5) + ((genbosonmass >= 50.0 && genbosonmass < 150.0 && npartons == 3)*1.33802133e-5) + ((genbosonmass >= 50.0 && genbosonmass < 150.0 && npartons == 4)*1.09698723e-5)+((genbosonmass < 50.0)*numberGeneratedEventsWeight*crossSectionPerEventWeight))/(numberGeneratedEventsWeight*crossSectionPerEventWeight*sampleStitchingWeight))", "z_stitching_weight"),
		               self.era.get_lumi_weight())

	@staticmethod
	def get_cuts():
		return Cuts(Cut("gen_match_2==5", "ztt_genmatch_mt"))

	def get_files(self):
		query = { "process" : "(DYJetsToLL_M10to50|DYJetsToLL_M50|DY1JetsToLL_M50|DY2JetsToLL_M50|DY3JetsToLL_M50|DY4JetsToLL_M50)",
		          "data": False, 
		          "campaign" : self.mc_campaign,
		          "generator" : "madgraph\-pythia8"
		}
		files = datasetsHelper.get_nicks_with_query(query)
		self.log_query(query,files)
		return self.artus_file_names(files)


class Zll_estimation(Ztt_estimation):

	def __init__(self, era, directory, channel):
		super(Ztt_estimation, self).__init__("Zll", "nominal", era, directory, channel)

	@staticmethod
	def get_cuts():
		return Cuts(Cut("(gen_match_2<5||gen_match_2==6)", "zll_genmatch_mt"))

class WJ_estimation(Estimation_method):

	def __init__(self, era, directory, channel):
		super(WJ_estimation, self).__init__("WJ", "nominal", era, directory, channel)

	def get_weights(self):
		return Weights(Weight("(((npartons == 0 || npartons >= 5)*7.09390278348407e-4) + ((npartons == 1)*1.90063898596475e-4) + ((npartons == 2)*5.8529964471165e-5) + ((npartons == 3)*1.9206444928444e-5) + ((npartons == 4)*1.923548021385e-5))/(numberGeneratedEventsWeight*crossSectionPerEventWeight*sampleStitchingWeight)", "wj_stitching_weight"),
		               Weight("((gen_match_2 == 5)*0.95 + (gen_match_2 != 5))", "hadronic_tau_sf"),
		               Weight("eventWeight", "eventWeight"),
		               self.era.get_lumi_weight())
	def get_files(self):
		query = {"process" : "W.*JetsToLNu",
		         "data" : False,
		         "campaign" : self.mc_campaign,
	             "generator" : "madgraph-pythia8"}
		files = datasetsHelper.get_nicks_with_query(query)
		return self.artus_file_names(files)


#class Wj_from_SS_OS_estimation(Wj_estimation):
#
#	def __init__(self, directory):
#		self.folder = "jecUncNom_tauEsNom"
#		self.name = "WJ"
#
#
#	def get_root_objects(self, systematic):
#		root_objects = super(Wj, self).get_root_objects(systematic)
#		self.wj_signal_shape = root_objects[0]
#		print self.wj_signal_shape
#		print "------------------------------------------"
#		print "hier", root_objects
#
#		self.wj_ss_os_category = copy.deepcopy(systematic.get_category())
#		self.wj_ss_os_category.get_cuts().remove("os").get("mt").invert().set_value(70)
#		self.wj_ss_os_category.variable = "q_1*q_2"
#		self.wj_ss_os_category.nbins = 2
#		self.wj_ss_os_category.xlow= -1.1
#		self.wj_ss_os_category.xhigh= 1.1
#		self.wj_ss_os_category.name = "inclusive"
#		self.wj_ss_os = Systematic(category=self.wj_ss_os_category, process="wj",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = Wj())
#		root_objects += self.wj_ss_os.get_root_objects().values()
#
#
#		self.highmt_os_category = copy.deepcopy(systematic.get_category())
#		self.highmt_os_category.get_cuts().get("mt").invert().set_value(70)
#		self.highmt_os_category.name = "high_mt_os"
#
#		self.highmt_ss_category = copy.deepcopy(systematic.get_category())
#		self.highmt_ss_category.get_cuts().get("mt").invert().set_value(70)
#		self.highmt_ss_category.get_cuts().get("os").invert().name = "ss"
#		self.highmt_ss_category.name = "high_mt_ss"
#		
#		for category in [self.highmt_os_category, self.highmt_ss_category]:
#				c_name = category.get_name()
#				setattr(self, c_name, [])
#				print "test"
#				print getattr(self, c_name)
#				getattr(self, c_name).append(Systematic(category=category, process="data", channel="mt", analysis = "example", era="2017", mass=None, syst_var=Nominal(), estimation_method = Data("data", folder="jecUncNom_tauEsNom")))
#
#				getattr(self, c_name).append(Systematic(category=category, process="ztt",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = Ztt()))
#				getattr(self, c_name).append(Systematic(category=category, process="zll",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = Zll()))
#				getattr(self, c_name).append(Systematic(category=category, process="tt",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = TT()))
#				getattr(self, c_name).append(Systematic(category=category, process="vv",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = VV()))
#				getattr(self, c_name).append(Systematic(category=category, process="wj",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=systematic.syst_var, estimation_method = Wj()))
#
#				for systematic in getattr(self, c_name):
#					root_objects += systematic.get_root_objects().values()
#
#		return root_objects
#
#	def define_root_objects(self, systematic):
#		# wj signal region
#		histogram_settings = super(Wj, self).define_root_objects(systematic)
#		return histogram_settings
#
#	def do_estimation(self, systematic, root_objects):
#
#		# call the estimation method of the individual estimations
#		for category in [self.highmt_os_category, self.highmt_ss_category]:
#				c_name = category.get_name()
#				for systematic in getattr(self, c_name):
#					systematic.do_estimation(root_objects)
#
#		# N_Fake^OS
#		n_fake_os = getattr(self, self.highmt_os_category.get_name())[0].shape.result.Integral() - sum([h.shape.result.Integral() for h in getattr(self, self.highmt_os_category.get_name())][1:-1])
#		n_fake_ss = getattr(self, self.highmt_ss_category.get_name())[0].shape.result.Integral() - sum([h.shape.result.Integral() for h in getattr(self, self.highmt_ss_category.get_name())][1:-1])
#
#		f_W_os_ss = getattr(self, self.highmt_os_category.get_name())[-1].shape.result.Integral() / getattr(self, self.highmt_ss_category.get_name())[-1].shape.result.Integral()
#		print f_W_os_ss, "1st"
#
#		self.wj_ss_os.do_estimation(root_objects)
#		f_W_os_ss = self.wj_ss_os.shape.result.GetBinContent(1) / self.wj_ss_os.shape.result.GetBinContent(2)
#		print f_W_os_ss, "2nd"
#
		# hard-coded number with no more motivation than "it works"
#		f_QCD_os_ss = 1.2
#
#		scale_factor = (n_fake_os - f_QCD_os_ss * n_fake_ss)/(1+f_QCD_os_ss/f_W_os_ss)
#
#		print n_fake_os
#		print n_fake_ss
#		print f_W_os_ss
#		print scale_factor
#
#		f_W_high_low = getattr(self, self.highmt_os_category.get_name())[-1].shape.result.Integral()  / self.wj_signal_shape.result.Integral()
		#nothing happens atm with these scale factors
#		return self.wj_signal_shape.result
#
#
#	def get_weights():
#		return Weights(Weight("1.0", "constant"))
#

class TT_estimation(Estimation_method):

	def __init__(self, era, directory, channel):
		super(TT_estimation, self).__init__("TT", "nominal", era, directory, channel)

	def get_weights(self):
		return Weights(Weight("topPtReweightWeight", "topPtReweightWeight"),
		               Weight("eventWeight", "eventWeight"),
		               Weight("((gen_match_2 == 5)*0.95 + (gen_match_2 != 5))", "hadronic_tau_sf"))

	def get_files(self):
		query = { "process" : "^TT$",
		          "data": False, 
		          "campaign" : self.mc_campaign
		}
		files = datasetsHelper.get_nicks_with_query(query)
		self.log_query(query,files)
		return self.artus_file_names(files)


class VV_estimation(Estimation_method):
	def __init__(self, era, directory, channel):
		super(VV_estimation, self).__init__("VV", "nominal", era, directory, channel)

	def get_weights(self):
		return Weights( Weight("((gen_match_2 == 5)*0.95 + (gen_match_2 != 5))", "hadronic_tau_sf"),
		                Weight("eventWeight", "eventWeight"))

	def get_files(self):
		query = { "process" : "(WWTo1L1Nu2Q|"
		                    + "WZTo1L1Nu2Q|"
		                    + "WZTo1L3Nu|"
		                    + "WZTo2L2Q|" 
		                    + "ZZTo2L2Q"
		                    +  ")",
		          "data" : False,
		          "campaign" : self.mc_campaign,
		          "generator" : "amcatnlo-pythia8" }
		files = datasetsHelper.get_nicks_with_query(query)

		query = { "process" : "ZZTo4L",
		          "extension" : "ext1",
		          "data" : False,
		          "campaign" : self.mc_campaign,
		          "generator" : "amcatnlo-pythia8"}
		files += datasetsHelper.get_nicks_with_query(query)

		query = { "process" : "WZJToLLLNu",
		          "data" : False,
		          "campaign" : self.mc_campaign,
		          "generator" : "pythia8"}
		files += datasetsHelper.get_nicks_with_query(query)

		query = { "process" : "(STt-channelantitop4finclusiveDecays|STt-channeltop4finclusiveDecays|STtWantitop5finclusiveDecays|STtWtop5finclusiveDecays)",
		          "data" : False,
		          "campaign" : self.mc_campaign}
		files += datasetsHelper.get_nicks_with_query(query)

		self.log_query("", files)
		return self.artus_file_names(files)

class QCD_estimation(Estimation_method):

	def __init__(self, era, directory, channel, bg_processes, data_process):
		super(QCD_estimation, self).__init__("QCD", "nominal", era, directory, channel)
		self.bg_processes = [copy.deepcopy(p) for p in bg_processes]
		self.data_process = copy.deepcopy(data_process)

	def create_root_objects(self, systematic):
		ss_category = copy.deepcopy(systematic.get_category())
		ss_category.get_cuts().get("os").invert().name = "ss"
		ss_category.name = "ss"

		self.root_objects = []
		self.systematics = []
		for process in [self.data_process] + self.bg_processes:
			self.systematics.append( Systematic(category=ss_category, process=process, analysis =systematic.get_analysis(), era=self.era, syst_var=systematic.get_syst_var()))
			self.root_objects += self.systematics[-1].get_root_objects()

		return self

	def do_estimation(self, systematic, root_objects_holder):
		self.shape = copy.deepcopy(self.systematics[0].do_estimation(root_objects_holder).get_shape())
		for i in range(1, len(self.systematics)):
			self.shape.get_result().Add(self.systematics[i].do_estimation(root_objects_holder).get_shape().get_result(), -1.0)

		self.shape.set_name(systematic.get_name())
		self.shape.save(root_objects_holder)
		return self.shape

	def get_files(self):
		raise NotImplementedError

	def get_weights():
		raise NotImplementedError
