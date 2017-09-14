

# -*- coding: utf-8 -*-
import logging
logger = logging.getLogger(__name__)

from Artus.HenryPlotter.cutstring import Constant

import os
from  Kappa.Skimming.datasetsHelperTwopz import datasetsHelperTwopz
datasetsHelper = datasetsHelperTwopz(os.path.join(os.environ.get("CMSSW_BASE"),"src/Kappa/Skimming/data/datasets.json"))
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

	def data_files(self, channel):
		if channel.get_name() == "mt":
			query = {"data" : True, "campaign": "Run2016(B|C|D|E|F|G|H)", "scenario": "03Feb2017.*"}
			query["process"] = "SingleMuon"
			files = datasetsHelper.get_nicks_with_query(query)
		return files

	def get_name(self):
		return self.name

	def get_lumi_weight(self):
		return Constant(str(self.luminosity), "lumi")
