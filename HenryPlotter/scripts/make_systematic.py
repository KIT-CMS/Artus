
# -*- coding: utf-8 -*-
import ROOT

"""
"""

from Artus.HenryPlotter.cutstring import Cut, Cuts
from Artus.HenryPlotter.systematics import Systematics, Systematic
from Artus.HenryPlotter.categories import Category
from Artus.HenryPlotter.estimation_methods import Ztt_estimation, Data_estimation_mt
from Artus.HenryPlotter.systematic_variations import Nominal, Different_pipeline, Reapply_remove_weight
from Artus.HenryPlotter.era import Run2016BCDEFG
from Artus.HenryPlotter.process import Process
from Artus.HenryPlotter.channel import MT

import logging
import logging.handlers
logger = logging.getLogger("")
logger.setLevel(logging.DEBUG)
handler = logging.StreamHandler()
formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s - %(message)s")
handler.setFormatter(formatter)
logger.addHandler(handler)

jec_downshift = Different_pipeline("jec", "jecUncDown_tauEsNom", "Down")
jec_upshift = Different_pipeline("jec", "jecUncUp_tauEsNom", "Up")
#zPtReweightWeight = Reapply_remove_weight("zPtReweightWeight", "zPtReweightWeight")
nominal = Nominal()

# todo: systematic shift class

#data_estimation = Data("data", folder="jecUncNom_tauEsNom")
#ztt_estimation = Ztt()
#zll_estimation = Zll()
#wj_estimation = Wj_from_SS_OS()
#tt_estimation = TT()
#vv_estimation = VV()
#simple_qcd_estimation = QCD()

era=Run2016BCDEFG()
ztt = Process("ZTT", Ztt_estimation(era))
data = Process("data", Data_estimation_mt())
mt = MT()


#definition of categories
some_category = Category( "some", Cuts(Cut("pt_2>40"), Cut("mt_1<50", "mt"), Cut("q_1*q_2<0", "os")), "m_vis", nbins=40, xlow=0, xhigh=150)

#systematics object, to be filled
systematics = Systematics()
# first, create the nominals
systematics.add( Systematic(category=some_category, process=data, channel=mt, analysis = "example", era=era, syst_var=nominal))
#systematics.add(data)
systematics.add( Systematic(category=some_category, process=ztt, channel=mt, analysis = "example", era=era, syst_var=nominal))
#zll  = Systematic(category=some_category, process="zll",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=nominal, estimation_method = zll_estimation)
#systematics.add(zll)
#wj  = Systematic(category=some_category, process="wj",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=nominal, estimation_method = wj_estimation)
#systematics.add(wj)
#wj_jec_upshift  = Systematic(category=some_category, process="wj",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=jec_upshift, estimation_method = wj_estimation)
#wj_jec_downshift  = Systematic(category=some_category, process="wj",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=jec_downshift, estimation_method = wj_estimation)
#systematics.add(wj_jec_upshift)
#systematics.add(wj_jec_downshift)

#tt  = Systematic(category=some_category, process="tt",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=nominal, estimation_method = tt_estimation)
#systematics.add(tt)

#vv  = Systematic(category=some_category, process="vv",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=nominal, estimation_method = vv_estimation)
#systematics.add(vv)

#qcd  = Systematic(category=some_category, process="qcd",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=nominal, estimation_method = simple_qcd_estimation)
#systematics.add(qcd)
#qcd_jec_down  = Systematic(category=some_category, process="qcd",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=jec_downshift, estimation_method = simple_qcd_estimation)
#systematics.add(qcd_jec_down)
#qcd_jec_up  = Systematic(category=some_category, process="qcd",  channel="mt", analysis = "example", era="2017", mass=None, syst_var=jec_upshift, estimation_method = simple_qcd_estimation)
#systematics.add(qcd_jec_up)
# add e.g. add additional categories

# add the systematic variations
#ztt_jec_shift  = Systematic(category=some_category, process="ztt", channel="mt", analysis = "example", era="2017", mass=None, syst_var=jec_shifts, estimation_method = ztt_estimation)
#systematics.add(ztt_jec_shift)
#ztt_zpt_shift  = Systematic(category=some_category, process="ztt", channel="mt", analysis = "example", era="2017", mass=None, syst=zPtReweightWeight, estimation_method = ztt_estimation)
#systematics.add(ztt_zpt_shift)

systematics.produce()

systematics.summary()
