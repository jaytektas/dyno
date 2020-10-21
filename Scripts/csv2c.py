#!/usr/bin/python3

import argparse
import re
import csv
import os

parser = argparse.ArgumentParser(description='csv2c - precompile csv converter')
parser.add_argument('csvFileName', metavar='<csv file>', type=str, nargs=1,  help='csv input file')
parser.add_argument('-v, --verbose', dest='v', action='store_true', help='debugging output')
parser.add_argument('-i', dest='initFileName', metavar='<init file>', type=str, nargs=1, help='create init structure')
parser.add_argument('-t', dest='typedefFileName', metavar='<typedef file>', type=str, nargs=1, help='create typedef structure')
args = parser.parse_args()

if not (args.initFileName or args.typedefFileName):
    print ("at least one option of type -i -t is required to determine the action to be take")
    exit()


# optional file handles
typedefFile = None
initFile= None

# required file handle
csvFile = open(args.csvFileName[0], 'r', encoding="utf8", errors='ignore')
csvReader = csv.DictReader(csvFile)

if args.typedefFileName:
    typedefFile= open(args.typedefFileName[0], 'w')

if args.initFileName:
    initFile = open(args.initFileName[0], 'w')

class Node(object):
    p = object
    e = object

    def __init__(self, name, properties):
        self.name = name
        self.properties = properties
        self.children = None
        self.sibblings = None

    def __iter__(self):
        self.p = self
        return self

    def __str__(self):
        return self.name

    def __next__(self):
        self.e = self.p
        if self.e is None:
            raise StopIteration
        self.p = self.p.sibblings
        return self.e

    def next(self): return self.__next__(self)

    def add(self, path, properties):
        if args.v: print ('%s Node.add(%s)' % (self.name, path))
        pathSplit = path.split('.')
        if args.v: print ('path split: %s' % (pathSplit))
        if len(pathSplit) == 1:
            if self.children is None:
                if args.v: print ('%s adding as my first child %s' % (self.name, path))
                self.children = Node(path, properties)
            else:
                if args.v: print ('%s adding as my last child %s' % (self.name, path))
                p = self.children
                found = False
                while True:
                    if p.name == path:
                        found = True
                        break
                    if p.sibblings is None:
                        break
                    p = p.sibblings
                if found == False: p.sibblings = Node(path, properties)

        else:
            if args.v: print('%s checking my children for path %s' % (self.name, pathSplit[0]))
            if self.children is None:
                if args.v: print ("%s has no children to search for path %s, adding path" % (self.name, pathSplit[0]))
                self.children = Node(pathSplit[0], {'type':'structure'})
                self.children.add('.'.join(pathSplit[1::]), properties)
            else:
                p = self.children
                found = False
                while not found:
                    if p.name == pathSplit[0]:
                        found = True
                        continue
                    if p.sibblings is None: break;
                    p = p.sibblings

                if found is True:
                    if args.v: print ('FOUND! %s telling it to add' % (p.name))
                    p.add('.'.join(pathSplit[1::]), properties)
                else:
                    if args.v: print ('path NOT FOUND %s adding' % (pathSplit[0]))
                    p.sibblings = Node(pathSplit[0], {'type':'structure'})
                    p.sibblings.add('.'.join(pathSplit[1::]), properties)

typedefName = (os.path.basename(args.csvFileName[0]).split('.', 1)[0])
typedef = Node('root', {'type':'root'})

for row in csvReader:
    if row['type'] == '': continue

    if args.v: print ('found %s' % (row['fqn']))
    typedef.add(row['fqn'], row)
    if args.v: print (row)


if args.v: print (uniquePageVars)

def traverse(node, t):

    for n in node:
        if n.children:
            if t == 0:
                if args.typedefFileName: typedefFile.write (('\t' * t + 'typedef struct %s\n' + '\t' * t + '{\n') % (n.name))
                if args.initFileName: initFile.write('\t' * t + '{\n')
                traverse(n.children, t + 1)
                if args.typedefFileName: typedefFile.write ('\t' * t + '} %s_t;\n' % (n.name))
                if args.initFileName: initFile.write('\t' * t + '};')

            else:
                if args.typedefFileName: typedefFile.write (('\t' * t + 'struct \n' + '\t' * t + '{\n'))
                if args.initFileName: initFile.write('\t' * t + '{\n')
                traverse(n.children, t + 1)
                if args.typedefFileName: typedefFile.write ('\t' * t + '} %s;\n' % (n.name))
                if args.initFileName:
                    if n.sibblings is None: initFile.write('\t' * t + '}\n')
                    else: initFile.write('\t' * t + '},\n')
        else:
            if args.typedefFileName: typedefFile.write ('\t' * t +'%s %s;\n' % (n.properties['type'], n.name))

            if args.initFileName:
                if n.sibblings:
                    initFile.write('\t' * t + n.properties['value'] + ',\t\t // %s\n' % (n.properties['description']))
                else:
                    initFile.write('\t' * t + n.properties['value'] + '\t\t // %s\n' % (n.properties['description']))

if not typedefFile is None or not initFile is None: traverse (typedef.children, 0)

if not typedefFile is None: typedefFile.close()
if not initFile is None: initFile.close()



