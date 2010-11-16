# srcdir  = 'src'
# blddir  = './build'
# VERSION = '0.0.1'
RAPTOR_PREFIX = '/usr/local'

def set_options(opt):
    opt.tool_options('compiler_cxx')

def configure(conf):
    conf.check_tool('compiler_cxx')
    conf.check_tool('node_addon')

def build(bld):    
    # obj.cxxflags = ['-g', '-D_FILE_OFFSET_BITS=64', '-D_LARGEFILE_SOURCE', '-Wall']
    obj          = bld.new_task_gen('cxx', 'shlib', 'node_addon')
    obj.target   = 'raptor'
    obj.source   = ['src/bindings.cc', 'src/parser.cc', 'src/statement.cc']
    obj.includes = [RAPTOR_PREFIX + '/include/raptor2']
    obj.lib      = ['raptor2']
    obj.libpath  = [RAPTOR_PREFIX + '/lib']
