{
	"targets": [
		{
			"target_name": "node_resampler",
			"sources": [ "node_resampler.cc"],
			"cflags": ['-D__STDC_CONSTANT_MACROS'],
			'conditions': [
				[ 'OS=="win"', {
					"libraries": [
						'-l<(ffmpeg_root)/lib/swresample.lib',
						'-l<(ffmpeg_root)/lib/avutil.lib',
					],
					'include_dirs' : [
						'<!(node -e "require(\'nan\')")',
						'<(ffmpeg_root)/include'
					]
				}],
				 ['OS=="linux"', {
				 "libraries": [
						'-lswresample',
						'-lavutil'
					],
					'include_dirs' : [
						'<!(node -e "require(\'nan\')")'
					]
		        }],
			]

		}
	]
}
