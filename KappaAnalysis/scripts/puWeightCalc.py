#!/usr/bin/env python
# -*- coding: utf-8 -*-
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True  # prevents Root from reading argv
import os
import argparse
import subprocess
import copy
import array
import sys
import difflib

"""
This is a tool for reweighting MC to match pile-up in data.

It requires: python, ROOT, pileupCalc.py (CMSSW 5.2 or above).
It calculates the pu distribution in data with the official tool.
Then it calculates the pu distribution from a skim.
From these distributions it calculates the weights and saves them to a json
file. Data and MC distributions are saved to root files.
Pleas use --help or read the options function for further information.
"""
ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kError


def main():
	op = options()
	mc, data = None, None
	if not op.dfile and not op.mfile:
		print  "No data nor MC files"
		print
		print  "Usage: weightCalc.py [options] datainput|mcinput [...]"
		print  "Use -h for detailed help. This pileupCalc is used:"
		subprocess.call(['which', 'pileupCalc.py'])
		sys.exit(1)


	if not op.dfile:
		print  "\n No data given."
	elif not op.inputLumiJSON:
		print  "\n Read data distribution from file", op.dfile;
		data = getDistributionFromFile(op.dfile, op.data_histo)
	else:
		print  "\n Calculate data distribution from pileupCalc.py";
		data = getDataFromPileupCalc(op.dfile, op.inputLumiJSON, op.dataoutput,
				op.minBiasXsec, op.maxPileupBin, op.numPileupBins, histo="pileup")
		if op.save:
			saveDistributionToFile(op.dataoutput, data, op.data_histo, op.force)

	# get pileup distribtions from input
	if not op.mfile:
		print  "\n No MC given."
	elif len(op.mfile) == 1:
		print  "\n Read MC distribution from file", op.mfile[0]
		mc = getDistributionFromFile(op.mfile[0], op.mc_histo)
	else:
		print  "\n Calculate MC distribution from skim. len(op.mfile):", len(op.mfile)
		mc = getDistributionFromSkim(op.mfile)
		if op.save:
			saveDistributionToFile(op.mcoutput, mc, op.mc_histo, op.force)


	if data and mc:
		print  "Bins (data, mc)", data.GetNbinsX(), mc.GetNbinsX()
		# use mc to data bin count? this may break later for bin count factors != 2 - MF20151023
		if data.GetNbinsX() > 1000:
			print "rebinned data"
			data.Rebin()
		if mc.GetNbinsX() > 1000:
			mc.Rebin()
			print "rebinned mc"
		weights = calcWeights(data, mc, verbose=op.verbose, warn=not op.no_warning, rebin=op.rebin, binning=op.binning, weight_limits=op.weight_limits)
		saveDistributionToFile(op.output, weights, histoname=None, force=True)
	else:
		print  "No weights calculated."
		sys.exit(1)


