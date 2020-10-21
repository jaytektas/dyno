import gdb

class Offsets(gdb.Command):
	outfile = None
	end = 0

	def __init__(self):
		super (Offsets, self).__init__ ('offsets-of', gdb.COMMAND_DATA)

	def invoke(self, arg, from_tty):
		argv = gdb.string_to_argv(arg)
		if len(argv) != 2:
			raise gdb.GdbError('offsets-of takes exactly 2 arguments.')
		self.outfile = open (argv[1], "w")
		self.outfile.write ('fqn,offset,size,bitpos,bits\n');
		root = gdb.lookup_type(argv[0] + '_t')
		self.outfile.write ('%s,%d,%d\n' % (argv[0], 0, root.sizeof))
		self.traverse(root.fields(), argv[0], 0)
		self.outfile.close()

	def traverse(self, branch, fqn, address):
		bfmode = 0
		bfaddress = 0
		bfsize = 0
	
		for field in branch:
			if field.type.code == gdb.TYPE_CODE_STRUCT:
				self.traverse(field.type.fields(), fqn + '.' + field.name, address + field.bitpos//8)
			else:
				if bfmode == 0:
					if field.bitsize:
						bfmode = 1
						bfaddress = address + field.bitpos//8
						bfsize = field.type.sizeof
						if bfaddress + bfsize > self.end:
							self.end = bfaddress + bfsize

				if bfmode:
					if field.bitsize:
						if (field.bitpos >= (bfaddress * 8 + bfsize * 8)):
							bfaddress = address + field.bitpos//8
							bfsize = field.type.sizeof
							if bfaddress + bfsize > self.end:
								self.end = bfaddress + bfsize
						self.outfile.write (('%s,%d,%d,%d,%d\n' % (fqn + '.' + field.name, bfaddress, field.type.sizeof, field.bitpos - ((bfaddress-address)*8), field.bitsize)))
					else:
						bfmode = 0

				if not bfmode:
					self.outfile.write (('%s,%d,%d,,\n' % (fqn + '.' + field.name, address + field.bitpos//8, field.type.sizeof)))
					if address + field.bitpos//8 + field.type.sizeof > self.end:
						self.end = address + field.bitpos//8 + field.type.sizeof

Offsets()




