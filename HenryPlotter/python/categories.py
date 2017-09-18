
# -*- coding: utf-8 -*-
import ROOT
from Artus.HenryPlotter.histogram import *
from Artus.HenryPlotter.cutstring import *

import logging
logger = logging.getLogger(__name__)

"""
"""

# Category object, holding all relevant information defining this category
# TODO: helper functions to compare the overlap of two categories
class Category(object):

	def __init__(self, name, channel, cuts, variable):
		self.channel = channel
		self.cuts = cuts + channel.get_cuts()
		self.name = name
		self.variable = variable
		logger.debug("Created category \"%s\" with selection %s", self.get_name(), self.cuts.expand())

	def get_cuts(self):
		return self.cuts

	def get_name(self):
		return self.channel.get_name() + "_"+ self.name

	def get_channel_name(self):
		return self.channel.get_name()

	def get_variable_name(self):
		return self.variable.get_name()

	def get_variable(self):
		return self.variable

	def get_channel(self):
		return self.channel