def calcWeights(data, mc, verbose=False, warn=True, rebin=False, binning=(), weight_limits=(float("-inf"), float("inf"))):
	# checks
	n = data.GetNbinsX()
	if n != mc.GetNbinsX():
		print  'Number of bins in input histograms is not compatible.'
	print  "Weight calculation ..."

	if rebin:
		print  "rebin:", binning
		for l in binning:
			for a, b in zip(l[:-1], l[1:]):
				x1 = data.FindBin(a)
				x2 = data.FindBin(b)
				summc = sum([mc.GetBinContent(i) for i in range(x1, x2)]) / (x2 - x1)
				sumdata = sum([data.GetBinContent(i) for i in range(x1, x2)]) / (x2 - x1)
				for i in range(x1, x2):
					data.SetBinContent(i, sumdata)
					mc.SetBinContent(i, summc)

	bin_centers = [data.GetBinCenter(i) for i in range(1, n+1)]
	if warn:
		missing_data, missing_mc, missing_both = [], [], set()
		for i in range(1, n + 1):
			if data.GetBinContent(i) <= 0:
				missing_data.append(data.GetBinCenter(i))
			if mc.GetBinContent(i) <= 0:
				missing_mc.append(mc.GetBinCenter(i))
		if missing_data and missing_mc:
			missing_both.update(missing_data)
			missing_both.intersection(missing_mc)
			print  "WARNING: No data & mc events with npu in", format_bin_ranges(sorted(missing_both), bin_centers)
		if missing_data:
			print  "WARNING: No data events with npu in", format_bin_ranges([npu for npu in missing_data if not npu in missing_both], bin_centers)
		if missing_mc:
			print  "WARNING: No mc events with npu in", format_bin_ranges([npu for npu in missing_mc if not npu in missing_both], bin_centers)

	# calculate weights
	data.Scale(1.0 / data.Integral())
	mc.Scale(1.0 / mc.Integral())
	weights = data.Clone('pileup')
	weights.SetTitle('pileup weights;n_{PU}^{truth};weight')
	weights.Divide(mc)

	# warn of excessive weights
	min_weight, max_weight = weight_limits
	min_bins, max_bins = [], []
	for i in range(1, weights.GetNbinsX() + 1):
		if weights.GetBinContent(i) > max_weight:
			weights.SetBinContent(i, max_weight)
			max_bins.append(weights.GetBinCenter(i))
		elif weights.GetBinContent(i) < min_weight:
			weights.SetBinContent(i, min_weight)
			min_bins.append(weights.GetBinCenter(i))
	if min_bins:
		print "WARNING: Bins truncated to lower limit (%f) in" % min_weight, format_bin_ranges([npu for npu in min_bins], bin_centers)
	if max_bins:
		print "WARNING: Bins truncated to upper limit (%f) in" % max_weight, format_bin_ranges([npu for npu in max_bins], bin_centers)
	# check efficiency of weights for matching MC to data
	mc.Multiply(weights)
	emit_data("weight_efficiency", mc.Integral())
	return weights


def getDataFromPileupCalc(files, lumijson, outfile, minBiasXsec=73500, maxPileupBin=80, numPileupBins=1600, histo="pileup"):
	"""
	Get the true Data PU from the official PU-json

	The "true" (or "observed") PU in data is estimated using this method:
	https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData
	1. pileupCalc is run (if json is given)
	2. the distribution is read from a rootfile
	"""
	cmd = ["python", "pileupCalc.py", "-i", files, "--inputLumiJSON", lumijson, "--calcMode", "true", "--minBiasXsec", str(minBiasXsec), "--maxPileupBin", str(maxPileupBin), "--numPileupBins", str(numPileupBins), outfile]
	print "Using:", subprocess.check_output(['which', 'pileupCalc.py']).strip()
	print " ".join(cmd)
	subprocess.call(cmd)
	result = getDistributionFromFile(outfile, histo)
	return result


def getDistributionFromSkim(filelist, nmax=80, save=True, bins=1600, histoname="pileup"):
	print "\t MC pile-up distribution from skim: " + filelist[0] + ", ... (" + str(len(filelist)) + " files)"
	chain = ROOT.TChain("Events")
	for file_idx, file_name in enumerate(filelist):
		# print "add:", file_name
		print "Added %4d/%4d\r" % (file_idx + 1, len(filelist)),
		chain.Add(file_name)
	print "\n\t Files added, chain.GetEntries:", chain.GetEntries()," starting to fill...",
	result = ROOT.TH1D(histoname, "True Number of Pile-Up Interactions;nputruth;events", bins, 0.0, nmax)
	chain.Draw("eventInfo.nPUMean >> " + histoname)
	print "\n\t resulting histogram of eventInfo.nPUMean:"; result.Print();
	return result


def saveDistributionToFile(filename, histo, histoname=None, force=False):
	if os.path.isfile(filename):
		if force:
			os.remove(filename)
		else:
			print "File %s exist and overwriting is not set. Use existing." % (filename)
			return

	if not os.path.exists(os.path.dirname(filename)):
		print "Creating output directory", os.path.dirname(filename)
		os.makedirs(os.path.dirname(filename))

	rootfile = ROOT.TFile(filename, "RECREATE")
	if histoname is not None:
		print 'histoname:', histoname
		histo.SetName(histoname)
	histo.Write()
	rootfile.Close()
	print filename, "written"


def getDistributionFromFile(filename, histoname="pileup"):
	rootfile = ROOT.TFile(filename, "READONLY")
	if not rootfile.IsOpen():
		print "The file", filename, "could not be opened."
		sys.exit(1)
	histo = rootfile.Get(histoname)
	if not histo:
		print "The histogram %s could not be found in %s." % (histoname, filename)
		sys.exit(1)
	return copy.deepcopy(histo)


