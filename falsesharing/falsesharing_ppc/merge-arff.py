#!/usr/bin/env python
#
#	merge-arff.py	- merges two ARFF files to produce a new merged ARFF file
#
# use : "python -m py_compile merge-arff.py" to check syntax
#
import sys, re
# import subprocess
#
data_string='bad_mem_access_and_false_sharing'
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
if (len(sys.argv) != 4):
	sys.exit("Usage: prog infile1 infile2 outfile")
infname1 = sys.argv[1]
infname2 = sys.argv[2]
outfname = sys.argv[3]
inf_1 = open(infname1, "r")
inf_2 = open(infname2, "r")
# merge infile1 and infile2 onto outfile
# 	1st insert preamble, then combined info from 2 inputs, then data of infile1, last dat from infile2
line_num_1 = line_num_2 = 0
#print 'F1: position=', inf_1.tell() , 'F2: position=' , inf_2.tell()
for line_1 in inf_1:
	line_num_1 += 1
	if re.search("^@attribute status", line_1):
		break
if not line_1:
	sys.exit('Error: File_1: unexpected end of file')
for line_2 in inf_2:
	line_num_2 += 1
	if re.search("^@attribute status", line_2):
		break
if not line_2:
	sys.exit('Error: File_2: unexpected end of file')
#print 'F1: position=', inf_1.tell() , 'F2: position=' , inf_2.tell()
if line_1 != line_2:
	sys.exit('Error: status lines mismatch: File_1 line %d: %s != File_2 line %d: %s' \
		%(line_num_1,line_1, line_num_2,line_2))
training_flag_1 = ratio_mode_1 = training_flag_2 = ratio_mode_2 = ratio_mode = 0
num_instances_1 = num_good_1 = num_badfs_1 = num_badma_1 = num_unknowns_1= 0
num_instances_2 = num_good_2 = num_badfs_2 = num_badma_2 = num_unknowns_2= 0
datatype=''
for line_1 in inf_1:
	line_num_1 += 1
	if re.search("^% training data", line_1):
		#print line_1
		training_flag_1=1
		datatype = "training"
		if not re.search("non-ratio mode", line_1):
			ratio_mode_1= ratio_mode = 1
		break
if not line_1:
	sys.exit('Error: File_1: unexpected end of file')
for line_2 in inf_2:
	if not line_2:
		sys.exit('Error: File_2: unexpected end of file')
	line_num_2 += 1
	if re.search("^% training data", line_2):
		#print line_2
		training_flag_2=1
		if not re.search("non-ratio mode", line_1):
			ratio_mode_2=1
		break
if not line_2:
	sys.exit('Error: File_2: unexpected end of file')
if training_flag_1 != training_flag_2 or ratio_mode_1 != ratio_mode_2:
	sys.exit('Error: data types mismatch: File_1 line %d: %s != File_2 line %d: %s' \
		%(line_num_1,line_1, line_num_2,line_2))
#print 'F1: position=', inf_1.tell() , 'F2: position=' , inf_2.tell()
for line_1 in inf_1:
	line_num_1 += 1
	if re.search("^% instances=", line_1):
		line_1 = line_1.rstrip('\n')
		[inst,good,badfs,badma]=line_1.split(':')
		[t1,num_instances_1] = inst.split('=')
		[t1,num_good_1] = good.split('=')
		[t1,num_badfs_1] = badfs.split('=')
		[t1,num_badma_1] = badma.split('=')
		num_instances_1 = int(num_instances_1)
		num_good_1 = int(num_good_1)
		num_badfs_1 = int(num_badfs_1)
		num_badma_1 = int(num_badma_1)
		#print num_instances_1, num_good_1, num_badfs_1, num_badma_1
		assert num_instances_1 == num_good_1 + num_badfs_1 + num_badma_1, 'File_1: Line %d' %(line_num_1)
		break
if not line_1:
	sys.exit('Error: File_1: unexpected end of file')
for line_2 in inf_2:
	line_num_2 += 1
	if re.search("^% instances=", line_2):
		line_2 = line_2.rstrip('\n')
		[inst,good,badfs,badma]=line_2.split(':')
		[t1,num_instances_2] = inst.split('=')
		[t1,num_good_2] = good.split('=')
		[t1,num_badfs_2] = badfs.split('=')
		[t1,num_badma_2] = badma.split('=')
		num_instances_2 = int(num_instances_2)
		num_good_2 = int(num_good_2)
		num_badfs_2 = int(num_badfs_2)
		num_badma_2 = int(num_badma_2)
		#print num_instances_2, num_good_2, num_badfs_2, num_badma_2
		assert num_instances_2 == num_good_2 + num_badfs_2 + num_badma_2, 'File_2: Line %d' %(line_num_2)
		break
