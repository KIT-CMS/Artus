# -*- coding: utf-8 -*-
import logging
logger = logging.getLogger(__name__)

"""
"""
# non-functional base class, just to express that the latter ones belong together
class Binning(object):

	def __init__(self):
		pass

class Variable_Binning(Binning):
	def __init__(self, *args):
		if not sorted(list(set(args))) == list(args):
			logger.fatal("An invalid variable binning has been requested with a wrong bin border ordering or a repetition of bins.")
			logger.fatal("The binning was: %s.", args)
			raise Exception
		self.bin_borders = args

	def get_nbinsx(self):
		return len(self.bin_borders)-1

	def extract(self):
		return str(self.bin_borders)
			

class Constant_Binning(Binning):
	def __init__(self, nbinsx, xlow, xhigh):
		self.nbinsx = int(nbinsx)
		self.xlow = float(xlow)
		self.xhigh = float(xhigh)

	def extract(self):
		return "".join(["(", str(self.nbinsx), ",",  str(self.xlow), ",", str(self.xhigh), ")"])

	def get_nbinsx(self):
		return self.nbinsx
