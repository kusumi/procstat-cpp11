src = [
  'buffer.cc',
  'container.cc',
  'main.cc',
  'panel.cc',
  'watcher.cc',
  'window.cc',
  ]

# https://mesonbuild.com/Threads.html
dep = [dependency('threads'), dependency('ncurses')]

if get_option('debug')
  add_global_arguments('-DDEBUG', language : 'cpp')
  src += 'log.cc'
endif

if get_option('stdout')
  add_global_arguments('-DCONFIG_STDOUT', language : 'cpp')
  src += 'screen_stdout.cc'
else
  src += 'screen_curses.cc'
endif

if get_option('stdthread')
  add_global_arguments('-DCONFIG_STDTHREAD', language : 'cpp')
  src += 'thread_std.cc'
else
  dep += meson.get_compiler('cpp').find_library('pthread', required : true)
  src += 'thread_posix.cc'
endif

if get_option('inotify') and build_machine.system() == 'linux'
  add_global_arguments('-DCONFIG_INOTIFY', language : 'cpp')
  src += 'watch_inotify.cc'
else
  src += 'watch_none.cc'
endif

if get_option('gettimeofday')
  add_global_arguments('-DCONFIG_GETTIMEOFDAY', language : 'cpp')
endif

executable('procstat-cpp11', src, dependencies : dep, install : true)
