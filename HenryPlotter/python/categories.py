
# -*- coding: utf-8 -*-
import ROOT
from Artus.HenryPlotter.histogram import *
from Artus.HenryPlotter.cutstring import *

"""
"""

# Category object, holding all relevant information defining this category
# TODO: helper functions to compare the overlap of two categories
class Category(object):

	def __init__(self, name, cuts, variable, nbins=None, xlow=None, xhigh=None):
		self.cuts = cuts
		self.name = name
		self.variable = variable
		self.nbins = nbins
		self.xlow = xlow
		self.xhigh = xhigh

	def get_cuts(self):
		return self.cuts

	def get_name(self):
		return self.name

	def get_variable(self):
		return self.variable

	def get_nbins(self):
		return self.nbins

	def get_xlow(self):
		return self.xlow

	def get_xhigh(self):
		return self.xhigh