# Formatting/Output
def emit_data(key, value):
	"""Write a key/value pair for machine processing"""
	print >> sys.stdout, str(key) + " = " + repr(value)


def format_bin_ranges(bins, all_bins=None, fmt_str="%.2f"):
	"""
	Format ranges of bins to be more readable

	If `all_bins` is given, it must be a superset of `bins`. It allows
	consecutive bins to be joined, e.g. 1, 2, 3 becomes 1-3.
	"""
	if all_bins is None or len(bins) <= 1:
		bin_strs = [fmt_str % binc for binc in bins]
	else:
		bin_strs = []
		for bin_idx, all_bin_idx, match_len in difflib.SequenceMatcher(a=bins, b=all_bins).get_matching_blocks():
			if match_len == 0:
				continue
			if match_len <= 2:
				bin_strs.append(fmt_str % bins[bin_idx])
			if match_len == 2:
				bin_strs.append(fmt_str % bins[bin_idx+1])
			elif match_len > 2:
				bin_strs.append((fmt_str % bins[bin_idx]) + "-" + (fmt_str % bins[bin_idx+match_len-1]))
	return "[" + ", ".join(bin_strs) + "]"


def options():
	parser = argparse.ArgumentParser(
		description="""
		%(prog)s calculates the weights for MC reweighting to match the number of
		pile-up interactions in data. Use cases:
		  %(prog)s Cert_JSON.txt /mc/skim/*.root -i pileup_JSON.txt [options]
		  %(prog)s datadist.root mcdist.root [options]
		""".strip(),
		epilog="""
		%(prog)s may output warnings to stderr. These are for diagnostics only and
		formatted to be human readable.
		Additional data is written exclusively to stdout. These are machine readable
		key-value pairs separated by ' = ' using repr(value), e.g.
		foo_bar = "quax string value".
		""".strip(),
		formatter_class=argparse.RawDescriptionHelpFormatter)
	parser.add_argument('files', metavar='input', type=str, nargs='+',
		help="""
		input files as [data] [mc, [mc, [...]]] - 'data' may be a root file
		with the distribution of pile-up interactions in data or a run json file
		(if a pile-up json is specified with '-i'). 'mc' may be a root file
		with the distribution of pile-up in MC or a set of skim files. For
		data/mc distribution files, see also '-M'/'-D'.
		""")
	#Pileup calc options:
	parser.add_argument('-M', '--mc-histo', type=str, default="pileup",
		help="histogram name of the pile-up distribution in the MC file %(default)s")
	parser.add_argument('-D', '--data-histo', type=str, default="pileup",
		help="histogram name of the pile-up distribution in the data file %(default)s")
	parser.add_argument('-d', '--dataoutput', type=str, default="",
		help="Output file for the data distribution. [Default: from data name]")
	parser.add_argument('-m', '--mcoutput', type=str, default="",
		help="Output file for the MC distribution. [Default: from mc name]")
	parser.add_argument('-o', '--output', type=str, default="data/pileup",
		help="Output path for the calculated weights.")
	parser.add_argument('-s', '--save', action='store_true',
		help="Save data and MC distributions.")

	parser.add_argument('-i', '--inputLumiJSON', type=str, default=None,
		help="Input Lumi JSON for pileupCalc.")
	parser.add_argument('-x', '--minBiasXsec', type=float, default=69400,
		help="Minimum bias cross section in nb. [Default: %(default)s mb] NB: pileupCalc takes ub!")
	parser.add_argument('-n', '--maxPileupBin', type=int, default=80,
		help="Maximum number of pile-up interactions. [default: %(default)s]")
	parser.add_argument('-b', '--numPileupBins', type=int, default=800,
		help="Number of desired pile-up bins. [default: %(default)s].")
	parser.add_argument('-r', '--rebin', action='store_true',
		help="merge low and high range bins")
	parser.add_argument('-w', '--weight-limits', nargs="*", default=[0, 4], type=float,
		help="Limit excessive weights. No value: no limits. One value: max if >1, "
			 "else min. Two values: min and max. [Default: %(default)s]")
	parser.add_argument('-v', '--verbose', action='store_true',
		help="verbosity")
	parser.add_argument('-c', '--check', action='store_true',
		help="check whether the weights are correctly normalized. If no MC "
			"events are availabe for some numbers of pile-up interactions, "
			"the distribution cannot be correctly normalized and the weights "
			"do not exactly add up to unity.")
	parser.add_argument('-q', '--no-warning', action='store_true',
		help="Do not print warnings if the Monte Carlo sample does not "
			"contain events for all numbers of pile-up interactions.")
	parser.add_argument('-l', '--label', type=str, default="n_{{PU}}^{{truth}}",
		help="x-axis label in histograms")

	parser.add_argument('--dry',  action='store_true',
		help="exit fast")
	parser.add_argument('-f', '--force',  action='store_true',
		help="force recreation during safe")
	args = parser.parse_args()

	# distribute files to data (1 file) and mc (rest)
	args.dfile = None
	args.mfile = []
	if args.files:
		args.dfile = args.files[0]
		args.mfile = args.files[1:]
		if len(args.files) == 2:  # data[txt, root], mc
			if args.files[1].endswith('.txt'):  # data[txt, root], mc.txt
				with open(args.files[1]) as f:
				    content = f.readlines()
				# you may also want to remove whitespace characters like `\n` at the end of each line
				content = [x.strip() for x in content]
				args.mfile = content
			elif args.files[1].endswith('.root'):  # data[txt, root], mc.root
				args.mfile = [args.files[1]]
		if len(args.files) > 2 and args.files[0][:8] == args.files[1][:8]:  # {mc.root}
			args.dfile = ""
			args.mfile = args.files

	# if not args.dfile or not args.mfile:
	# 	args.save = True

	# construct default names from (JSON) input files
	# data: data_190456-208686_8TeV_22Jan2013ReReco.root
	if not args.dataoutput:
		if args.inputLumiJSON:
			args.dataoutput = os.path.basename(args.dfile)
			args.dataoutput = args.dataoutput[5:args.dataoutput.find("Collision") - 1]
			args.dataoutput = "data_" + args.dataoutput + ".root"
		elif args.dfile:
			args.dataoutput = os.path.basename(args.dfile)

	# mc: mc_kappa539_MC12_madgraph.root
	if not args.mcoutput:
		if len(args.mfile) > 1:
			if len(args.files) == 2:
				args.mcoutput = "mc_" + args.files[1].split('/')[-1].strip('.txt') + ".root"
			else:
				args.mcoutput = args.mfile[0].split('/')[-2]
				args.mcoutput = "mc_" + args.mcoutput + ".root"
		elif len(args.mfile) == 1:
			args.mcoutput = args.mfile[0].split('/')[-1]

	# weights: weights_190456-208686_8TeV_22Jan2013ReReco_kappa539_MC12_madgraph.root
	if not args.output.endswith('.root'):
		args.output = args.output + '/' + args.dataoutput.replace(".root", "").replace("data_", "weights_") + '_' + args.mcoutput.replace("mc_", "")
	print 'args.output:', args.output
	print 'args.mcoutput:', args.mcoutput
	print 'args.dataoutput:', args.dataoutput
	if  not type(args.mfile) is list:
		print 'args.mfile:', args.mfile
	print 'args.dfile:', args.dfile

	# if rebinning is activated, low stat bins are merged
	args.binning = []
	if args.rebin:
		# ranges to merge and new binning to use for them - others are left untouched
		args.binning = [[0, 1, 2, 3.5, 5], range(45, args.maxPileupBin + 1)]

	# ensure all weight limits are present
	if not args.weight_limits:
		args.weight_limits = (float("-inf"), float("inf"))
	if len(args.weight_limits) == 1:
		args.weight_limits = (float("-inf"), args.weight_limits[0]) if args.weight_limits[0] > 1 else (args.weight_limits[0], float("inf"))
	if len(args.weight_limits) > 2:
		print "WARNING: ignoring additional weight limits",
		args.weight_limits = args.weight_limits[:2]
	assert args.weight_limits[0] < args.weight_limits[1], "Lower weight limit must be smaller than upper limit"

	if args.verbose:
		print "Using options:"
		print args

	if args.dry:
		print 'bye!'
		exit(1)

	return args


if __name__ == "__main__":
	main()
