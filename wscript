# -*- python -*-

APPNAME= 'bip2'
VERSION= '0.0.1'

top = '.'
out = 'bin'

def options(opt):
  opt.load('compiler_cxx')
  opt.load('compiler_cxx unittest_gtest')

def configure(conf):
  conf.load('compiler_cxx')
  conf.load('compiler_cxx unittest_gtest')
  conf.env.append_value('CXXFLAGS', ['-Wall', '-Wextra', '-g', '-O3', '-std=c++0x'])
  conf.check_cxx(lib = ['gflags'], uselib_store = 'common')

def build(bld):
  bld.recurse('src')
