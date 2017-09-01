# -*- coding: utf-8 -*-
import sys
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
			print "No appropriate name has been assigned to weight with value " + str(weightstring) + ". Please use an explicit name for this weight string. \n Aborting."
			sys.exit(1)
			

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
			print "No appropriate name has been assigned to weight with value " + str(weightstring) + ". Please use an explicit name for this weight string. \n Aborting."
			sys.exit(1)

	def invert(self):
		if is_float:
			self.weightstring = str(1.0/float(self.weightstring))
		else:
			if self.weightstring.startswith("1.0/"):
				self.weightstring = self.weightstring.replace("1.0/", "")
			else:
				self.weightstring = "1.0/"+self.weightstring

# holder class for weight/cutstring objects
class Weights(object):
	def __init__(self, *args):
		self.weightstrings = []
		if args!=False:
			for w in args:
				self.add(w)

	def add(self, weightstring):
		if (issubclass(type(weightstring), Weight)):
			if weightstring.get_name() in self.get_names():
				print "Not possible to add the weightstring " + str(weightstring) + " since its name is not unique. Aborting."
				sys.exit(1)
			else:
	
				self.weightstrings.append(weightstring)

	def extract(self):
		full_weightstring = "*".join([c.extract() for c in self.weightstrings])
		return full_weightstring

	def get_names(self):
		return [w.get_name() for w in self.weightstrings]

	def get(self, name):
		for w in self.weightstrings:
			if w.get_name() == name:
				return w
		return False

	def remove(self, name):
		if name in get_names:
			self.weightstrings = [w for w in self.weightstrings if not w.get_name() == name]
			return True
		else:
			return False

# Class for a simple cut expression e.g. 'pt_1>22'
class Cut():

	def __init__(self, cutstring, name=False):
		self.varleft = False
		self.varright = False
		self.operator = False
		# set name
		if name != False:
			self.name = name
		else:
			self.name = filter(str.isalnum, cutstring)

		# test if simple, parseable cutstring
		operators = [s for s in cutstring if s in supported_operators]	
		self.operator = operators[0]
		tmpcutstring = cutstring.split(self.operator)
		if len(tmpcutstring) ==2:
			self.varleft = tmpcutstring[0]
			self.varright = float(tmpcutstring[1])
		self.update_weightstring()

			

	def invert(self):
		self.operator = inverted_operators[supported_operators.index(self.operator)]
		self.update_weightstring()

	def update_weightstring(self):
		self.weightstring = "".join([self.varleft, self.operator, str(self.varright)])

	def set_value(self, value):
		self.varright = float(value)
		self.update_weightstring()

	def get_value(self):
		return self.varright

	def get_variable(self):
		return self.varleft

	def set_variable(self, variable):
		self.varleft = variable
		self.update_weightstring()

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
				self.add(w)

	def add(self, cutstring):
		if (issubclass(type(cutstring), Weight)):
			if cutstring.get_name() in self.get_names():
				print "Not possible to add the cutstring " + str(cutstring) + " since its name is not unique. Aborting."
				sys.exit(1)
			else:
	
				self.cutstrings.append(cutstring)

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
		return False

	def remove(self, name):
		if name in get_names:
			self.cutstrings = [w for w in self.cutstrings if not w.get_name() == name]
			return True
		else:
			return False
