

# -*- coding: utf-8 -*-
import logging
logger = logging.getLogger(__name__)

from Artus.HenryPlotter.cutstring import Constant
"""
"""

class Era(object):
	pass
	# luminosity
	# data files
	# NOT mc files!	 -> they correspond to the background estimation methods!

class Run2016BCDEFG(Era):
	def __init__(self):
		self.name = "Run2016BCDEFG"
		self.luminosity = 35.87*1000.0
		self.data_format = "MINIAOD"
		self.energy = 13

	def data_files(channel):
		#build datasets.json query
		pass

	def get_name(self):
		return self.name

	def get_lumi_weight(self):
		return Constant(str(self.luminosity), "lumi")
