#!/usr/bin/env python

from itertools import groupby
from operator import itemgetter
import sys
from optparse import OptionParser
try:
    from cStringIO import StringIO
except ImportError:
    from StringIO import StringIO

class HStream:
    """
    simple wrapper class to facilitate writing hadoop streaming jobs
    in python. inherit the class and define mapper, combiner and reducer functions.
    """
    default_delim = ''
    default_istream = sys.stdin
    default_ostream = sys.stdout
    default_estream = sys.stderr

    def __init__(self,
                 delim = default_delim,
                 istream = default_istream,
                 ostream = default_ostream):
        self.delim = delim
        self.istream = istream
        self.ostream = ostream

        self.parse_args()

    def read_input(self, step_type):
        for line in self.istream:
            if step_type == 'm':
                yield (None, line.strip())
            elif step_type == 'c' or step_type == 'r':
                key, value = line.strip().split(self.delim, 1)
                yield key, value

    def write_output(self, key, value):
        print >> self.ostream, self.delim.join((key, value))

    def map(self):
        self.mapper_init()

        for (key, value) in self.read_input('m'):
            self.mapper(key, value)

        self.mapper_end()

    def combine(self):
        self.combiner_init()

        data = self.read_input('c')
        for key, kv_pairs in groupby(data, itemgetter(0)):
            values = (v for k, v in kv_pairs)
            self.combiner(key, values)

        self.combiner_end()

    def reduce(self):
        self.reducer_init()

        data = self.read_input('r')
        for key, kv_pairs in groupby(data, itemgetter(0)):
            values = (v for k, v in kv_pairs)
            self.reducer(key, values)

        self.reducer_end()

    def mapper(self, key, value):
        raise NotImplementedError

    def combiner(self, key, values):
        raise NotImplementedError

    def reducer(self, key, values):
        raise NotImplementedError

    def mapper_init(self):
        return

    def mapper_end(self):
        return

    def combiner_init(self):
        return

    def combiner_end(self):
        return

    def reducer_init(self):
        return

    def reducer_end(self):
        return

    def parse_args(self):
        parser = OptionParser()
        parser.add_option("-m", "--map",
                          help="run mapper",
                          action="store_true",
                          dest="run_map",
                          default="False")
        parser.add_option("-c", "--combine",
                          help="run combiner",
                          action="store_true",
                          dest="run_combine",
                          default="False")
        parser.add_option("-r", "--reduce",
                          help="run reduce",
                          action="store_true",
                          dest="run_reduce",
                          default="False")

        opts, args = parser.parse_args()

        self.args = dict([s.split('=', 1) for s in args])

        if opts.run_map is True:
            self.map()
        elif opts.run_combine is True:
            self.combine()
        else:
            self.reduce()

if __name__ == '__main__':
    pass
