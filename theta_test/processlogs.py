#!/usr/bin/env python
import argparse,logging,glob
import numpy as np
logger = logging.getLogger(__name__)


def main():
   ''' simple starter program that can be copied for use when starting a new script. '''
   logging_format = '%(asctime)s %(levelname)s:%(name)s:%(message)s'
   logging_datefmt = '%Y-%m-%d %H:%M:%S'
   logging.basicConfig(level=logging.INFO,format=logging_format,datefmt=logging_datefmt)

   parser = argparse.ArgumentParser(description='')
   parser.add_argument('-i','--input',dest='input',type=str,help='input',required=True,nargs='*')
   parser.add_argument('--debug', dest='debug', default=False, action='store_true', help="Set Logger to DEBUG")
   parser.add_argument('--error', dest='error', default=False, action='store_true', help="Set Logger to ERROR")
   parser.add_argument('--warning', dest='warning', default=False, action='store_true', help="Set Logger to ERROR")
   parser.add_argument('--logfilename',dest='logfilename',default=None,help='if set, logging information will go to file')
   args = parser.parse_args()

   if args.debug and not args.error and not args.warning:
      # remove existing root handlers and reconfigure with DEBUG
      for h in logging.root.handlers:
         logging.root.removeHandler(h)
      logging.basicConfig(level=logging.DEBUG,
                          format=logging_format,
                          datefmt=logging_datefmt,
                          filename=args.logfilename)
      logger.setLevel(logging.DEBUG)
   elif not args.debug and args.error and not args.warning:
      # remove existing root handlers and reconfigure with ERROR
      for h in logging.root.handlers:
         logging.root.removeHandler(h)
      logging.basicConfig(level=logging.ERROR,
                          format=logging_format,
                          datefmt=logging_datefmt,
                          filename=args.logfilename)
      logger.setLevel(logging.ERROR)
   elif not args.debug and not args.error and args.warning:
      # remove existing root handlers and reconfigure with WARNING
      for h in logging.root.handlers:
         logging.root.removeHandler(h)
      logging.basicConfig(level=logging.WARNING,
                          format=logging_format,
                          datefmt=logging_datefmt,
                          filename=args.logfilename)
      logger.setLevel(logging.WARNING)
   else:
      # set to default of INFO
      for h in logging.root.handlers:
         logging.root.removeHandler(h)
      logging.basicConfig(level=logging.INFO,
                          format=logging_format,
                          datefmt=logging_datefmt,
                          filename=args.logfilename)

   log_files = []
   for g in args.input:
      log_files += glob.glob(g)
   log_files = sorted(log_files)

   output_data = {}
   config_start_str = ' running with '

   for log_file in log_files:
      logger.info('processing %s',log_file)
      lines = open(log_file).readlines()

      parts = log_file.split('.')

      config = {}

      config['jobid'] = parts[0]
      config['descr'] = parts[1]
      config['ranks_per_node'] = int(parts[1][:parts[1].find('r_')])
      config['nodes'] = int(parts[1][parts[1].find('r_')+2:parts[1].find('n_')])
      config['collectors'] = int(parts[1][parts[1].find('n_')+2:parts[1].find('c')])

      logger.info('processing file: %s; with %s rpn, %s nodes, %s collectors',log_file,config['ranks_per_node'],config['nodes'],config['collectors'])

      logfile_data = {}
      logfile_data['config'] = config

      runtimedata = {}
      logfile_data['runtimedata'] = runtimedata
      waittimes = []
      runtimedata_names = []

      
      for line in lines:
         try:
            if line.startswith('CCT '):
               
               if len(runtimedata_names) > 0 and 'run time' not in line:
                  parts = line.split()
                  
                  for i in range(len(runtimedata_names)):
                     if '.' in parts[i+1] or 'E' in parts[i+1] or 'e' in parts[i+1]:
                        runtimedata[runtimedata_names[i]].append(float(parts[i+1]))
                     else:
                        runtimedata[runtimedata_names[i]].append(int(parts[i+1]))
                  
               elif len(runtimedata_names) == 0:
                  parts = line.replace('CCT ','').split('\t')
                  #logger.info('names: %s',parts)
                  for i in range(len(parts)):
                     name = parts[i].strip().replace(' ','_')
                     runtimedata_names.append(name)
                     runtimedata[name] = []
                  #logger.info('names: %s',runtimedata_names)

            elif line.startswith(config_start_str):
               name_start = len(config_start_str)
               name_end = line.find(':',name_start)
               config_name = line[name_start:name_end].strip().replace(' ','_')
               value = int(line[name_end+1:])
               config[config_name] = value

            elif 'wait time:' in line:
               start = line.find(':') + 1
               waittimes.append(float(line[start:]))
         except Exception as e:
            logger.exception('received exception for line: %s',line)
            raise
      #assert(len(waittimes) == len(runtimedata['run_time']))

      logfile_data['worker_wait_time'] = get_mean_sigma(waittimes)

      runtimedata['worker_wait_time'] = []
      for i in range(len(waittimes)):
         runtimedata['worker_wait_time'].append(waittimes[i])

      logfile_data['messages_per_second'] = get_mean_sigma(runtimedata['messages_per_second'])
      logfile_data['merge_time'] = get_mean_sigma(runtimedata['merge_time'])
      logfile_data['probe_time'] = get_mean_sigma(runtimedata['probe_time'])

      output_data[logfile_data['config']['descr']] = logfile_data


      logger.info('worker_wait_time:    %s',logfile_data['worker_wait_time'])
      logger.info('messages_per_second: %s',logfile_data['messages_per_second'])
      logger.info('merge_time:          %s',logfile_data['merge_time'])
      logger.info('probe_time:          %s',logfile_data['probe_time'])


   print('nodes\tranks_per_node\tcollectors\tranks\tmessages_per_second\t\tworker_wait_time\t\tmerge_time')
   for descr in output_data:
      print('%10i' % output_data[descr]['config']['nodes'] + '\t'
            '%10i' % output_data[descr]['config']['ranks_per_node'] + '\t'
            '%10i' % output_data[descr]['config']['collectors'] + '\t'
            '%10i' % (output_data[descr]['config']['ranks_per_node']*output_data[descr]['config']['nodes']) + '\t'
            '%10.4f' % output_data[descr]['messages_per_second']['mean'] + '\t'
            '%10.4f' % output_data[descr]['messages_per_second']['sigma'] + '\t'
            '%10.4f' % output_data[descr]['worker_wait_time']['mean'] + '\t'
            '%10.4f' % output_data[descr]['worker_wait_time']['sigma'] + '\t'
            '%10.4f' % output_data[descr]['merge_time']['mean'] + '\t'
            '%10.4f' % output_data[descr]['merge_time']['sigma']
            )


def get_mean_sigma(data):
   sum = 0.
   sum2 = 0.
   for i in range(len(data)):
      sum += data[i]
      sum2 += data[i]*data[i]

   n = float(len(data))
   output = {}
   mean = sum / n
   output['mean'] = mean
   output['sigma'] = np.sqrt((1./n)*sum2 - mean*mean)
   return output
   
   


if __name__ == "__main__":
   main()
