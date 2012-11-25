#!/usr/bin/env python2
#
#	toarff.py	- converts  data files to ARFF
#
# use : "python -m py_compile toarff.py" to check syntax
#
# import os
import sys, re
# import subprocess
#
data_string='bad_mem_access_and_false_sharing'	# this will be the name of 'relation' of ARFF file
class_string='{good, badfs, badma}'
attrib=["r00c0","r0149","r0151","r02a2","r0126","r0227","r0224","r08a2","r01b0","r20f0","r02f1","r01f2","r01b8","r02b8","r04b8","r40cb"]
skip_attrib=['r00c0']
divisor_key='r00c0'
#-----------------------------------------------------
def write_preamble(out, infile_name, attrib_list, skip_list):
	"This writes the ARFF preamble"
	out.write('% ARFF file for ' + infile_name +'\n%\n')
	out.write('@relation '+ data_string +'\n%\n')
	out.write('% ' + str(len(attrib_list)) + ' original attributes=' + str(attrib_list) + '\n')
	out.write('% skipped attributes=' + str(skip_list) + '\n%\n')
	out.write('% the selected ' + str(len(attrib_list) - len(skip_list)) + ' attributes + the class attribute:\n%\n')
	#for index in range(len(attrib_list)):
	#	out.write('@attribute ' + attrib_list[index] + ' numeric\n')
	for attribute in attrib_list:
		if (attribute not in skip_list):
			out.write('@attribute ' + attribute + ' numeric\n')
	out.write('@attribute status '+ class_string +'\n')
	out.write('%\n@data\n%\n')
	return	
#-----------------------------------------------------
if (len(sys.argv) != 4 and len(sys.argv) != 3):
	sys.exit("Usage: prog infile [outfile] -r|-nr")
infname = sys.argv[1]
ratio_mode=0
if (len(sys.argv) == 4):
	outfname = sys.argv[2]
	if (sys.argv[3] == '-r'):
		ratio_mode=1
	else:
		assert (sys.argv[3] == '-nr')
		#sys.exit("Usage: prog infile [outfile] -r|-nr")
elif (len(sys.argv) == 3):
	outfname = infname + '.arff'
	if (sys.argv[2] == '-r'):
		ratio_mode=1
		outfname = infname + '.ratio.arff'
	else:
		assert (sys.argv[2] == '-nr')
inf = open(infname, "r")
outf = open(outfname, "w")
attrib_count = 0
line_num = 0
good_count = badfs_count = badma_count = mode_count = prev_mode_count = 0
comment_count = ind_count = outd_count = nocount = 0
noratio_count = noratio_good_count = noratio_badma_count = noratio_badfs_count = 0
mode=''
line = inf.readline()
while line :
	line_num += 1
	if re.match("^#", line):	# skip lines beginning with '#'
		if attrib_count == 0 :	# then we are OK
			outf.write('% '+line)
			#print ('Line ' +str(line_num)+':'+line),
			if re.search("\\bGood\\b", line, re.IGNORECASE):
				mode="good"
				good_count += 1
				mode_count += 1
			elif re.search("\\bMatMult.*Block\\b", line, re.IGNORECASE):	# blocking for matmult is good
				# skip the lines as: '# 	With BLOCKING for N=400, Block size=100-------------'
				mode="good"
				good_count += 1
				mode_count += 1
			elif re.search("\\bBad-MA\\b", line, re.IGNORECASE):
				mode="badma"
				badma_count += 1
				mode_count += 1
			elif re.search("\\bBad-FS\\b", line, re.IGNORECASE):
				mode="badfs"
				badfs_count += 1
				mode_count += 1
			else:
				mode="?"
				comment_count += 1
			line = inf.readline()
			continue
		else:
			sys.exit('Error in input file: Line %d' %(line_num))
	data = { }						# start with empty dictionary map (hash)
	line = line.rstrip('\n')
	while line :					# data line
		#print "Line: ", line_num, "line:<", line, ">"
		if not re.search("<not counted>", line):  
			if line.count(':') == 1:
				[value, key] = re.split(":", line)		# we have 2 fields, "value:key"
				# or can say: [value, key] = line.split(':') 
			elif line.count(':') == 2 :
				[value, key, stddev] = re.split(":", line)	# or 3 fields "value:key:stddev"
			else :
				sys.exit('Error in input file: Line %d' %(line_num))
			#value = int(value)
		else:						# we have 2 fields, "<not counted>:key"
			#print "Found <not counted>, Line=", line_num
			if line.count(':') == 1:
				[value, key] = re.split(":", line) 	# or can say: [value, key] = line.split(':')
			else :
				sys.exit('Error in input file: Line %d' %(line_num))			
			value = '?'
			nocount += 1
			if (ratio_mode == 1 and key == divisor_key):
				#assert data[divisor_key] != '?', 'Line %d' %(line_num)
				print 'Warning! **** divisor_key=%s=\'?\': Line %d' %(divisor_key, line_num)
				noratio_count += 1
				if (mode == 'good'):
					noratio_good_count += 1
				elif mode == 'badma':
					noratio_badma_count += 1
				elif mode == 'badfs':
					noratio_badfs_count += 1				
		#print "Line: ", line_num, "key:", key, "\tvalue=", value, "\stddev=", stddev
		data[key] = value
		attrib_count += 1
		if attrib_count == len(attrib) :
			break;
		line = inf.readline()
		line = line.rstrip('\n')
		line_num += 1
	assert len(data) == len(attrib), 'Line %d' %(line_num)
	ind_count = ind_count + 1
	attrib_count = 0
	for k in attrib:
		assert k in data
	#start writing to the output file
	if (ratio_mode == 1 and data[divisor_key]=='?'):
		outf.write('% Skipped; ratio cannot be computed as divisor_key=\'?\'\n')
	else:
		outf.write('% ' + str(ind_count) + '\n')
		for k in attrib:
			if (k not in skip_attrib):
				if (ratio_mode == 0):
					outf.write(str(data[k])+', ')
					#print str(data[k]),', '
				else:
					if (data[k] == '?'):
						strvalue='?'
					else:
						strvalue= str(float(data[k])/float(data[divisor_key]))
					outf.write(strvalue+', ')
					#print strvalue,', '	
		outf.write(mode+'\n')
		#print mode
	if (mode_count > 0) :
		if (prev_mode_count > 0) :
			assert mode_count == prev_mode_count, 'Line %d: %d != %d' %(line_num, mode_count, prev_mode_count)
		prev_mode_count = mode_count
		mode_count = 0
	line = inf.readline()
	#print "num_good=", good_count/3, ", num_badfs=", badfs_count/3, ", num_badma=", badma_count/3, ", nocounts=", nocount
