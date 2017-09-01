# -*- coding: utf-8 -*-

import ROOT


"""
"""

class Histogram(object):

	default_nbins = 100
	default_xlow = 0.0
	default_xhigh = 1.0
	default_variable = "x"
	default_folder = ""

	def __init__(self, name, inputfiles, folder, cuts, weights, variable=None, nbins=None, xlow=None, xhigh=None): # empty histogram
		self.name = name
		self.nbins = Histogram.default_nbins if (nbins==None) else nbins
		self.xlow = Histogram.default_xlow if (xlow==None) else xlow
		self.xhigh = Histogram.default_xhigh if (xhigh==None) else xhigh
		self.folder = Histogram.default_folder if (folder==None) else folder
		self.variable = Histogram.default_variable if (variable==None) else variable

		self.inputfiles = [inputfiles] if isinstance(inputfiles, str) else inputfiles

		self.cuts = cuts
		self.weights = weights
		self.weight_name = 'weight_' + self.name
		self.root_object = False

	def present(self): # return if h is already filled
		return self.root_object != False

	def add(self, second_histogram):
		pass # to be implemented

	def files_folders(self):
		return (self.inputfiles, self.folder)

	def apply_cuts_on_dataframe(self, dataframe):
		for cutstring in self.cuts.extract():
			dataframe = dataframe.Filter(cutstring)
		return dataframe

	def create_histogram(self, dataframe=False):
		if dataframe:
			self.root_object = dataframe.Histo1D(self.variable, self.weight_name)
		else:
			tree = ROOT.TChain()
			for inputfile in self.inputfiles:
				tree.Add(inputfile + "/" + self.folder)
			print self.cuts
			print self.cuts.expand() + "*" + self.weights.extract()
			tree.Draw(self.variable + ">>" + self.name + "(" + ",".join([str(self.nbins), str(self.xlow), str(self.xhigh)]) + ")",
			          self.cuts.expand() + "*" + self.weights.extract(),
			          "goff")
			self.root_object = ROOT.gDirectory.Get(self.name)
		return self


	def produce_eventweight(self, dataframe):
		new_dataframe = dataframe.Define(self.weight_name, self.weights.extract())
		return new_dataframe
	
	def draw(self):
		self.root_object.Draw()	
		import time
		time.sleep(1.5)

	def update(self):
		if self.present():
			self.root_object.SetName(self.name)

class Root_objects(object):
	def __init__(self, output_file):
		self.histograms = []
		self.counts = []
		self.produced = False
		self.output_file = ROOT.TFile(output_file, "new")

	def add_histogram(self, histogram):
		if self.produced:
			print "The root_objects.produced() has already been called. No more histograms can be added"
			return False
		else:
			self.histograms.append(histogram)

	def new_histogram(self, **kwargs):
		self.add_histogram(Histogram(**kwargs))

	# debug function
	def print_all(self):
		print self.histograms

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
		files_folders = self.get_combinations(self.histograms, self.counts)

		for files_folder in files_folders:
			# create the dataframe
			common_dataframe = ROOT.Experimental.TDataFrame(files_folder[1], files_folder[0][0])
			# loop over the corresponding histograms and create an own dataframe for each histogram -> TODO
			for h in [h for h in self.histograms if h.files_folders()==files_folder]:
				# find overlapping cut selections -> dummy atm
				special_dataframe = h.apply_cuts_on_dataframe(common_dataframe)
				special_dataframe = h.produce_eventweight(common_dataframe)
				h.create_histogram(dataframe=special_dataframe)
			# create the histograms
			for h in [h for h in self.histograms if h.files_folders()==files_folder]:
				h.update()
				h.draw()

	def create_histogram(self, index):
		return self.histograms[index].create_histogram()

	def produce_classic(self, processes=1):
		self.produced = True
		if processes==1:
			for i in range(len(self.histograms)):
				self.create_histogram(i)
		else:
			from pathos.multiprocessing import ProcessingPool as Pool
			pool = Pool(processes=processes)
			res = pool.map(self.create_histogram, range(len(self.histograms)))
			for h in res:
				h.root_object.Write()


	def save(self):
		if not self.produced:
			self.produce()
		self.output_file.Write()
		self.output_file.Close()