if not line_2:
	sys.exit('Error: File_2: unexpected end of file')
num_instances = num_instances_1 + num_instances_2
num_good = num_good_1 + num_good_2
num_badfs = num_badfs_1 + num_badfs_2
num_badma = num_badma_1 + num_badma_2
#print 'F1: position=', inf_1.tell() , 'F2: position=' , inf_2.tell()
#print 'F1: Line ' + str(line_num_1)+': <' + line_1 + '>'
#print 'F2: Line ' + str(line_num_2)+': <' + line_2 + '>'
for line_1 in inf_1:
	line_num_1 += 1
	if re.search("unknown_\(\?\)_entries=", line_1):
		line_1 = line_1.rstrip('\n')
		[unknowns,dummy]=line_1.split(':')
		[t1,num_unknowns_1] = unknowns.split('=')
		num_unknowns_1 = int(num_unknowns_1)
		break
if not line_1:
	sys.exit('Error: File_1: unexpected end of file')
assert line_1 
for line_2 in inf_2:
	line_num_2 += 1
	if re.search("unknown_\(\?\)_entries=", line_2):
		line_2 = line_2.rstrip('\n')
		[unknowns,dummy]=line_2.split(':')
		[t1,num_unknowns_2] = unknowns.split('=')
		num_unknowns_2 = int(num_unknowns_2)
		break
if not line_2:
	sys.exit('Error: File_2: unexpected end of file')
num_unknowns = num_unknowns_1 + num_unknowns_2
#print 'F1: position=', inf_1.tell() , 'F2: position=' , inf_2.tell()
#print 'F1: Line ' + str(line_num_1)+': <' + line_1 + '>'
#print 'F2: Line ' + str(line_num_2)+': <' + line_2 + '>'
#
outf = open(outfname, "w")
# write the preamble
write_preamble(outf,infname1 + '_and_' + infname2,attrib,skip_attrib)	
# then combined info from 2 inputs
if (datatype == "training"):
	outf.write('% training data')
	if (ratio_mode == 1) :
		outf.write(': ratio mode\n')
	else:
		outf.write(': non-ratio mode\n')
	outf.write('% instances=' + str(num_instances)+ ' : good=' + str(num_good) + ' : badfs=' + str(num_badfs) + \
			' : badma=' + str(num_badma) + '\n')
	outf.write('% \tunknown_(?)_entries=' + str(num_unknowns) + ' : (out of total=' + str(num_instances*len(attrib))+ ')')
	outf.write('='+ format(((float(num_unknowns)/(len(attrib)*num_instances))*100), '.2f')+'%\n%\n')
	
else:
	outf.write('% testing data')
	if (ratio_mode == 1) :
		outf.write(': ratio mode\n')
	else:
		outf.write(': non-ratio mode\n')
	outf.write('% instances=' + str(num_instances)+ '\n')
	outf.write('% \tunknown_(?)_entries=' + str(num_unknowns) + ' : (out of total=' + str(num_instances*len(attrib))+ ')')
	outf.write('='+ format(((float(num_unknowns)/(len(attrib)*num_instances))*100), '.2f')+'%\n%\n')		
# finally write the instances
# first data of infile1, last data from infile2
for line_1 in inf_1:		# read the remainder of infile 1 line by line and write to outfile
	outf.write(line_1)	
	line_num_1 += 1
for line_2 in inf_2:		# read the remainder of infile 2 line by line and write to outfile
	outf.write(line_2)	
	line_num_2 += 1
#print 'F1: Line ' + str(line_num_1)+': <' + line_1 + '>'
#print 'F2: Line ' + str(line_num_2)+': <' + line_2 + '>'
inf_1.close()
inf_2.close()
outf.close()
if (datatype == "training"):
	print '% produced', num_instances , 'instances (', num_good , 'good,' , num_badfs , 'badfs,' , \
		num_badma , 'badma)'
else:
	print '% produced\t', num_instances , 'instances'
print '%\tmerging\t', num_instances_1, 'instances in file_1 (', num_good_1, 'good,' , num_badfs_1 , 'badfs,' , \
		num_badma_1 , 'badma)'
print '%\tand\t', num_instances_2, 'instances in file_2 (', num_good_2, 'good,' , num_badfs_2 , 'badfs,' , \
		num_badma_2 , 'badma)'
print '%\tunknown_(?)_entries=' + str(num_unknowns) + ' : (out of total=' + str(num_instances*len(attrib))+ ')' + \
	'='+ format(((float(num_unknowns)/(len(attrib)*num_instances))*100), '.2f') +'%'
#print "%----------------------------------------------------------"







