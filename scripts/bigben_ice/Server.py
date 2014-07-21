#!/usr/bin/env python
'''
Created on 2013-12-11
@author zyy_max
@brief test Ice Server to print arguments passed to  PageMarkServer and ClassificationServer
'''

import sys, os
ice_path = '/home/max.zhang/external/ice4py/python'
sys.path.append(ice_path)

import Ice, traceback
Ice.loadSlice('PageMark.ice')
Ice.loadSlice('textClassification.ice')
import BigBen

class PageMarkServerI(BigBen.PageMarkServerInterface):
    def doMark(self, page, current=None):
        for key, value in page.items():
            print key, '==>', value
class TextClassificationServerI(BigBen.TextClassificationServerInterface):
    def doPredict(self, page, current=None):
        for key, value in page.items():
            print key, '==>', value

class Server(Ice.Application):
    def run(self, args):
        if len(args) > 3:
            print(self.appName() + ": too many arguments")
            return 1
        if args[1] == 'PageMarkServer':
            print 'PageMarkServer activated...'
            adapter = self.communicator().createObjectAdapter('PageMarkServer')
            adapter.add(PageMarkServerI(), self.communicator().stringToIdentity("PageMarkServer"))
        elif args[1] == 'TextClassificationServer':
            print 'TextClassificationServer activated...'
            adapter = self.communicator().createObjectAdapter('TextClassificationServerInterface')
            adapter.add(TextClassificationServerI(), self.communicator().stringToIdentity("TextClassificationServerInterface"))
        else:
            print 'Error argument',args[1]
            return -1
        adapter.activate()
        self.communicator().waitForShutdown()
        return 1

if __name__=="__main__":
    if len(sys.argv) < 3:
        print 'Usage:\t', sys.argv[0], '{PageMarkServer | TextClassificationServer} config.server'
        exit(-1)
    app = Server()
    sys.exit(app.main(sys.argv, sys.argv[-1]))

