
# -*- coding: utf-8 -*-
import ROOT

"""
"""
# import helper classes
# necessary for all analyses
from Artus.HenryPlotter.cutstring import Cut, Cuts
from Artus.HenryPlotter.systematics import Systematics, Systematic
from Artus.HenryPlotter.categories import Category

# Estimation methods, import only what is really necessary
from Artus.HenryPlotter.estimation_methods import Ztt_estimation, Zll_estimation, Data_estimation, TT_estimation, VV_estimation, WJ_estimation, QCD_estimation
from Artus.HenryPlotter.systematic_variations import Nominal, Different_pipeline, Reapply_remove_weight
from Artus.HenryPlotter.era import Run2016BCDEFGH
from Artus.HenryPlotter.process import Process
from Artus.HenryPlotter.channel import MT, ET

# Logging
import logging
import logging.handlers
logger = logging.getLogger("")
logger.setLevel(logging.DEBUG)
handler = logging.StreamHandler()
formatter = logging.Formatter("%(name)s - %(levelname)s - %(message)s")
handler.setFormatter(formatter)
logger.addHandler(handler)


directory = "/storage/b/rcaspart/htautau/2017-06-21_eleScale/"
era=Run2016BCDEFGH()
channel = ET()
ztt = Process("ZTT", Ztt_estimation(era, directory, channel))
zll = Process("ZLL", Zll_estimation(era, directory, channel))
tt =  Process("TT",  TT_estimation(era, directory, channel))
wj =  Process("WJ",  WJ_estimation(era, directory, channel))
vv =  Process("VV",  VV_estimation(era, directory, channel))
data = Process("data", Data_estimation(era, directory, channel))
qcd = Process("QCD", QCD_estimation(era, directory, channel, [ztt, zll, tt, vv, wj], data))

# systematic variations. Start with "nominal" for the central values without any variation
nominal = Nominal()

jec_downshift = Different_pipeline("jec", "jecUncDown_tauEsNom", "Down")
jec_upshift = Different_pipeline("jec", "jecUncUp_tauEsNom", "Up")

#definition of categories
nobtag_tight_mt = Category( "nobtag_tight", channel, Cuts(
	Cut("nbtag==0", "nobtag"),
	Cut("mt_1<40","mt")),
	variable="pt_1", nbins=50, xlow=0, xhigh=100)

nobtag_tight_et = Category( "nobtag_tight", channel, Cuts(
	Cut("nbtag==0", "nobtag"),
	Cut("mt_1<40","mt")),
	variable="pt_1", nbins=50, xlow=0, xhigh=100)


#systematics object, to be filled
systematics = Systematics()
# first, create the nominals
systematics.add( Systematic(category=nobtag_tight_et, process=data, channel=channel, analysis = "example", era=era, syst_var=nominal))
systematics.add( Systematic(category=nobtag_tight_et, process=ztt,  channel=channel, analysis = "example", era=era, syst_var=nominal))
systematics.add( Systematic(category=nobtag_tight_et, process=zll,  channel=channel, analysis = "example", era=era, syst_var=nominal))
systematics.add( Systematic(category=nobtag_tight_et, process=tt,   channel=channel, analysis = "example", era=era, syst_var=nominal))
systematics.add( Systematic(category=nobtag_tight_et, process=vv,   channel=channel, analysis = "example", era=era, syst_var=nominal))
systematics.add( Systematic(category=nobtag_tight_et, process=wj,   channel=channel, analysis = "example", era=era, syst_var=nominal))
systematics.add( Systematic(category=nobtag_tight_et, process=qcd,  channel=channel, analysis = "example", era=era, syst_var=nominal))

systematics.produce()

systematics.summary()
