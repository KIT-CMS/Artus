# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.Utility.tools as tools


class Cutflow(analysisbase.AnalysisBase):
	def __init__(self):
		super(Cutflow, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(Cutflow, self).modify_argument_parser(parser, args)
		
		self.cutflow_options = parser.add_argument_group(
			"Cutflow options",
			"Prepares cutflow histograms for plotting by computing efficiencies. "
			"If neither cutflow names nor nicks are set, cutflow histograms are "
			"guessed."
		)
		self.cutflow_options.add_argument("--cutflow-names", nargs="*", default=[],
				help="Names of cutflow histograms; if not given, histograms are guessed.")
		self.cutflow_options.add_argument("--cutflow-nicks", nargs="*", default=[],
				help="Nicks of cutflow histograms; if not given, histograms are guessed.")
		self.cutflow_options.add_argument("--rel-cuts", action="store_true", default=False,
				help="Scale bins relative to respective left bins.")
		self.cutflow_options.add_argument("--cutflow-base-bin", default='1',
				help="Cut to use as base level (1.0 in absolute cutflow), either as an index or label. [Default: %(default)s]")
		self.cutflow_options.add_argument("--cutflow-sequence", nargs="*", default=[],
				help="Cuts to show for cutflows. [Default: <extracted from cutflows>]")
		self.cutflow_options.add_argument("--cutflow-blacklist", nargs="*", default=[],
				help="Cuts to erase from the cutflow sequence.")

	def prepare_args(self, parser, plotData):
		super(Cutflow, self).prepare_args(parser, plotData)
		if plotData.plotdict["x_label"] == parser.get_default("x_label"):
			plotData.plotdict["x_label"] = ""
		if plotData.plotdict["y_label"] == parser.get_default("y_label"):
			plotData.plotdict["y_label"] = ("Relative " if plotData.plotdict["rel_cuts"] else "") + "Cut Efficiency"
		# integer index or string label
		try:
			plotData.plotdict["cutflow_base_bin"] = int(plotData.plotdict["cutflow_base_bin"])
		except ValueError:
			plotData.plotdict["cutflow_base_bin"] = str(plotData.plotdict["cutflow_base_bin"])
	
	def run(self, plotData=None):
		super(Cutflow, self).run(plotData)
		cutflows = self._get_cutflows(
			plotData,
			histo_names=plotData.plotdict["cutflow_names"],
			histo_nicks=plotData.plotdict["cutflow_nicks"]
		)
		cutflows = self._align_cuts(
			plotData, cutflows,
			all_cuts=plotData.plotdict["cutflow_sequence"],
			cut_blacklist=plotData.plotdict["cutflow_blacklist"],
		)
		cutflows = self._compile_efficiencies(
			cutflows,
			relative=plotData.plotdict["rel_cuts"],
			base_bin=plotData.plotdict["cutflow_base_bin"],
		)

	def _get_cutflows(self, plotData, histo_names=(), histo_nicks=()):
		"""
		Get all cutflow histograms
		
		:param histo_names: names of cutflow histograms
		:type histo_names: list[str]
		:param histo_nicks: nicks of cutflow histograms
		:type histo_nicks: list[str]
		:returns: candidate histograms by nickname
		"""
		cutflows = {}
		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			# either pick histograms explicitly or guess - cutflow is always TH1
			if (nick in histo_nicks or root_histogram.GetName() in histo_names) or \
				(not (histo_names or histo_nicks) and isinstance(root_histogram, ROOT.TH1)):
				cutflows[nick] = root_histogram
		return cutflows

	def _compile_efficiencies(self, cutflow_histograms, relative=False, base_bin=1):
		"""
		Rescale cutflows to include efficiencies instead of absolute counts

		Histograms are modified inplace.

		:param cutflow_histograms: list of cutflow histograms
		:type cutflow_histograms: list[:py:class:`ROOT.TH1`]
		:param relative: scale each efficiency relative to previous efficiency
		:type relative: bool
		:param base_bin: bin considered to be "all events" - either a bin number or label
		:type base_bin: int or str
		:returns: efficiency histograms by nickname
		"""
		for root_histogram in cutflow_histograms.values():
			if relative:
				for x_bin in xrange(root_histogram.GetNbinsX(), 1, -1):
					new_bin_content = root_histogram.GetBinContent(x_bin)
					if new_bin_content != 0:
						new_bin_content /= root_histogram.GetBinContent(x_bin - 1)
					root_histogram.SetBinContent(x_bin, new_bin_content)
				root_histogram.SetBinContent(1, 1.0)
			else:
				# typecheck for different ROOT interfaces...
				if isinstance(base_bin, basestring):
					root_histogram.Scale(1.0 / root_histogram.GetBinContent(root_histogram.GetXaxis().FindBin(base_bin)))
				else:
					root_histogram.Scale(1.0 / root_histogram.GetBinContent(base_bin))
		return cutflow_histograms

	def _align_cuts(self, plotData, cutflow_histograms, all_cuts=(), cut_blacklist=()):
		"""
		Ensure all cutflows have the same cuts and use the same sequence

		:param plotData: plot settings
		:type plotData: dict
		:param cutflow_histograms: list of cutflow histograms
		:type cutflow_histograms: list[:py:class:`ROOT.TH1`]
		:param all_cuts: alternative sequence of cuts to use
		:type all_cuts: list[str]

		:returns: completed histograms by nickname

		:note: Cutflows are *not* modified inplace, but are replaced in
		       ``plotData`` if cuts are inserted.
		:raises ValueError: if the ordering of ``all_cuts`` conflicts with
		                    any cutflow's sequence of labels
		"""
		# get list of filternames (= bin labels) from all cutflow histograms
		filternames = {}
		for nick, root_histogram in cutflow_histograms.iteritems():
			filternames[nick] = []
			for binnumber in range(1, root_histogram.GetNbinsX()+1):
				filternames[nick].append(root_histogram.GetXaxis().GetBinLabel(binnumber))
		_unique_cutflows = set(tuple(cutlabels) for cutlabels in filternames.values())
		if not all_cuts and not cut_blacklist and len(_unique_cutflows) == 1:
			# no modification required
			return cutflow_histograms
		if not all_cuts and len(_unique_cutflows) > 1:
			log.warning("Cutflow histograms have different number of bins! New histograms containing all cuts will be constructed.")
		# if we can merge desired cuts and individual cuts, subsequences don't have conflicting order
		all_cuts = tools.merge_sequences(all_cuts, *_unique_cutflows)
		if cut_blacklist:
			all_cuts = [cut for cut in all_cuts if cut not in cut_blacklist]
		# recreate histograms with all cuts
		new_cutflows = {}
		for nick, root_histogram in cutflow_histograms.iteritems():
			new_histo = ROOT.TH1F(root_histogram.GetName()+"_new", root_histogram.GetTitle()+"_new", len(all_cuts), 0, len(all_cuts))
			# missing leading cuts are always perfect
			cut_efficiency = 1.
			cut_efficiency_error = 0.
			for index, cut in enumerate(all_cuts):
				# cut exists, reuse it and save it for next bin (if missing)
				if cut in filternames[nick]:
					cut_efficiency = root_histogram.GetBinContent(filternames[nick].index(cut)+1)
					cut_efficiency_error = root_histogram.GetBinError(filternames[nick].index(cut)+1)
				new_histo.SetBinContent(index+1, cut_efficiency)
				new_histo.SetBinError(index+1, cut_efficiency_error)
				new_histo.GetXaxis().SetBinLabel(index+1, cut)
			plotData.plotdict["root_objects"][nick] = new_histo
			new_cutflows[nick] = new_histo
		return new_cutflows

