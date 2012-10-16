{
  'targets': [
    {
      'target_name': 'vu8_addon_1',
      'include_dirs': [
        './deps/vu8/src',
        './src'
      ],
      'sources': [
        'src/vu8_addon_1.cpp'
      ],
      'cflags': [
        '-fexceptions'
      ],
      'cflags_cc': [
        '-fexceptions'
      ],
      'link_settings': {
        'libraries': [
          '-lboost_date_time',
        ],
      }
    }
  ]
}
