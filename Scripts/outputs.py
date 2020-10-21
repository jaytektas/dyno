#!/usr/bin/python3
# -*- coding: utf-8 -*-

import argparse
import re
import csv

parser = argparse.ArgumentParser(description='csv2c - precompile csv converter')
parser.add_argument('csvFileName', metavar='<csv file>', type=str, nargs=1,  help='csv input file')
parser.add_argument('offsetFileName', metavar='<offset file>', type=str, nargs=1,  help='offset input file')
parser.add_argument('iniFileName', metavar='<ini file>', type=str, nargs=1,  help='ini output file')

parser.add_argument('-v, --verbose', dest='v', action='store_true', help='debugging output')
args = parser.parse_args()

# required file handles
csvFile = open(args.csvFileName[0], 'r')
csvReader = csv.DictReader(csvFile)

offsetFile = open(args.offsetFileName[0], 'r')
offsetReader = csv.DictReader(offsetFile)
offsets = {}

iniFile = open(args.iniFileName[0], 'w')
ini = []

# offsets in to dictionary with fqn as key
for o in offsetReader:
    offsets[o['fqn']] = o

if args.v: print (offsets)

def type(t):
    return {'char':'S08', \
            'signed char':'S08', \
            'unsigned char':'U08', \
            'short':'S16', \
            'signed short':'S16', \
            'unsigned short':'U16', \
            'long':'S32', \
            'unsigned long':'U32', \
            'uint8_t':'U08', \
            'uint16_t':'U16', \
            'uint32_t':'U32', \
            'int8_t':'S08', \
            'int16_t':'S16', \
            'int32_t':'S32', \
            'float':'F32', \
            'double':'S32', \
            '1':'U08', \
            '2':'U16', \
            '4':'U32'
            }[t]

iniFile.write('ochBlockSize = %s\n' % (offsets['status']['size']))

for row in csvReader:
    if row['type'] == '': continue
    if re.search('^#', row['type']):
        iniFile.write('%s\n' % (row['type']))
        continue

    qn = re.sub('[[].*[]]|:\d.*', '', row['fqn']);
    if row['class'] == 'bits':
        size = offsets[qn]['size']
        iniFile.write('%s = %s, %s, %d, [%d:%d]\n' % ( \
            re.sub('[[].*[]]|:\d.*', '', '_'.join(row['fqn'].split('.'))), \
            row['class'], \
            type(size), \
            int(offsets[qn]['offset']), \
            int(offsets[qn]['bitpos']), \
            int(offsets[qn]['bitpos']) + int(offsets[qn]['bits'])-1))
    else:
        iniFile.write('%s = %s, %s, %d, %s, %s, %s\n' % ( \
            re.sub('[[].*[]]', '', '_'.join(row['fqn'].split('.'))), \
            row['class'], \
            type(row['type']), \
            int(offsets[qn]['offset']), \
            row['units'], \
            row['scale'], \
            row['translate']))

csvFile.close()
offsetFile.close()
iniFile.close()
