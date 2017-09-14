
# -*- coding: utf-8 -*-
import ROOT
from Artus.HenryPlotter.histogram import *
import copy

import logging
logger = logging.getLogger(__name__)
"""
"""

class Systematic(object):
	
	def __init__(self, category, process, analysis, era, channel, syst_var, mass=None):
		self.category = category
		self.process = process
		self.analysis = analysis
		self.era = era
		self.channel = channel
		self.mass = mass
		self.syst_var = syst_var
		self.shape = "doof"
		logger.debug("Created systematic called %s", self.get_name())

	def get_category(self):
		return self.category

	# function to return the histogram classes necessary for this systematic variation
	def get_root_objects(self):
		self.process.estimation_method.create_root_objects(self)
			#self.input_root_objects = dict([(h.get_name(), h) for h in self.process.estimation_method.get_root_objects(self)])
			# call estimation_methods with corresponding arguments...
		return self.process.estimation_method.get_root_objects()

	def do_estimation(self, root_objects_holder): # function doing the actual calculations.
		self.shape = self.process.estimation_method.do_estimation(self, root_objects_holder)
		return self

	def get_name(self):
		name = "_".join([self.channel.get_name(), self.process.get_name(), self.category.get_name(), self.analysis, self.era.get_name()])
		if self.syst_var!=None:
			name += "_" + self.syst_var.get_name()
#		name = "_".join([name]+ list(args))
		return name

	def summary(self):
		return [self.get_name(), self.category.get_name(), self.process.get_name(), self.analysis, self.era.get_name(), self.channel.get_name(), str(self.mass), self.syst_var.get_name() , str(self.process.estimation_method), str(self.shape)]


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
		# sort the estimation modules
		self.sort_estimations()
		# do the background estimations
		logger.debug("Calling the estimation methods")
		self.do_estimations()

	# read root histograms from the inputfiles and write them to the outputfile
	def create_histograms(self):
		self.root_objects_holder = Root_objects("outputfile.root")
		for systematic in self.systematics:
			self.root_objects_holder.add(systematic.get_root_objects())
		self.root_objects_holder.remove_duplicates()
		self.root_objects_holder.produce_classic(processes=1)

	# TODO function to sort the estimation modules depending on what has to be previously ran
	def sort_estimations(self):
		logger.warning("Not implemented")

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

		
