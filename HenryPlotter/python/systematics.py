
# -*- coding: utf-8 -*-
import ROOT
from Artus.HenryPlotter.histogram import *
import copy

import logging
logger = logging.getLogger(__name__)
"""
"""

class Systematic(object):
	
	def __init__(self, category, process, analysis, era, syst_var, mass=None):
		self.category = category
		self.channel = self.category.get_channel()
		self.process = process
		self.analysis = analysis
		self.era = era
		self.mass = mass
		self.syst_var = syst_var
		logger.debug("Created systematic called %s", self.get_name())

	def get_category(self):
		return self.category
	
	def get_analysis(self):
		return self.analysis

	def get_syst_var(self):
		return self.syst_var

	def set_syst_var(self, new_syst_var):
		self.syst_var = new_syst_var
		return self

	def set_category(self, new_category):
		self.category = new_category
		return self

	# function to return the histogram classes necessary for this systematic variation
	def get_root_objects(self):
		self.process.estimation_method.create_root_objects(self)
		return self.process.estimation_method.get_root_objects()

	def set_root_objects(self, root_objects_holder):
		self.process.estimation_method.set_root_objects(root_objects_holder)
		return self

	def do_estimation(self, root_objects_holder): # function doing the actual calculations.
		self.set_root_objects(root_objects_holder)
		self.shape = self.process.estimation_method.do_estimation(self, root_objects_holder)
		return self

	def get_shape(self):
		return self.shape

	def get_name(self):
		name = "_".join([
			self.process.get_name(),
			self.category.get_name(),
			self.analysis,
			self.era.get_name(),
			self.category.get_variable_name(),
			self.syst_var.get_name()])
		return name

	def summary(self):
		return [self.get_name(), self.category.get_name(), self.process.get_name(), self.analysis, self.era.get_name(), self.category.get_channel_name(), str(self.mass), self.syst_var.get_name() , str(self.process.estimation_method), str(self.shape)]


# holder class for systematics
class Systematics(object):
	
	def __init__(self):
		# member holding the systematics
		self.systematics = []

	def add(self, systematic):
		self.systematics.append(systematic)

	# do the estimations
	def produce(self):
		# create the input histograms, all at once to make optimal use of TDFs
		logger.debug("Creating the histograms")
		self.create_histograms()
		# sort the estimation modules. TODO to be implemented
		# self.sort_estimations()
		# do the background estimations
		logger.debug("Calling the estimation methods")
		self.do_estimations()

	# read root histograms from the inputfiles and write them to the outputfile
	def create_histograms(self):
		self.root_objects_holder = Root_objects("outputfile.root")
		for systematic in self.systematics:
			self.root_objects_holder.add(systematic.get_root_objects())
		logger.debug("root objects before: %s", self.root_objects_holder.root_objects)
		self.root_objects_holder.remove_duplicates()
		self.root_objects_holder.produce_classic(processes=3)
		# multiprocessing returns copies of the histograms. This function puts them back to the estimation methods
		# works based on the histogram names, which hav to be unique
		logger.debug("root objects after: %s", self.root_objects_holder.root_objects)
		

	# TODO function to sort the estimation modules depending on what has to be previously ran
	def sort_estimations(self):
		raise NotImplementedError

	# to the actual estimations. Currently ran in paralell due to expected very low runtime, can in principle be parallelized
	def do_estimations(self):
		for systematic in self.systematics:
			systematic.do_estimation(self.root_objects_holder)

	def summary(self):
		table =  [["name", "category", "process", "analysis", "era", "channel", "mass", "systematic", "estimation method", "results"]]
		for systematic in self.systematics:
			table.append(systematic.summary())
		for line in table:
			logger.info( "|".join([a.ljust(20)[0:20] for a in line]))

	# function to add systematic variations 
	def add_syst_var(self, syst_vars, **kwargs) :
		for i in range(len(self.systematics)):
			Found = True
			for key, value in kwargs.iteritems():
				if hasattr(self.systematics[i], key):
					if not getattr(self.systematics[i], key).get_name() in value:
						Found = False
						break
				elif hasattr(self.systematics[i], "get_"+key):
					if not getattr(self.systematics[i], "get_"+key)().get_name() in value:
						Found = False
						break
				else:
					logger.fatal("Key \"%s\" not found and no member function get_%s().get_name() available", key, key)
					raise KeyError
			if Found:
				for syst_var in syst_vars:
					new_systematic = copy.deepcopy(self.systematics[i])
					new_systematic.set_syst_var(syst_var)
					self.systematics.append(new_systematic)

		
