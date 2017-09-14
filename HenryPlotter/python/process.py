


# -*- coding: utf-8 -*-
import logging
logger = logging.getLogger(__name__)

"""
"""

class Process(object):

	def __init__(self, name, estimation):
		self.name = name
		self.estimation_method = estimation

	def get_name(self):
		return self.name

class Processes(object):
	def __init__(self):
		self.processes = []

	def get_processes(self, *args):
			print args
			return [p for p in self.processes if p.get_name() in args]

	def add(self, process):
		self.processes.append(process)
	
