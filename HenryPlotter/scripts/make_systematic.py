
# -*- coding: utf-8 -*-
import ROOT

"""
"""

from Artus.HenryPlotter.histogram import *
from Artus.HenryPlotter.cutstring import *
from Artus.HenryPlotter.systematics import *
from Artus.HenryPlotter.categories import *
from Artus.HenryPlotter.estimation_methods import *
from Artus.HenryPlotter.systematic_variations import *


jec_shifts = Different_pipeline("jec", "jecUncDown_tauEsNom", "jecUncUp_tauEsNom")
zPtReweightWeight = Reapply_remove_weight("zPtReweightWeight", "zPtReweightWeight")

# todo: systematic shift class

data_estimation = Data("data", folder="jecUncNom_tauEsNom")
ztt_estimation = Ztt()
wj_estimation = Wj()

#definition of categories
some_category = Category( "some", Cuts(Cut("pt_2>40"), Cut("mjj>100"), Cut("mt_1<50", "mt"), Cut("q_1*q_2<0", "os")), "m_vis", nbins=40, xlow=0, xhigh=150)

#systematics object, to be filled
systematics = Systematics()
# first, create the nominals
data = Systematic(category=some_category, process="data", channel="mt", analysis = "example", era="2017", mass=None, syst=None, estimation_method = data_estimation)
#systematics.add(data)
ztt  = Systematic(category=some_category, process="ztt",  channel="mt", analysis = "example", era="2017", mass=None, syst=None, estimation_method = ztt_estimation)
#systematics.add(ztt)
wj  = Systematic(category=some_category, process="wj",  channel="mt", analysis = "example", era="2017", mass=None, syst=None, estimation_method = wj_estimation)
systematics.add(wj)

# add e.g. add additional categories

# add the systematic variations
ztt_jec_shift  = Systematic(category=some_category, process="ztt", channel="mt", analysis = "example", era="2017", mass=None, syst=jec_shifts, estimation_method = ztt_estimation)
#systematics.add(ztt_jec_shift)
ztt_zpt_shift  = Systematic(category=some_category, process="ztt", channel="mt", analysis = "example", era="2017", mass=None, syst=zPtReweightWeight, estimation_method = ztt_estimation)
#systematics.add(ztt_zpt_shift)

systematics.produce()

systematics.summary()
