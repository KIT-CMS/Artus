
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


jec_downshift = Different_pipeline("jec", "jecUncDown_tauEsNom", "Down")
jec_upshift = Different_pipeline("jec", "jecUncUp_tauEsNom", "Up")
#zPtReweightWeight = Reapply_remove_weight("zPtReweightWeight", "zPtReweightWeight")
nominal = Nominal()

# todo: systematic shift class

data_estimation = Data("data", folder="jecUncNom_tauEsNom")
ztt_estimation = Ztt()
zll_estimation = Zll()
wj_estimation = Wj()
tt_estimation = TT()
vv_estimation = VV()
simple_qcd_estimation = QCD()

#definition of categories
some_category = Category( "some", Cuts(Cut("pt_2>40"), Cut("mjj>100"), Cut("mt_1<50", "mt"), Cut("q_1*q_2<0", "os")), "m_vis", nbins=40, xlow=0, xhigh=150)

#systematics object, to be filled
systematics = Systematics()
# first, create the nominals
data = Systematic(category=some_category, process="data", channel="mt", analysis = "example", era="2017", mass=None, syst_var=nominal, estimation_method = data_estimation)
#systematics.add(data)
ztt  = Systematic(category=some_category, process="ztt",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=nominal, estimation_method = ztt_estimation)
systematics.add(ztt)
zll  = Systematic(category=some_category, process="zll",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=nominal, estimation_method = zll_estimation)
systematics.add(zll)
wj  = Systematic(category=some_category, process="wj",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=nominal, estimation_method = wj_estimation)
systematics.add(wj)
wj_jec_upshift  = Systematic(category=some_category, process="wj",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=jec_upshift, estimation_method = wj_estimation)
wj_jec_downshift  = Systematic(category=some_category, process="wj",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=jec_downshift, estimation_method = wj_estimation)
systematics.add(wj_jec_upshift)
systematics.add(wj_jec_downshift)

tt  = Systematic(category=some_category, process="tt",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=nominal, estimation_method = tt_estimation)
systematics.add(tt)

vv  = Systematic(category=some_category, process="vv",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=nominal, estimation_method = vv_estimation)
systematics.add(vv)

qcd  = Systematic(category=some_category, process="qcd",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=nominal, estimation_method = simple_qcd_estimation)
systematics.add(qcd)
# add e.g. add additional categories

# add the systematic variations
#ztt_jec_shift  = Systematic(category=some_category, process="ztt", channel="mt", analysis = "example", era="2017", mass=None, syst_var=jec_shifts, estimation_method = ztt_estimation)
#systematics.add(ztt_jec_shift)
#ztt_zpt_shift  = Systematic(category=some_category, process="ztt", channel="mt", analysis = "example", era="2017", mass=None, syst=zPtReweightWeight, estimation_method = ztt_estimation)
#systematics.add(ztt_zpt_shift)

systematics.produce()

systematics.summary()
