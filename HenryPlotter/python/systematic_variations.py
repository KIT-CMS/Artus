

# -*- coding: utf-8 -*-
import ROOT

"""
"""

import copy

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

class Nominal(Systematic_variation):

	def __init__(self):
		self.name = "Nominal"

	def get_name(self):
		return self.name

	def change_histogram_name(self, h_settings, direction):
		return h_settings # do nothing

	def shifted_root_objects(self, h_settings):
		return h_settings # do nothing

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
		super(Reapply_remove_weight, self).__init__(name)
		self.weight_name = weight_name

	def shifted_root_objects(self, h_settings, direction):
		if direction == "Up":
			up = copy.deepcopy(h_settings)
			up["weights"]().square(self.weight_name)
#			up = self.change_histogram_name(up, "Up")
			return up
		elif direction == "Down":
			down = copy.deepcopy(h_settings)
			down["weights"]().remove(self.weight_name)
#			down = self.change_histogram_name(down, "Down")
			return down
		else:
			# error
			assert(False)

