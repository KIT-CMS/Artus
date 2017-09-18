# -*- coding: utf-8 -*-
import sys
import logging
logger = logging.getLogger(__name__)
"""
"""

# Cut -> base class for sorting them
# Cuts -> holder for selection steps
# Weight -> Weight, constant
# Weights -> holder for weight expression


supported_operators = ['<', '>', '&&', '||', '==', '!=']
inverted_operators =  ['>', '<', '||', '&&', '!=', '==']

# Base class all others inherit from.
# Usage: For any weight string that does not follow one of the special definitions listed below
class Weight(object):

	def __init__(self, weightstring, name=False):
		self.weightstring = weightstring
		self.name = name
		if name == False:
			logger.fatal( "No appropriate name has been assigned to weight with value %s. Please use an explicit name for this weight string. \n Aborting.", str(weightstring))
			raise ValueError
		logger.debug("Created %s object with name \"%s\" and weightstring \"%s\"", self, self.get_name(), self.extract())
			

	def get_name(self):
		return self.name

	def extract(self):
			return self.embrace(self.weightstring)

	def embrace(self, c):
		return "(" + c + ")"


# Class for a constant number, e.g. '0.9' or a constant variable e.g. "generatorWeight"
class Constant(Weight):
	def __init__(self, weightstring, name=False):
		self.is_float = False
		self.weightstring = weightstring
		try:
			float(weightstring)
			is_float = True
			self.name = name
		except:
			self.name = weightstring if name==False else name

		if name == False:
			logger.fatal( "No appropriate name has been assigned to weight with value %s. Please use an explicit name for this weight string. \n Aborting.", str(weightstring))
			raise ValueError
		logger.debug("Created %s object with name \"%s\" and string \"%s\"", self, self.get_name(), self.extract())

	def invert(self):
		if is_float:
			self.weightstring = str(1.0/float(self.weightstring))
		else:
			if self.weightstring.startswith("1.0/"):
				self.weightstring = self.weightstring.replace("1.0/", "")
			else:
				self.weightstring = "1.0/"+self.weightstring
		logger.debug("Inverted Constant object %s with name \"%s\", value is now \"%s\"", self, self.get_name(), self.extract())

# holder class for weight/cutstring objects
class Weights(object):
	def __init__(self, *args):
		self.weightstrings = []
		if args!=False:
			for w in args:
				self.add(w, False)
		self.log()

	def add(self, weightstring, verbose=False):
		if (issubclass(type(weightstring), Weight)):
			if weightstring.get_name() in self.get_names():
				logger.fatal("Not possible to add the weightstring %s since its name is not unique. Aborting.", cutstring)
				raise LookupError
			else:
				self.weightstrings.append(weightstring)
		if verbose:
			self.log()

	def extract(self):
		if len(self.weightstrings) > 0:
			full_weightstring = "*".join([c.extract() for c in self.weightstrings])
			return full_weightstring
		else:
			return "(1.0)"

	def get_names(self):
		return [w.get_name() for w in self.weightstrings]

	def get(self, name):
		for w in self.weightstrings:
			if w.get_name() == name:
				return w
		logger.fatal("The name \"%s\" is not part of this Weichts object.", name)
		logger.fatal("Avilable names are: %s", [w.get_name() for w in self.weightstrings])
		raise KeyError

	def remove(self, name):
		if name in self.get_names():
			self.weightstrings = [w for w in self.weightstrings if not w.get_name() == name]
		else:
			logger.fatal("Error while trying to remove weightstring with key \"%s\"", name)
			raise KeyError
		return self

	def square(self, name):
		new = self.get(name)
		new.name = new.name+"2"
		self.weightstrings.append(new)
		return self

	def log(self):
		logger.debug("Weights object %s now holds the weights %s", self, self.get_names())

# Class for a simple cut expression e.g. 'pt_1>22'
class Cut():

	def __init__(self, cutstring, name=False):
		self.varleft = None
		self.varright = None
		self.operator = None
		self.weightstring = cutstring
		# set name
		if name != False:
			self.name = name
		else:
			self.name = filter(str.isalnum, cutstring.replace(">", "Gt").replace("<", "St"))

		# test if simple, parseable cutstring
		operators = [s for s in supported_operators if s in cutstring]
		self.operator = operators[0]
		tmpcutstring = cutstring.split(self.operator)
		if len(tmpcutstring) ==2 and len(operators)==1:
			self.varleft = tmpcutstring[0]
			self.varright = float(tmpcutstring[1])
		self.update_weightstring()
		logger.debug("Created %s object with name \"%s\" and string \"%s\"", self, self.get_name(), self.extract())
				

	def invert(self):
		self.operator = inverted_operators[supported_operators.index(self.operator)]
		self.update_weightstring()
		logger.debug("Inverted Cut object %s with name \"%s\", value is now \"%s\"", self, self.get_name(), self.extract())
		return self

	def update_weightstring(self):
		if (self.varleft != None) and (self.operator!=None) and (self.varright!=None):
			self.weightstring = "".join([self.varleft, self.operator, str(self.varright)])
		return self

	def set_value(self, value):
		self.varright = float(value)
		self.update_weightstring()
		return self

	def get_value(self):
		return self.varright

	def get_variable(self):
		return self.varleft

	def set_variable(self, variable):
		self.varleft = variable
		self.update_weightstring()
		return self

	def get_name(self):
		return self.name

	def extract(self):
			return self.embrace(self.weightstring)

	def embrace(self, c):
		return "(" + c + ")"
	
# holder class for cutstring objects
class Cuts(object):
	def __init__(self, *args):
		self.cutstrings = []
		if args!=False:
			for w in args:
				self.add(w, False)
		self.log()

	def log(self):
		logger.debug("Cuts object %s now holds the cuts %s", self, self.get_names())

	def __add__(self, other):
		new_cuts = Cuts()
		for c in self.cutstrings:
			new_cuts.add(c, False)
		for c in other.cutstrings:
			new_cuts.add(c, False)
		self.log()
		return new_cuts

	def add(self, cutstring, verbose=True):
		if (issubclass(type(cutstring), Cut)) or isinstance(cutstring, Cut):
			if cutstring.get_name() in self.get_names():
				logger.fatal("Not possible to add the cutstring %s since its name is not unique. Aborting.", cutstring)
				raise LookupError
			else:
				self.cutstrings.append(cutstring)
		if verbose:
			self.log()

	def extract(self):
		return self.cutstrings

	def expand(self):
		if len(self.cutstrings) > 0:
			return "*".join([c.extract() for c in self.cutstrings])
		else:
			return "(1.0)"

	def get_names(self):
		return [w.get_name() for w in self.cutstrings]

	def get(self, name):
		for w in self.cutstrings:
			if w.get_name() == name:
				return w
		logger.fatal("The name \"%s\" is not part of this Cuts object", name)
		logger.fatal("Avilable names are: %s", [w.get_name() for w in self.cutstrings])
		raise KeyError

	def remove(self, name):
		if name in self.get_names():
			self.cutstrings = [w for w in self.cutstrings if not w.get_name() == name]
			self.log()
		return self
