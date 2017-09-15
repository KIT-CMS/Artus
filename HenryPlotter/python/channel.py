

# -*- coding: utf-8 -*-
import ROOT
from Artus.HenryPlotter.cutstring import *
import logging
logger = logging.getLogger(__name__)

"""
"""

class Channel(object):
	def get_name(self):
		return self.name

class EM(Channel):
	def __init__(self):
		self.name = "em"


class MT(Channel):
	def __init__(self):
		self.name = "mt"
	def get_cuts():
		return Cuts(
			Cut("extraelec_veto<0.5", "extraelec_veto"),
			Cut("extramuon_veto<0.5", "extramuon_veto"),
			Cut("againstMuonTight3_2>0.5", "againstMuonTight"),
			Cut("dilepton_veto<0.5", "dilepton_veto"),
			Cut("againstElectronVLooseMVA6_2>0.5","againstElectronVeto"),
			Cut("byTightIsolationMVArun2v1DBoldDMwLT_2>0.5","tau_iso"),
			Cut("iso_1<0.15","muon_iso"),
			Cut("q_1*q_2<0", "os"),
			Cut("trg_singlemuon==1","trg_singlemuon"))

class ET(MT):
	def __init__(self):
		self.name = "et"

	def get_cuts(self):
		return Cuts(
			Cut("extraelec_veto<0.5", "extraelec_veto"),
			Cut("againstMuonLoose3_2>0.5", "againstMuonTight"),
			Cut("dilepton_veto<0.5", "dilepton_veto"),
			Cut("againstElectronTightMVA6_2>0.5","againstElectronVeto"),
			Cut("byTightIsolationMVArun2v1DBoldDMwLT_2>0.5","tau_iso"),
			Cut("iso_1<0.1","ele_iso"),
			Cut("q_1*q_2<0", "os"),
			Cut("trg_singleelectron==1","trg_singleelectron"))

# collection of channels an analysis can be ran on 
class Channels(object):

	def __init__(self, name):
		self.name = name
		self.channels = []

	def add(self, channel):
		self.channels.append(channel)
