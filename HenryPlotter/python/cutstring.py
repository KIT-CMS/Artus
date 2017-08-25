# -*- coding: utf-8 -*-
import sys
"""
"""

# arten:
	# larger/ smaller
	# (save) equivalent
	# AND
	# OR
	# 
supported_operators = ['<', '>', '&&', '||', '==', '!=']
inverted_operators =  ['>', '<', '||', '&&', '!=', '==']

class Weightstring(object):

	def __init__(self, weightstring, name=False):
		# test weightstring: only constant values and floating point numbers are allowed
		self.weightstring = weightstring
		try:
			float(weightstring)
			self.is_float = True
			self.name = name
		except:
			self.is_float = False
			self.name = weightstring if name==False else name
		if name == False:
			print "No appropriate name has been assigned to weight with value " + str(weightstring) + ". Please use an explicit name for this weight string. \n Aborting."
			sys.exit(1)
			
	def invert(self):
		if self.is_float:
			self.weightstring = str(1.0/float(self.weightstring))
		else:
			if self.weightstring.startswith("1.0/"):
				self.weightstring = self.weightstring.replace("1.0/", "")
			else:
				self.weightstring = "1.0/"+self.weightstring

	def get_name(self):
		return self.name

	def extract(self):
			return self.embrace(self.weightstring)

	def embrace(self, c):
		return "(" + c + ")"


class Weightstrings(object):
	def __init__(self, *args):
		self.weightstrings = []
		if args!=False:
			for w in args:
				self.add(w)

	def add(self, weightstring):
		if (issubclass(type(weightstring), Weightstring)):
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

class Cutstring(Weightstring):

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
