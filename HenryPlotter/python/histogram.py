# -*- coding: utf-8 -*-

import ROOT
from array import array
import hashlib
import logging
logger = logging.getLogger(__name__)

"""
"""

# Base class for Histogram and Count
class TTree_content(object):
	def __init__(self, name, inputfiles, folder, cuts, weights): # empty histogram
		self.name = name
		self.inputfiles = [inputfiles] if isinstance(inputfiles, str) else inputfiles

		self.cuts = cuts
		self.weights = weights
		self.weight_name = 'weight_' + self.name # internal name needed for TDFs
		self.result = False
		self.folder = folder

	def present(self): # return if h is already filled
		return self.result != False

	def files_folders(self):
		return (self.inputfiles, self.folder)

	def apply_cuts_on_dataframe(self, dataframe):
		for cutstring in self.cuts.extract():
			dataframe = dataframe.Filter(cutstring.extract(), cutstring.name)
		return dataframe

	def produce_eventweight(self, dataframe):
		new_dataframe = dataframe.Define(self.weight_name, self.weights.extract())
		return new_dataframe

	def update(self):
		pass

	def get_name(self):
		return self.name

	def set_name(self, new_name):
		self.name = new_name
		self.update()
		return self

	def __eq__(self, other):
		return self.__hash__() == other.__hash__()

	def __hash__(self):
		m = hashlib.md5()
		m.update(self.name)
		for ifile in self.inputfiles:
			m.update(ifile)
		m.update(self.cuts.expand()) # todo
		m.update(self.weights.extract())
		m.update(self.folder)
		m.update(self.variable.get_name())
		return int(m.hexdigest(), 16)

	def get_result(self):
		if not self.present():
			logger.fatal("The result object of %s called \"%s\" has not yet been produced. Call the create_result() method before calling get_result()", self, self.get_name())
			raise RuntimeError
		return self.result

class Histogram(TTree_content):

	def __init__(self, name, inputfiles, folder, cuts, weights, variable): # empty histogram
		self.variable = variable
		super(Histogram, self).__init__(name, inputfiles, folder, cuts, weights)

	def create_result(self, dataframe=False):
		if dataframe:
			self.result = dataframe.Histo1D(self.variable.get_name(), self.weight_name)
		else: # classic way
			tree = ROOT.TChain()
			for inputfile in self.inputfiles:
				tree.Add(inputfile + "/" + self.folder)
			tree.Draw(self.variable.get_name() + ">>" + self.name + self.variable.get_binning().extract(),
			          self.cuts.expand() + "*" + self.weights.extract(),
			          "goff")
			self.result = ROOT.gDirectory.Get(self.name)
			self.log()
		return self

	def update(self):
		if self.present():
			self.result.SetName(self.name)

	def log(self):
			logger.info("Creating histogram %s with name %s on variable %s", self, self.get_name(), self.variable.get_name())
			logger.debug("\tInput ROOT files: %s", self.inputfiles)
			logger.debug("\tBinning: %s", self.variable.get_binning().extract())
			logger.debug("\tCuts: %s", self.cuts.expand())
			logger.debug("\tWeights: %s", self.weights.extract())
	#		logger.debug("\tResult: %s", self.get_result())

	def save(self, output_tree):
		self.result.Write()

	def summary(self):
		return """Histogram %s """, self.get_name()

# class to count the (weighted) number of events in a selection
class Count(TTree_content):
	def __init__(self, name, inputfiles, folder, cuts, weights):
		super(Count, self).__init__(name, inputfiles, folder, cuts, weights)
		self.inputfiles = [inputfiles] if isinstance(inputfiles, str) else inputfiles

		self.weights = weights
		self.result = False

	def create_result(self, dataframe=False):
		if dataframe:
			self.result = dataframe.Define("flat", "1").Histo1D("flat", self.weight_name)
		else: # classic way
			tree = ROOT.TChain()
			for inputfile in self.inputfiles:
				tree.Add(inputfile + "/" + self.folder)
			
			tree.Draw("1>>" + self.name + "(1)",
			          self.cuts.expand() + "*" + self.weights.extract(),
			          "goff")
			
			self.result = ROOT.gDirectory.Get(self.name).GetBinContent(1)
		return self


	def save(self, output_tree):
		self.result_array = array("f", [self.result])
		name = self.name
		output_tree.Branch(name, self.result_array, name + "/F")

	def update(self):
		if not isinstance(self.result, float):
			self.result = self.result.GetBinContent(59)

