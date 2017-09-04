# -*- coding: utf-8 -*-
import ROOT

"""
"""
from datetime import datetime
start=datetime.now()
ROOT.ROOT.EnableImplicitMT(30)

from Artus.HenryPlotter.histogram import *
from Artus.HenryPlotter.cutstring import *

weightA = Weights( Constant("pt_1", "eventweight"), Constant("pt_2", "weightwo"))
weightB = Weights( Constant("1.0", "constant"))
weightC = Weights( Constant("0.7", "constant"))

cutA = Cuts(Cut("pt_1>30")) 

#hA = Histogram( name="testA", nbins=10, variable="pt_1", inputfiles="/home/friese/artus/2017-01-18_eleEsShifts/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8.root", folder="mt_jecUncNom_tauEsNom/ntuple", cuts=cutA, weights=weightA, xlow=0.0, xhigh=100.0)
#hB = Histogram( name="testB", variable="pt_1", inputfiles="/home/friese/artus/2017-01-18_eleEsShifts/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8.root", folder="mt_jecUncNom_tauEsNom/ntuple", cuts=cutA, weights=weightB, nbins=10, xlow=0.0, xhigh=100.0)

cA = Count( name="varA", inputfiles="/home/friese/artus/2017-01-18_eleEsShifts/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8.root", folder="mt_jecUncNom_tauEsNom/ntuple", cuts=cutA, weights=weightC)

rO = Root_objects("fileA.root")
#rO.add(hA)
#rO.add(hB)
#rO.add(cA)

Histogram.default_nbins=1000
Histogram.default_xlow=0.0
Histogram.default_xhigh=100
Histogram.default_variable="pt_1"
Histogram.default_folder=""
#inputfilesA="/home/friese/artus/2017-01-18_eleEsShifts/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8.root"
#inputfilesA=["/home/friese/artus/2017-01-02_longRun/SingleMuon_Run2016B_PromptRecov2_13TeV_MINIAOD/SingleMuon_Run2016B_PromptRecov2_13TeV_MINIAOD.root", "/home/friese/artus/2017-01-02_longRun/SingleMuon_Run2016C_PromptRecov2_13TeV_MINIAOD/SingleMuon_Run2016C_PromptRecov2_13TeV_MINIAOD.root"]
inputfilesA=[ "/home/friese/artus/2017-01-02_longRun/SingleMuon_Run2016C_PromptRecov2_13TeV_MINIAOD/SingleMuon_Run2016C_PromptRecov2_13TeV_MINIAOD.root"]
#rO.new_histogram( name="testC", inputfiles="/home/friese/artus/2017-01-18_eleEsShifts/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8.root", folder="et_jecUncNom_tauEsNom/ntuple", cuts=cutA, weights=weightC)
#rO.new_count( name="varC", inputfiles="/home/friese/artus/2017-01-18_eleEsShifts/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8/GluGluHToTauTauM125_RunIISpring16MiniAODv2reHLT_PUSpring16RAWAODSIM_13TeV_MINIAOD_powheg-pythia8.root", folder="mt_jecUncNom_tauEsNom/ntuple", cuts=cutA, weights=weightC)
from string import letters
#for a in range(len(letters)):
for a in [8,9,10]:
	name = "varWeighted" + letters[a]
	rO.new_count( name=name, inputfiles=inputfilesA, folder="mt_jecUncNom_tauEsNom/ntuple", cuts=Cuts(Cut("pt_1>"+str(float(a)+20))), weights=Weights(Weight(str(a), str(a))))
	name = "var" + letters[a]
	rO.new_count( name=name, inputfiles=inputfilesA, folder="mt_jecUncNom_tauEsNom/ntuple", cuts=Cuts(Cut("pt_1>"+str(float(a)+20))), weights=weightC)
	name = "h" + letters[a]
	rO.new_histogram( name=name, inputfiles=inputfilesA, folder="mt_jecUncNom_tauEsNom/ntuple", cuts=Cuts(Cut("pt_1>"+str(float(a)+20))), weights=weightB)

#rO.produce_classic(processes=10)
rO.produce_tdf()
rO.save()
print datetime.now()-start
