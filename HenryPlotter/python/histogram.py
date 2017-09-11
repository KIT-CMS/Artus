# -*- coding: utf-8 -*-

import ROOT
from array import array
import hashlib

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
		self.folder = Histogram.default_folder if (folder==None) else folder

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
		m.update(self.variable)
		return int(m.hexdigest(), 16)


class Histogram(TTree_content):

	default_nbins = 100
	default_xlow = 0.0
	default_xhigh = 1.0
	default_variable = "x"
	default_folder = ""

	def __init__(self, name, inputfiles, folder, cuts, weights, variable=None, nbins=None, xlow=None, xhigh=None): # empty histogram
		self.nbins = Histogram.default_nbins if (nbins==None) else nbins
		self.xlow = Histogram.default_xlow if (xlow==None) else xlow
		self.xhigh = Histogram.default_xhigh if (xhigh==None) else xhigh
		self.variable = Histogram.default_variable if (variable==None) else variable
		super(Histogram, self).__init__(name, inputfiles, folder, cuts, weights)

	def get_result(self, dataframe=False):
		if dataframe:
			self.result = dataframe.Histo1D(self.variable, self.weight_name)
		else: # classic way
			tree = ROOT.TChain()
			for inputfile in self.inputfiles:
				tree.Add(inputfile + "/" + self.folder)
			tree.Draw(self.variable + ">>" + self.name + "(" + ",".join([str(self.nbins), str(self.xlow), str(self.xhigh)]) + ")",
			          self.cuts.expand() + "*" + self.weights.extract(),
			          "goff")
			self.result = ROOT.gDirectory.Get(self.name)
		return self
	
	def show(self):
		print "showing histogram " + self.name 
		self.result.Draw()	
		import time
		time.sleep(1.5)

	def update(self):
		if self.present():
			self.result.SetName(self.name)

	def save(self, output_tree):
		self.result.Write()

# class to count the (weighted) number of events in a selection
class Count(TTree_content):
	def __init__(self, name, inputfiles, folder, cuts, weights):
		super(Count, self).__init__(name, inputfiles, folder, cuts, weights)
		self.inputfiles = [inputfiles] if isinstance(inputfiles, str) else inputfiles

		self.weights = weights
		self.result = False

	def get_result(self, dataframe=False):
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

	def show(self):
		print "Result from count with name " + self.name + " : " + str(self.result) + ", selection: " +self.cuts.expand() + "*" + self.weights.extract()

	def save(self, output_tree):
		self.result_array = array("f", [self.result])
		name = self.name
		output_tree.Branch(name, self.result_array, name + "/F")

	def update(self):
		if not isinstance(self.result, float):
			self.result = self.result.GetBinContent(59)

# automatic determination of the type
def create_root_object(**kwargs):
	keys = ["variable", "nbins", "xlow", "xhigh"]
	if all(item in  kwargs.keys() for item in keys):
		return Histogram(**kwargs)
	elif any(item in  kwargs.keys() for item in keys):
		print "invalid configuration found"
		assert(False)
	else:
		return Count(**kwargs)

class Root_objects(object):
	def __init__(self, output_file):
		self.root_objects = []
		self.counts = []
		self.produced = False
		self.output_file = ROOT.TFile(output_file, "new")
		self.output_tree = ROOT.TTree('output_tree', 'output_tree')

	def add(self, root_object):
		if self.produced:
			print "The results.produced() has already been called. No more histograms can be added"
			return False
		else:
			if isinstance(root_object, list):
				self.root_objects += root_object
			else:
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

	def produce_tdf(self):
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
				h.get_result(dataframe=special_dataframe)
			# create the histograms
			for h in [h for h in self.root_objects if h.files_folders()==files_folder]:
				h.update()
				h.save(self.output_tree)

	def remove_duplicates(self):
		self.root_objects = list(set(self.root_objects))

	def produce_classic(self, processes=1):
		self.produced = True
		if processes==1:
			for i in range(len(self.root_objects)):
				self.get_result(i)
		else:
			from pathos.multiprocessing import ProcessingPool as Pool
			pool = Pool(processes=processes)
			self.root_objects = pool.map(self.get_result, range(len(self.root_objects)))
			
		for h in self.root_objects: # write sequentially to prevent race conditions
			h.save(self.output_tree)
		return self

	def get_result(self, index):
		return self.root_objects[index].get_result()
	
	def show():
		for h in self.root_objects:
			h.show()

	def save(self):
		assert(self.produced)
		self.output_tree.Fill()
		self.output_file.Write()
		self.output_file.Close()
