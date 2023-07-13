text_viewer1_sources = [
  'main.c',
  'text_viewer1-window.c',
  'text_viewer1-application.c',
]

text_viewer1_deps = [
  dependency('libadwaita-1', version: '>= 1.0'),
]

gnome = import('gnome')

text_viewer1_sources += gnome.compile_resources('text_viewer1-resources',
  'text_viewer1.gresource.xml',
  c_name: 'text_viewer1'
)

executable('text-viewer1', text_viewer1_sources,
  dependencies: text_viewer1_deps,
  install: true,
)
