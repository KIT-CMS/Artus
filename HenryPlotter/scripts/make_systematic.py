
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
# et
et = ET()
ztt_et = Process("ZTT", Ztt_estimation(era, directory, et))
zll_et = Process("ZLL", Zll_estimation(era, directory, et))
tt_et =  Process("TT",  TT_estimation(era, directory, et))
wj_et =  Process("WJ",  WJ_estimation(era, directory, et))
vv_et =  Process("VV",  VV_estimation(era, directory, et))
data_et = Process("data", Data_estimation(era, directory, et))
qcd_et = Process("QCD", QCD_estimation(era, directory, et, [ztt_et, zll_et, tt_et, vv_et, wj_et], data_et))

mt = MT()
ztt_mt = Process("ZTT", Ztt_estimation(era, directory, mt))
zll_mt = Process("ZLL", Zll_estimation(era, directory, mt))
tt_mt =  Process("TT",  TT_estimation(era, directory, mt))
wj_mt =  Process("WJ",  WJ_estimation(era, directory, mt))
vv_mt =  Process("VV",  VV_estimation(era, directory, mt))
data_mt = Process("data", Data_estimation(era, directory, mt))
qcd_mt = Process("QCD", QCD_estimation(era, directory, mt, [ztt_mt, zll_mt, tt_mt, vv_mt, wj_mt], data_mt))
# systematic variations. Start with "nominal" for the central values without any variation
nominal = Nominal()

jec_downshift = Different_pipeline("jec", "jecUncDown_tauEsNom", "Down")
jec_upshift = Different_pipeline("jec", "jecUncUp_tauEsNom", "Up")

#definition of categories
nobtag_tight_mt = Category( "nobtag_tight", MT(), Cuts(
	Cut("nbtag==0", "nobtag"),
	Cut("mt_1<40","mt")),
	variable="pt_1", nbins=50, xlow=0, xhigh=100)

nobtag_tight_et = Category( "nobtag_tight", ET(), Cuts(
	Cut("nbtag==0", "nobtag"),
	Cut("mt_1<40","mt")),
	variable="pt_1", nbins=50, xlow=0, xhigh=100)


#systematics object, to be filled
systematics = Systematics()
# first, create the nominals
for process in [ztt_mt, zll_mt, tt_mt, vv_mt, wj_mt, data_mt, qcd_mt]:
	systematics.add( Systematic(category=nobtag_tight_mt, process=process, analysis = "example", era=era, syst_var=nominal))

for process in [ztt_et, zll_et, tt_et, vv_et, wj_et, data_et, qcd_et]:
	systematics.add( Systematic(category=nobtag_tight_et, process=process, analysis = "example", era=era, syst_var=nominal))

systematics.produce()
systematics.summary()