outd_count = ind_count - noratio_count
if (ratio_mode == 1 and noratio_count > 0):
	print '%', noratio_count, 'instances with divisor_key=\'?\' found; ',  (outd_count), 'were output'
assert line_num == (ind_count * len(attrib)+ ind_count * prev_mode_count + comment_count), 'Line %d' %(line_num)
if (good_count > 0 or badma_count > 0 or badfs_count > 0):
	datatype="training"
	good_count /= prev_mode_count
	badma_count /= prev_mode_count
	badfs_count /= prev_mode_count
	assert ind_count == good_count + badma_count + badfs_count
else:
	datatype="testing"
print '%Done: processed ', line_num, 'lines, ', ind_count, 'instances, ', comment_count, 'comment lines'
inf.close()
outf.close()
#
if (datatype == "training") and (ratio_mode == 1):
	#print noratio_count, noratio_good_count, noratio_badfs_count,  noratio_badma_count
	assert noratio_count == (noratio_good_count + noratio_badfs_count+ noratio_badma_count)
	good_count -= noratio_good_count
	badfs_count -= noratio_badfs_count
	badma_count -= noratio_badma_count
#	
with open(outfname, "r+") as outf:
	instances=outf.read()		# read existing content in the whole file into a string
	outf.seek(0)			# rewind to the beginning
	write_preamble(outf,infname,attrib,skip_attrib)	# write the preamble
	# append the following after the preamble but before the instances
	if (datatype == "training"):
		outf.write('% training data')
		if (ratio_mode == 1) :
			outf.write(': ratio mode\n')
		else:
			outf.write(': non-ratio mode\n')
		if (ratio_mode == 1) and (noratio_count > 0):
			outf.write('% '+ str(outd_count) + '\tinstances were actually produced, out of' + \
					str(ind_count) + '\n%\n')
		outf.write('% instances=' + str(outd_count)+ ' : good=' + str(good_count) + ' : badfs=' + str(badfs_count) + \
			' : badma=' + str(badma_count) + '\n')
		outf.write('% \tunknown_(?)_entries=' + str(nocount) + ' : (out of total=' + str(ind_count*len(attrib))+ ')')
		outf.write('='+ format(((float(nocount)/(len(attrib)*ind_count))*100), '.2f')+'%\n%\n')
	else:
		outf.write('% testing data')
		if (ratio_mode == 1) :
			outf.write(': ratio mode\n')
		else:
			outf.write(': non-ratio mode\n')
		if (ratio_mode == 1 and noratio_count > 0):
			outf.write('% '+ str(outd_count) + '\tinstances were actually produced, out of' + str(ind_count) + '\n%\n')
		else :
			outf.write('% instances=' + str(outd_count)+ '\n')
		outf.write('% \tunknown_(?)_entries=' + str(nocount) + ' : (out of total=' + str(ind_count*len(attrib))+ ')')
		outf.write('='+ format(((float(nocount)/(len(attrib)*ind_count))*100), '.2f')+'%\n%\n')		
	#finally write the instances
	outf.write(instances)
if (datatype == "training"):
	print '% produced ', outd_count , 'instances (', good_count , ' good, ' , badfs_count , ' badfs, ' , \
		badma_count , ' badma)'
else:
	print '% produced ', outd_count , 'instances'
print '%   ', nocount , ' unknown entries in input (out of ', ind_count*len(attrib), ')=',  format((float(nocount)/(ind_count*len(attrib)))*100.0, '.2f'),'%'
#print "%----------------------------------------------------------"







