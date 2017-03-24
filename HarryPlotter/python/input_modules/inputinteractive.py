# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import copy
import hashlib
import ROOT
import sys

import Artus.HarryPlotter.inputbase as inputbase
import Artus.HarryPlotter.utility.roottools as roottools
import Artus.Utility.progressiterator as pi


class InputInteractive(inputbase.InputBase):
	def __init__(self):
		super(InputInteractive, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(InputInteractive, self).modify_argument_parser(parser, args)
		
		self.input_options.add_argument("--x-errors", type=str, nargs="+",
		                                help="x-error(s). If --x-errors-up is also specified, this corresponds to the x-errors low")
		self.input_options.add_argument("--x-errors-up", type=str, nargs="+",
										help="x-error(s) high")
		self.input_options.add_argument("--y-errors", type=str, nargs="+",
		                                help="y-error(s). If --y-errors-up is also specified, this corresponds to the y-errors low")
		self.input_options.add_argument("--y-errors-up", type=str, nargs="+",
										help="y-error(s) high")
		self.input_options.add_argument("--z-errors", type=str, nargs="+",
		                                help="z-error(s)")
		self.input_options.add_argument("-w", "--weights", type=str, nargs="+",
		                                help="Weight(s)")
	
	def prepare_args(self, parser, plotData):
		super(InputInteractive, self).prepare_args(parser, plotData)
		
		self.prepare_list_args(plotData, ["nicks", "x_expressions", "y_expressions", "z_expressions", "x_bins", "y_bins", "z_bins", "scale_factors", "x_errors", "x_errors_up", "y_errors", "y_errors_up", "z_errors", "weights"])
		
		def secure_float_conversion(input_string):
			try:
				return float(input_string)
			except:
				return input_string
		
		# prepare binning
		for key in ["x_bins", "y_bins", "z_bins"]:
			for index, binning in enumerate(plotData.plotdict[key]):	
				if (not binning is None) and (len(binning) > 1):
					plotData.plotdict[key][index] = [float(bin_edge) for bin_edge in binning]
		
		# parse values/errors to plot
		values_keys = ["x_expressions", "y_expressions", "z_expressions"]
		errors_keys = ["x_errors", "y_errors", "z_errors"]
		errors_up_keys = ["x_errors_up", "y_errors_up"]
		weight_keys = ["weights"]
		for key in values_keys + errors_keys + errors_up_keys + weight_keys:
			if plotData.plotdict[key] != None:
				plotData.plotdict[key] = [[secure_float_conversion(value) for value in (values.split() if values != None else [])] for values in plotData.plotdict[key]]
		
		# set default errors to 0.0 and weights to 1.0
		for index in xrange(len(plotData.plotdict["nicks"])):
			for values_key, errors_key in zip(values_keys, errors_keys):
				if len(plotData.plotdict[errors_key][index]) == 0:
					plotData.plotdict[errors_key][index] = len(plotData.plotdict[values_key][index]) * [0.0]
			for values_key, weight_key in zip(values_keys, weight_keys):
				if len(plotData.plotdict[weight_key][index]) == 0:
					plotData.plotdict[weight_key][index] = len(plotData.plotdict[values_key][index]) * [1.0]
		
		x_errors_up_given = []
		y_errors_up_given = []
		for index in xrange(len(plotData.plotdict["nicks"])):
			for errors_up_key in errors_up_keys:
				if errors_up_key == "x_errors_up":
					if len(plotData.plotdict[errors_up_key][index]) == 0:
						x_errors_up_given.append(False)
					else:
						x_errors_up_given.append(True)
				elif errors_up_key == "y_errors_up":
					if len(plotData.plotdict[errors_up_key][index]) == 0:
						y_errors_up_given.append(False)
					else:
						y_errors_up_given.append(True)
		for index in xrange(len(plotData.plotdict["nicks"])):
			for values_key, errors_up_key in zip(values_keys, errors_up_keys):
				if len(plotData.plotdict[errors_up_key][index]) == 0 and ((errors_up_key == "x_errors_up" and x_errors_up_given[index] == False and y_errors_up_given[index] == True) or (errors_up_key == "y_errors_up" and y_errors_up_given[index] == False and x_errors_up_given[index] == True)):
					plotData.plotdict[errors_up_key][index] = len(plotData.plotdict[values_key][index]) * [0.0]
		
		# check that x/y/z values/errors for one plot have the same size
		for index in xrange(len(plotData.plotdict["nicks"])):
			n_values_per_plot = []
			for key in values_keys + errors_keys + errors_up_keys + weight_keys:
				if len(plotData.plotdict[key][index]) > 0:
					n_values_per_plot.append(len(plotData.plotdict[key][index]))
			assert(len(set(n_values_per_plot)) == 1)
		
		inputbase.InputBase.prepare_nicks(plotData)
	
	def run(self, plotData):
		
		for index, (
				nick,
				x_bins, x_values, x_errors, x_errors_up,
				y_bins, y_values, y_errors, y_errors_up,
				z_bins, z_values, z_errors,
				weights,
		) in enumerate(pi.ProgressIterator(zip(*[plotData.plotdict[key] for key in [
				"nicks",
				"x_bins", "x_expressions", "x_errors", "x_errors_up",
				"y_bins", "y_expressions", "y_errors", "y_errors_up",
				"z_bins", "z_expressions", "z_errors",
				"weights",
		]]), description="Reading inputs")):
			
			# prepare unique name
			name_hash = hashlib.md5("_".join([str(item) for item in [nick, x_bins, x_values, x_errors, x_errors_up, y_bins, y_values, y_errors, y_errors_up, z_bins, z_values, z_errors, weights]])).hexdigest()
			
			# determine mode
			create_function = False
			create_graph = False
			create_histogram = False
			if x_bins is None:
				if len(y_values) == 0:
					create_function = True
				else:
					create_graph = True
			else:
				if len(x_values) == 1:
					create_function = True
				else:
					create_histogram = True
			
			if create_function:
				# prepare binning
				default_binning = ["100,-1e3,1e3"]
				if x_bins is None:
					x_bins = copy.deepcopy(default_binning)
				if y_bins is None:
					y_bins = copy.deepcopy(default_binning)
				if z_bins is None:
					z_bins = copy.deepcopy(default_binning)
				x_bins = [float(x) for x in x_bins[0].split(",")]
				y_bins = [float(y) for y in y_bins[0].split(",")]
				z_bins = [float(z) for z in z_bins[0].split(",")]
			
				expression = " ".join([str(x_value) for x_value in x_values])
				formula = ROOT.TFormula("formula_"+name_hash, expression)
				function_class = None
				function_class_name = ""
				lim_args = []
				if formula.GetNdim() >= 1:
					function_class = ROOT.TF1
					function_class_name = "ROOT.TF1"
					lim_args.extend(x_bins[1:])
				if formula.GetNdim() >= 2:
					function_class = ROOT.TF2
					function_class_name = "ROOT.TF2"
					lim_args.extend(y_bins[1:])
				if formula.GetNdim() >= 3:
					function_class = ROOT.TF3
					function_class_name = "ROOT.TF3"
					lim_args.extend(z_bins[1:])
				
				log.debug(function_class_name+"(function_"+name_hash+", "+expression+", "+(", ".join([str(lim) for lim in lim_args]))+")")
				root_function = function_class("function_"+name_hash, expression, *lim_args)
				
				if formula.GetNdim() >= 1:
					root_function.SetNpx(int(x_bins[0]))
				if formula.GetNdim() >= 2:
					root_function.SetNpy(int(y_bins[0]))
				if formula.GetNdim() >= 3:
					root_function.SetNpz(int(z_bins[0]))
				
				plotData.plotdict.setdefault("root_objects", {})[nick] = root_function
			
			elif create_graph:
				if len(z_values) == 0:
					if len(x_errors_up) == 0 and len(y_errors_up) == 0:
						log.debug("ROOT.TGraphErrors("+
								str(len(x_values))+", "+
								str(array.array("d", x_values))+", "+str(array.array("d", y_values))+", "+
								str(array.array("d", x_errors))+", "+str(array.array("d", y_errors))+")"
						)
						plotData.plotdict.setdefault("root_objects", {})[nick] = ROOT.TGraphErrors(
								len(x_values),
								array.array("d", x_values), array.array("d", y_values),
								array.array("d", x_errors), array.array("d", y_errors)
						)
					else:
						log.debug("ROOT.TGraphAsymmErrors("+
								str(len(x_values))+", "+
								str(array.array("d", x_values))+", "+str(array.array("d", y_values))+", "+
								str(array.array("d", x_errors))+", "+str(array.array("d", x_errors_up))+", "+
								str(array.array("d", y_errors))+", "+str(array.array("d", y_errors_up))+")"
						)
						plotData.plotdict.setdefault("root_objects", {})[nick] = ROOT.TGraphAsymmErrors(
								len(x_values),
								array.array("d", x_values), array.array("d", y_values),
								array.array("d", x_errors), array.array("d", x_errors_up),
								array.array("d", y_errors), array.array("d", y_errors_up)
						)
				else:
					log.debug("ROOT.TGraph2DErrors("+
							str(len(x_values))+", "+
							str(array.array("d", x_values))+", "+str(array.array("d", y_values))+", "+str(array.array("d", z_values))+", "+
							str(array.array("d", x_errors))+", "+str(array.array("d", y_errors))+", "+str(array.array("d", z_errors))+")"
					)
					plotData.plotdict.setdefault("root_objects", {})[nick] = ROOT.TGraph2DErrors(
							len(x_values),
							array.array("d", x_values), array.array("d", y_values), array.array("d", z_values),
							array.array("d", x_errors), array.array("d", y_errors), array.array("d", z_errors)
					)
				plotData.plotdict["root_objects"][nick].SetName("graph_"+name_hash)
				plotData.plotdict["root_objects"][nick].SetTitle("")
			
			elif create_histogram:
				root_histogram = roottools.RootTools.create_root_histogram(
						x_bins=x_bins,
						y_bins=y_bins,
						z_bins=z_bins,
						profile_histogram=False,
						name="histogram_"+name_hash
				)
				
				if root_histogram.GetDimension() == 1:
					if len(y_values) == 0:
						log.debug("ROOT.TH1.FillN("+str(len(x_values))+", "+str(array.array("d", x_values))+", "+str(array.array("d", weights))+")")
						root_histogram.FillN(len(x_values), array.array("d", x_values), array.array("d", weights))
					else:
						set_bin_errors = any([bin_error != 0.0 for bin_error in y_errors])
						log.debug("ROOT.TH1.SetBinContent/SetBinError(<"+str(x_values)+", "+str(y_values)+", "+str(y_errors)+">)")
						for x_value, y_value, bin_error in zip(x_values, y_values, y_errors):
							global_bin = root_histogram.FindBin(x_value)
							root_histogram.SetBinContent(global_bin, y_value)
							if set_bin_errors:
								root_histogram.SetBinError(global_bin, bin_error)
				
				elif root_histogram.GetDimension() == 2:
					if len(z_values) == 0:
						log.debug("ROOT.TH1.FillN("+str(len(x_values))+", "+str(array.array("d", x_values))+", "+str(array.array("d", y_values))+", "+str(array.array("d", weights))+")")
						root_histogram.FillN(len(x_values), array.array("d", x_values), array.array("d", y_values), array.array("d", weights))
					else:
						set_bin_errors = any([bin_error != 0.0 for bin_error in z_errors])
						log.debug("ROOT.TH1.SetBinContent/SetBinError(<"+str(x_values)+", "+str(y_values)+", "+str(z_values)+", "+str(z_errors)+">)")
						for x_value, y_value, z_value, bin_error in zip(x_values, y_values, z_values, z_errors):
							global_bin = root_histogram.FindBin(x_value, y_value)
							root_histogram.SetBinContent(global_bin, z_value)
							if set_bin_errors:
								root_histogram.SetBinError(global_bin, bin_error)
				
				elif root_histogram.GetDimension() == 3:
					log.debug("ROOT.TH1.FillN("+str(len(x_values))+", "+str(array.array("d", x_values))+", "+str(array.array("d", y_values))+", "+str(array.array("d", z_values))+", "+str(array.array("d", weights))+")")
					root_histogram.FillN(len(x_values), array.array("d", x_values), array.array("d", y_values), array.array("d", z_values), array.array("d", weights))
				
				plotData.plotdict.setdefault("root_objects", {})[nick] = root_histogram
		
		# run upper class function at last
		super(InputInteractive, self).run(plotData)

