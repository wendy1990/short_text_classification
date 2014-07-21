#!/usr/bin/env python
'''
Created on 2013-12-11
@author zyy_max
@brief test Ice Client to call PageMarkServer and ClassificationServer
'''

import sys, os
ice_path = '/home/max.zhang/external/ice4py/python'
sys.path.append(ice_path)

import Ice, traceback
Ice.loadSlice('PageMark.ice')
Ice.loadSlice('textClassification.ice')
import BigBen

class Client(Ice.Application):
    def run(self, args):
        if len(args) > 4:
            print(self.appName() + ": too many arguments")
            return 1
        if args[1] == 'PageMarkServer':
            print 'Using PageMarkServer ...'
            proxy = BigBen.PageMarkServerInterfacePrx.checkedCast(self.communicator().propertyToProxy('PageMarkServerInterface.Proxy')) 
        elif args[1] == 'TextClassificationServer':
            print 'Using TextClassificationServer ...'
            proxy = BigBen.TextClassificationServerInterfacePrx.checkedCast(self.communicator().propertyToProxy('TextClassificationServerInterface.Proxy')) 
        else:
            print 'Error argument',args[1]
            return -1
        # Reading data from file {key:value--++++--key:value--++++--...--++++--key:value}
        with open(args[-1]) as ins:
            page = {}
            for lineid, line in enumerate(ins):
                for pair in line.strip().split('--++++--'):
                    key, _, value = pair.partition(':')
                    page[key] = value.strip('"').strip("'").strip()
                print 'Dealing with page:',lineid,line
                try:
                    if args[1] == 'TextClassificationServer':
                        proxy.doPredict(page)
                    elif args[1] == 'PageMarkServer':
                        proxy.doMark(page)
                except Ice.Exception as ex:
                    print ex
        return 1

if __name__=="__main__":
    if len(sys.argv) < 4:
        print 'Usage:\t', sys.argv[0], '{PageMarkServer | TextClassificationServer} config.client input.txt'
        exit(-1)
    app = Client()
    sys.exit(app.main(sys.argv, sys.argv[2]))
