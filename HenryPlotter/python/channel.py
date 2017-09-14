

# -*- coding: utf-8 -*-
import ROOT
from Artus.HenryPlotter.cutstring import *
import logging
logger = logging.getLogger(__name__)

"""
"""

class Channel(object):
	def get_name(self):
		return self.name

class EM(Channel):
	def __init__(self):
		self.name = "em"


class MT(Channel):
	def __init__(self):
		self.name = "mt"

class ET(MT):
	def __init__(self):
		self.name = "mt"
	pass

# collection of channels an analysis can be ran on 
class Channels(object):

	def __init__(self, name):
		self.name = name
		self.channels = []

	def add(self, channel):
		self.channels.append(channel)
