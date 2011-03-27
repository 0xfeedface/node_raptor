import os
import Options

RAPTOR_PREFIX = '/usr/local'

def set_options(ctx):
    ctx.add_option('--clang', action='store_true', default='', dest='use_clang', help='Uses the clang compiler')
    ctx.add_option('--analyze', action='store_true', default='', dest='analyze', help='Runs the clang static analyzer (implies --clang)')
    ctx.tool_options('compiler_cxx')

def configure(ctx):
    if Options.options.analyze:
        ctx.env.ANALYZE = Options.options.analyze
        Options.options.use_clang = 1

    if Options.options.use_clang:
        os.environ['CXX'] = '/usr/bin/clang'

    ctx.check_tool('compiler_cxx')
    ctx.check_tool('node_addon')

def build(ctx):
    obj = ctx.new_task_gen('cxx', 'shlib', 'node_addon')

    # Analyze
    if ctx.env.ANALYZE:
        obj.cxxflags = ['-g', '-Wall', '--analyze']
    else:
        obj.cxxflags = ['-g', '-Wall']

    obj.find_sources_in_dirs('src')

    obj.target   = 'raptor'
    obj.includes = RAPTOR_PREFIX + '/include/raptor2'
    obj.lib      = 'raptor2'
    obj.libpath  = RAPTOR_PREFIX + '/lib'

    # obj.staticlibpath = RAPTOR_PREFIX + '/lib'
    # obj.staticlib     = 'raptor2'

