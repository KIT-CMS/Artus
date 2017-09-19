

# -*- coding: utf-8 -*-
import ROOT

"""
"""

import copy
import logging
logger = logging.getLogger(__name__)

# this helper function can be used in case the systematic variation's name ends with "Down" and "Up"
def create_syst_variations(name, syst_variation):
	results = []
	results.append(syst_variation(name, name + "Down", "Down"))
	results.append(syst_variation(name, name + "Up", "Up"))
	return results

# class performing the systematic variation
class Systematic_variation(object):

	def __init__(self, name, direction):
		self.name = name
		self.direction = direction

	def get_name(self):
		return self.name + "_" + self.direction

	def change_histogram_name(self, h_settings, direction):
		if isinstance(h_settings["name"], list):
			h_settings["name"].append( direction )
		else:
			h_settings["name"] = [h_settings["name"], direction]
		return h_settings

	def shifted_root_objects(self, h_settings):
		return h_settings # do nothing

	def is_nominal(self):
		return False

class Nominal(Systematic_variation):

	def __init__(self, direction=None):
		self.name = "Nominal"
		self.direction = direction

	def get_name(self):
		name = self.name
		if self.direction:
			name += "_"  + self.direction
		return name

	def change_histogram_name(self, h_settings, direction):
		return h_settings # do nothing

	def shifted_root_objects(self, h_settings):
		return h_settings # do nothing

	def is_nominal(self):
		return True

class Different_pipeline(Systematic_variation):
	def __init__(self, name, pipeline, direction):
		super(Different_pipeline, self).__init__(name, direction)
		self.pipeline = pipeline

	def shifted_root_objects(self, h_settings):
		for index in range(len(h_settings)):
			h_settings[index]["folder"][2] = self.pipeline
		return h_settings

class Reapply_remove_weight(Systematic_variation):

	def __init__(self, name, weight_name, direction):
		super(Reapply_remove_weight, self).__init__(name, direction)
		self.weight_name = weight_name

	def shifted_root_objects(self, h_settings):
		for index in range(len(h_settings)):
			if self.direction == "Up":
				h_settings[index]["weights"] = h_settings[index]["weights"]().square(self.name)
			elif self.direction == "Down":
				h_settings[index]["weights"] = h_settings[index]["weights"]().remove(self.name)
			return h_settings

