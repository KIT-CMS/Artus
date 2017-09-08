
# -*- coding: utf-8 -*-
import ROOT
from Artus.HenryPlotter.histogram import *
import copy

"""
"""

class Systematic(object):
	# class members. Set by hand if you want to use them as defaults
	category_default = None # use category class
	process_default = None  # process after combine convetions
	analysis_default = None # arbitrary string
	era_default = None	# arbitrary string
	channel_default = None  # channel after combine convetions
	mass_default = None	# (Higgs) mass as string
	
	def __init__(self, estimation_method, category=None, process=None, analysis=None, era=None, channel=None, mass=None, syst=None): # all defaults are none, if wanted use class members as default parameters 
		self.category = category if category!=None else Systematic.category_default 
		self.process = process if process!=None else Systematic.process_default 
		self.analysis = analysis if analysis!=None else Systematic.analysis_default 
		self.era = era if era!=None else Systematic.era_default 
		self.channel = channel if channel!=None else channel_default 
		self.mass = mass if mass!=None else Systematic.mass_default 
		self.syst = syst
		self.estimation_method = copy.deepcopy(estimation_method)
		#self.estimation_method.set_systematic(self.syst)
		# TODO: make sure all parameters are not None any more
		self.input_histograms = []

	def get_category(self):
		return self.category

	# function to return the histogram classes necessary for this systematic variation
	def get_histograms(self):
		self.input_histograms = dict([(h.get_name(), h) for h in self.estimation_method.get_histograms(self)])
		#self.input_histograms = self.estimation_method.get_histograms(self)
			# call estimation_methods with corresponding arguments...
		return self.input_histograms

	def produce(self): # function doing the actual calculations.
		self.shape = self.estimation_method.do_estimation(self)

	def get_name(self, *args):
		name = "_".join([self.channel, self.process, self.category.get_name(), self.analysis, self.era])
		if self.syst!=None:
			name += "_" + self.syst.get_name()
		name = "_".join([name]+ list(args))
		print "name: " + name
		print list(args)
		return name

	def summary(self):
		return [self.get_name(), self.category.get_name(), self.process, self.analysis, self.era, self.channel, str(self.mass), self.syst.get_name() if self.syst!=None else "", str(self.estimation_method), str(self.input_histograms), str(self.shape)]


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
		self.create_histograms()
		# sort the estimation modules
		self.sort_estimations()
		# do the background estimations
		self.do_estimation()

	# read root histograms from the inputfiles and write them to the outputfile
	def create_histograms(self):
		root_objects = Root_objects("outputfile.root")
		for systematic in self.systematics:
			root_objects.add(systematic.get_histograms().values())
		root_objects.produce_classic(processes=1)
		# TODO: write back the resulting histograms if not already done 

	# TODO function to sort the estimation modules depending on what has to be previously ran
	def sort_estimations(self):
		pass

	# to the actual estimations. Currently ran in paralell due to expected very low runtime, can in principle be parallelized
	def do_estimation(self):
		for systematic in self.systematics:
			systematic.produce()

	def summary(self): #TODO: beautify with logger
		table =  [["name", "category", "process", "analysis", "era", "channel", "mass", "systematic", "estimation method", "input histograms", "results"]]
		for systematic in self.systematics:
			table.append(systematic.summary())
		for line in table:
			print "|".join([a.ljust(20)[0:20] for a in line])

		
