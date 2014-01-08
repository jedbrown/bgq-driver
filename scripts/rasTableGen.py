#! /usr/bin/python
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
#                                                                  
# Blue Gene/Q                                                      
#                                                                  
# (C) Copyright IBM Corp.  2010, 2011                              
#                                                                  
# US Government Users Restricted Rights -                          
# Use, duplication or disclosure restricted                        
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
# This software is available to you under the                      
# Eclipse Public License (EPL).                                    
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               

import os

class RasTableGen:

	'''The RasTableGen is responsible for generating the 
	file that will be used to populate the event codes
	database table.   The output file has an event per
	row with each item in a comma sperated list.'''
	 
	def __init__(self, path):
		self.filename = path + '/event_code_table.txt'
		self.id_list = {}
		self.errorStart = '*********** RAS Metatdata Issue Start ******** '
		self.errorEnd   = '*********** RAS Metatdata Issue End ********** '
		print 'Ras event code table file name: ', self.filename
		
	def gen(self, events):
		self.writeFile(events)
		return
	
	def writeFile(self, events):
		f = open(self.filename, 'w')
		# for each id dictionary write the info to the file.
		id_keys = events.keys()
		id_keys.sort()
		for i_key in id_keys:
			id = i_key;
			details = events[i_key]
			category = details['category']
			comp = details['component']
			sev = details['severity']
			msg = details['message']
			desc = details['description']
			act = details['service_action']
			decoder = details['decoder']
			ctl = details['control_action']
			thrs_cnt = details['threshold_count']
			thrs_per = details['threshold_period']
			diags = details['relevant_diags']
			# id = details['id']
			if (self.len_ok(id, category, comp, sev,  msg, desc, act, decoder, ctl, thrs_cnt, thrs_per, diags) and self.id_ok(id, details)):
				f.write('\"' + id + '\",')
				f.write('\"' + category + '\",')
				f.write('\"' + comp + '\",')
				f.write('\"' + sev + '\",')
				f.write('\"' + msg + '\",')
				if desc == '':
					f.write(',')
				else:
					f.write('\"' + desc + '\",')
				if act == '':
					f.write(',')
				else:
					f.write('\"' + act + '\",')
				if decoder == '':
					f.write(',')
				else: 
					f.write('\"' + decoder + '\",')
				if ctl == '':
					f.write(',')
				else:
					f.write('\"' + ctl + '\",')
				if thrs_cnt == '':
					f.write(',')
				else:
					f.write(thrs_cnt + ',')
				if thrs_per == '':
					f.write(',')
				else:
					f.write('\"' + thrs_per + '\",')
				if diags == "":
					f.write('\n')
				else:
					f.write('\"' + diags + '\"\n')
		f.close()
		return

	def id_ok(self, id, details):
		try:
			orig = self.id_list[id]
			print self.errorStart
			print 'Rejecting duplicate id: ', id
			print 'Def (1) category=', orig['category'], ' comp=', orig['component']
			print '   src file=', orig['file_name'], ' line=', orig['line_num']
			print 'Def (2) category=', details['category'], ' comp=', details['component']
			print '   src file=', details['file_name'], ' line=', details['line_num']
			print self.errorEnd
			return False			      
		except KeyError, e:
			self.id_list[id] = details
			return True
			
	def len_ok(self, id, category, comp, sev,  msg, desc, act, decoder, ctl, thrs_cnt, thrs_per, diags):
		ok = True
		max_id_len = 8;
		if (len(id) > max_id_len):
			print self.errorStart
			print 'Id ' + id + ' is too long; ' + str(len(id)) + ' > ' + str(max_id_len)
			print self.errorEnd
			ok = False
		max_category_len = 16
		if (len(category) > max_category_len):
			print self.errorStart
			print 'Category ' + category + ' is too long; ' + str(len(category)) + ' > ' + str(max_category_len)
			print self.errorEnd
			ok = False
		max_comp_len = 16;
		if (len(comp) > max_comp_len):
			print self.errorStart
			print 'Component ' + comp + ' is too long; ' + str(len(comp)) + ' > ' + str(max_comp_len)
			print self.errorEnd
			ok = False
		if (len(sev) > 8):
			print self.errorStart
			print 'Severity ' + sev + ' is too long; ' + str(len(sev)) + ' > 8'
			print self.errorEnd
			ok = False
		if (len(msg) > 1024):
			print self.errorStart
			print 'Message ' + msg + ' is too long; ' + str(len(msg)) + ' > 1024'
			print self.errorEnd
			ok = False
		if (len(desc) > 1024):
			print self.errorStart
			print 'Description ' + desc + ' is too long; ' + str(len(desc)) + ' > 1024'
			print self.errorEnd
			ok = False
		if (len(act) > 1024):
			print self.errorStart
			print 'Service Action ' + act + ' is too long; ' + str(len(act)) + ' > 1024'
			print self.errorEnd
			ok = False
		max_decoder_len = 64
		if (len(decoder) > max_decoder_len):
			print self.errorStart
			print 'Decoder ' + decoder + ' is too long; ' + str(len(decoder)) + ' > ' + str(max_decoder_len)
			print self.errorEnd
			ok = False
		max_ctl_len = 256
		if (len(ctl) > max_ctl_len):
			print self.errorStart
			print 'Control Action ' + ctl + ' is too long; ' + str(len(ctl)) + ' > ' + str(max_ctl_len)
			print self.errorEnd
			ok = False
		max_thrs_cnt_len = 16
		if (len(thrs_cnt) > max_thrs_cnt_len):
			print self.errorStart
			print 'Threshold Count ' + thrs_cnt + ' is too long; ' + str(len(thrs_cnt)) + ' > ' + str(max_thrs_cnt_len)
			print self.errorEnd
			ok = False
		max_thrs_per_len = 16
		if (len(thrs_per) > max_thrs_per_len):
			print self.errorStart
			print 'Threshold Period ' + thrs_per + ' is too long; ' + str(len(thrs_per)) + ' > ' + str(max_thrs_per_len)
			print self.errorEnd
			ok = False
		max_diags_len = 256
		if (len(diags) > max_diags_len):
			print self.errorStart
			print 'Relevant Diagnostics ' + diags + ' is too long; ' + str(len(diags)) + ' > ' + str(max_diags_len)
			print self.errorEnd
			ok = False
		return ok

import getopt, sys

# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------
def usage():
    print sys.argv[0], '[-h] root'
    print '   where -h for help'

# ----------------------------------------------------------------------------
# main
# ----------------------------------------------------------------------------
def test():
    '''Self test of RasTableGen'''
    try:
        optlist, args = getopt.getopt(sys.argv[1:], "h", ["help"])

    except getopt.GetoptError:
        usage()
        sys.exit(1)
    
    for o, a in optlist:    
        if o in ("-h", "--help"):
            usage()
            sys.exit(0)
    
    events = {}
    tg = RasTableGen()
    tg.gen(events)

if __name__=='__main__':
	test()

