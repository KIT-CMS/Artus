

# -*- coding: utf-8 -*-
import ROOT

"""
"""

import copy

# class performing the systematic variation
class Systematic_variation(object):

	def __init__(self, name):
		self.name = name

	def get_name(self):
		return self.name

	def change_histogram_name(self, h_settings, direction):
		h_settings["name"] = [h_settings["name"], direction]
		return h_settings

	def shifted_histograms(self, h_settings):
		return h_settings # do nothing

class Different_pipeline(Systematic_variation):
	def __init__(self, name, pipeline_downshift, pipeline_upshift):
		super(Different_pipeline, self).__init__(name)
		self.pipeline_downshift = pipeline_downshift
		self.pipeline_upshift = pipeline_upshift

	def shifted_histograms(self, h_settings):
		up = copy.deepcopy(h_settings)
		up["folder"][2] = self.pipeline_upshift
		up = self.change_histogram_name(up, "Up")

		down = copy.deepcopy(h_settings)
		down["folder"][2] = self.pipeline_downshift
		down = self.change_histogram_name(down, "Down")
		return [up, down]

class Reapply_remove_weight(Systematic_variation):

	def __init__(self, name, weight_name):
		super(Reapply_remove_weight, self).__init__(name)
		self.weight_name = weight_name

	def shifted_histograms(self, h_settings):
		up = copy.deepcopy(h_settings)
		print up["weights"]()
		up["weights"]().square(self.weight_name)
		up = self.change_histogram_name(up, "Up")

		down = copy.deepcopy(h_settings)
		down["weights"]().remove(self.weight_name)
		down = self.change_histogram_name(down, "Down")

		return [up, down]
