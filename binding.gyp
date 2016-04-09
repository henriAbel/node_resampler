{
	"targets": [
		{
			"target_name": "node_resampler",
			"sources": [ "node_resampler.cc"],
			"cflags": ['-D__STDC_CONSTANT_MACROS'],
			"libraries": [
				'-lswresample',
				'-lavutil'
			],
			'include_dirs' : [
				'<!(node -e "require(\'nan\')")'
			]
		}
	]
}
