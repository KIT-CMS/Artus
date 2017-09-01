# -*- coding: utf-8 -*-
import ROOT

"""
"""

#ROOT.ROOT.EnableImplicitMT()

from Artus.HenryPlotter.histogram import *
from Artus.HenryPlotter.cutstring import *

weightA = Weights( Constant("pt_1", "eventweight"), Constant("pt_2", "weightwo"))
weightB = Weights( Constant("weight", "constant"))
weightC = Weights( Constant("0.7", "constant"))

cutA = Cuts(Cut("iso_1<0.15")) 

hA = Histogram( name="testA", nbins=10, variable="pt_1", inputfiles="/home/friese/artus/2017-01-18_eleEsShifts/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8.root", folder="mt_jecUncNom_tauEsNom/ntuple", cuts=cutA, weights=weightA, xlow=0.0, xhigh=100.0)
hB = Histogram( name="testB", variable="pt_1", inputfiles="/home/friese/artus/2017-01-18_eleEsShifts/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8.root", folder="mt_jecUncNom_tauEsNom/ntuple", cuts=cutA, weights=weightB, nbins=10, xlow=0.0, xhigh=100.0)

rO = Root_objects("fileA.root")
rO.add_histogram(hA)
rO.add_histogram(hB)

Histogram.default_nbins=1000
Histogram.default_xlow=0.0
Histogram.default_xhigh=100
Histogram.default_variable="pt_1"
Histogram.default_folder=""

rO.new_histogram( name="testC", inputfiles="/home/friese/artus/2017-01-18_eleEsShifts/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8.root", folder="et_jecUncNom_tauEsNom/ntuple", cuts=cutA, weights=weightC)

rO.produce_classic(processes=3)
rO.save()
