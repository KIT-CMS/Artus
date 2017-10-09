
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
		self.mc_campaign = "RunIISummer17MiniAOD"
		self.channel = channel
		self.era = era
		self.root_objects = set()


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

		for setting in root_object_settings:
			self.root_objects.add( create_root_object(**setting))
		return self

	def get_root_objects(self):
		return self.root_objects

	def set_root_objects(self, root_object_holder):
                # this function disturbed the creation of histograms... for what is it needed anyway?
                pass
		

	# doing nothing, shape is exactly the histogram as default
	def do_estimation(self, systematic, root_objects):
		logger.debug("Calling do_estimation for the estimation method %s. ", self.get_name())
		if len(self.get_root_objects()) == 0:
			logger.warning("No histogram associated to %s with name %s.", self, self.get_name())
			raise Exception
                root_obj = self.get_root_objects()[0]
                for ro in self.get_root_objects():
                    if ro.variable.get_name() == systematic.get_category().get_variable_name():
                        root_obj = ro
		return root_obj

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
		               self.era.get_lumi_weight())

	@staticmethod
	def get_cuts():
		return Cuts(Cut("gen_match_2==5", "ztt_genmatch_mt"))

	def get_files(self):
		query = { "process" : "(DYJetsToLL_M10to50|DYJetsToLL_M50)",
		          "data": False, 
		          "campaign" : self.mc_campaign,
		          "generator" : "madgraph\-pythia8",
                          "version" : "v1"
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
		return Weights(
		               Weight("((gen_match_2 == 5)*0.95 + (gen_match_2 != 5))", "hadronic_tau_sf"),
		               Weight("eventWeight", "eventWeight"),
		               self.era.get_lumi_weight())
	def get_files(self):
		query = {"process" : "WJetsToLNu",
		         "data" : False,
		         "campaign" : self.mc_campaign,
	             "generator" : "madgraph-pythia8"}
		files = datasetsHelper.get_nicks_with_query(query)
		return self.artus_file_names(files)

class TT_estimation(Estimation_method):

	def __init__(self, era, directory, channel):
		super(TT_estimation, self).__init__("TT", "nominal", era, directory, channel)

	def get_weights(self):
		return Weights(Weight("topPtReweightWeight", "topPtReweightWeight"),
		               Weight("eventWeight", "eventWeight"),
		               Weight("((gen_match_2 == 5)*0.95 + (gen_match_2 != 5))", "hadronic_tau_sf"),
                               self.era.get_lumi_weight())

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
		self.systematics = []
		self.root_objects = set()

	def create_root_objects(self, systematic):
		ss_category = copy.deepcopy(systematic.get_category())
		ss_category.get_cuts().get("os").invert().name = "ss"
                ss_category.name += "_ss"

		for process in [self.data_process] + self.bg_processes:
			self.systematics.append( Systematic(category=ss_category, process=process, analysis =systematic.get_analysis(), era=self.era, syst_var=systematic.get_syst_var()))
			self.root_objects.update(self.systematics[-1].get_root_objects())
		logger.debug("root objects from %s: %s", self.get_name(), self.root_objects)
		return self

	def do_estimation(self, systematic, root_objects_holder):
                considered_variable = systematic.get_category().get_variable()
                considered_systematics = [s for s in self.systematics if s.get_category().get_variable_name() == considered_variable.get_name()]
		considered_systematics[0].do_estimation(root_objects_holder)
		self.shape = considered_systematics[0].get_shape()
		for i in range(1, len(considered_systematics)):
			considered_systematics[i].do_estimation(root_objects_holder)
		for i in range(1, len(considered_systematics)):
			self.shape.get_result().Add(considered_systematics[i].get_shape().get_result(), -1.0)

		self.shape.set_name(systematic.get_name())
		self.shape.save(root_objects_holder)
		return self.shape

	# data-driven estimation, no associated files and weights
	def get_files(self):
		raise NotImplementedError

	def get_weights():
		raise NotImplementedError
