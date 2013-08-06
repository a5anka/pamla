
def main():
  in_file_name = 'data/linear_regression_key_file_100MB.txt_12.txt'
  out_file_name = in_file_name.split('/')[1].split('.')[0]+in_file_name.split('/')[1].split('.')[1]+'.arff'
  #file_name = './data/out.dat'
  program = 'linear_regressi'
  events_list = ['0x149','0x151','0x2a2','0x126','0x227','0x224','0x8a2','0x1b0','0x20f0','0x2f1','0x1f2','0x1b8','0x2b8','0x4b8','0x40cb']
  arff_header = ['@relation function_level_badfs_badma_good\n',\
                 '@attribute r0149 numeric\n','@attribute r0151 numeric\n','@attribute r02a2 numeric\n','@attribute r0126 numeric\n',\
                 '@attribute r0227 numeric\n','@attribute r0224 numeric\n','@attribute r08a2 numeric\n','@attribute r01b0 numeric\n',\
                 '@attribute r20f0 numeric\n','@attribute r02f1 numeric\n','@attribute r01f2 numeric\n','@attribute r01b8 numeric\n',\
                 '@attribute r02b8 numeric\n','@attribute r04b8 numeric\n','@attribute r40cb numeric\n','@attribute status {good, badfs, badma}\n',\
                 '@data\n']
  
  perf_file = open(in_file_name,'rb')
  out_file = open(out_file_name,'wb')
  #write the headers to the arff file
  out_file.writelines(arff_header)
  #dictionary to store {function:{raw_event:interpolated_val,...}}
  event_count_list = {}
  
  for line in perf_file:
    if 'Samples' in line:
      raw_event = line.split()[6].strip('\'')
      event_count = int(perf_file.next().split()[4])
      
    if '#' not in line and len(line.split())!=0:
      temp = {}
      if line.split()[1] == program:
        size = len(line.split())
        symbol = ''
        overhead = float(line.split()[0].strip('%'))
        shared_obj = line.split()[2]

        if size<=5:
          symbol = line.split()[4]
        else:
          i=4
          while(i<size):
            symbol += line.split()[i]
            i+=1
            
        if overhead != 0: interpolated_val = round((event_count/overhead)*100,2)
        else: interpolated_val = '?'
        #print symbol
        temp[raw_event] = interpolated_val
      if len(temp) !=0:
        if symbol in event_count_list: event_count_list[symbol][raw_event]=interpolated_val
        else: event_count_list[symbol] = temp
  #print event_count_list
  i=1
  for k in event_count_list.keys():
    for k2 in events_list:
      if k2 not in event_count_list[k].keys():
        event_count_list[k][k2] = '?'
    
    #write to the arff file only if instruction count [0xc0] is available for the function
    if '0xc0' in event_count_list[k].keys() and event_count_list[k]['0xc0'] != '?':
      print '>>>>'+str(event_count_list[k]['0xc0'])
      
    #write to the output arff file
      out = ''
      out_file.write('%\n'+'% '+str(i)+' Function: '+k+'\n%\n')
      i += 1
      print 'Function: '+k
      for k2 in events_list:
        if event_count_list[k][k2] == '?':
          out += str((event_count_list[k][k2])) + ','
        else:
          out += str(event_count_list[k][k2]/event_count_list[k]['0xc0']*pow(10, 9)) + ','
      out += '?\n'
      print out
      out_file.write(out)  
  print 'done'


if __name__=='__main__':
  main()