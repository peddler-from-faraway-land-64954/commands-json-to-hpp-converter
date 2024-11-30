project('commands json to hpp converter', 'cpp',
  version : '1.0.0',
  license : 'MIT')
add_global_arguments('-DBOOST_LOG_DYN_LINK', language : 'cpp')
dep = [ dependency('boost', modules : ['log', 'program_options', 'log_setup']) ]

inc = include_directories('include')

subdir('include')
subdir('src')