# automatic determination of the type
def create_root_object(**kwargs):
	if "variable" in kwargs.keys():
		return Histogram(**kwargs)
	else:
		return Count(**kwargs)

class Root_objects(object):
	def __init__(self, output_file):
		self.root_objects = []
		self.counts = []
		self.produced = False
		self.output_file_name = output_file

	def add(self, root_object):
		if self.produced:
			logger.critical("A produce function has already been called. No more histograms can be added.")
			raise Exception
			return False
		else:
			if isinstance(root_object, list):
				for r in root_object:
					if r.get_name() in [ro.get_name() for ro in self.root_objects]:
						logger.fatal("Unable to add root object with name \"%s\" because another one with the same name is already contained")
						raise KeyError
				self.root_objects += root_object
			else:
				if root_objects.get_name() in [ro.get_name() for ro in self.root_objects]:
						logger.fatal("Unable to add root object with name \"%s\" because another one with the same name is already contained")
						raise KeyError
				self.root_objects.append(root_object)

	def new_histogram(self, **kwargs):
		self.add(Histogram(**kwargs))

	def new_count(self, **kwargs):
		self.add(Count(**kwargs))

	# get all possible files/folders combinations to determine how many data frames are needed
	def get_combinations(self, *args):
		files_folders = []
		for obj in args:
			for o in obj:
				if not o.files_folders() in files_folders:
					files_folders.append(o.files_folders())
		return files_folders

	#getter function depending on the histogram name
	def get(self, name):
		logger.debug("searching for %s in %s", name, [a.get_name() for a in self.root_objects])
		for index in range(len(self.root_objects)):
			if self.root_objects[index].get_name() == name:
				logger.debug("pos: %s, %s", index, self.root_objects[index])
				return self.root_objects[index]

	def create_output_file(self):
		self.output_file = ROOT.TFile(self.output_file_name, "new")
		self.output_tree = ROOT.TTree('output_tree', 'output_tree')
		logger.debug("Created output file \"%s\"", self.output_file_name)

	def produce_tdf(self):
		self.create_output_file()
		self.produced = True
		# determine how many data frames have to be created; sort by inputfiles and trees
		files_folders = self.get_combinations(self.root_objects, self.counts)

		for files_folder in files_folders:
			# create the dataframe
			common_dataframe = ROOT.Experimental.TDataFrame(files_folder[1], files_folder[0][0])
			# loop over the corresponding histograms and create an own dataframe for each histogram -> TODO
			for h in [h for h in self.root_objects if h.files_folders()==files_folder]:
				# find overlapping cut selections -> dummy atm
				special_dataframe = h.apply_cuts_on_dataframe(common_dataframe)
				special_dataframe = h.produce_eventweight(special_dataframe)
				h.create_result(dataframe=special_dataframe)
			# create the histograms
			for h in [h for h in self.root_objects if h.files_folders()==files_folder]:
				h.update()
				h.save(self.output_tree)

	def remove_duplicates(self):
		self.root_objects = list(set(self.root_objects))

	def produce_classic(self, processes=1):
		self.create_output_file()
		self.produced = True
		if processes==1:
			for i in range(len(self.root_objects)):
				self.create_result(i)
		else:
			from pathos.multiprocessing import ProcessingPool as Pool
			pool = Pool(processes=processes)
			self.root_objects =  pool.map(self.create_result, range(len(self.root_objects)))
			logger.debug("map result: %s",  [a for a in self.root_objects])
			
			
		for h in self.root_objects: # write sequentially to prevent race conditions
			h.save(self.output_tree)
		logger.debug("Produced root objects %s", [h.get_name() for h in self.root_objects])
		return self

	def create_result(self, index):
		logger.debug("creating results for %s with index %s", self.root_objects[index].get_name(), index)
		a = self.root_objects[index].create_result()
		logger.debug("result from %s:  are %s", self.root_objects[index].get_name(), a.get_result())
		return a

	def save(self):
		if not self.produced:
			logger.critical("No produce method has been called for %s. Call produce_classic() or produce_tdf() before saving", self)
			raise Exception
		self.output_tree.Fill()
		self.output_file.Write()
		self.output_file.Close()
