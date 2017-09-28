# -*- coding: utf-8 -*-
from Artus.HenryPlotter.cutstring import *
import logging
logger = logging.getLogger(__name__)

"""
"""

# Class to store a variable and its binning
class Variable(object):

	def __init__(self, name, binning):
		self.name = name
		self.binning = binning

	def get_name(self):
		return self.name

	def get_binning(self):
		return self.binning
