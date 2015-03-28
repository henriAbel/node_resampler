var Resampler = require('node_resampler');

// Downsample from 44100 to 22050
var resampler = new Resampler({
	sourceRate: 44100,
	targetRate: 22050,
	stereo : true
});

// Read from stdin -> resample -> print to stdout
process.stdin.pipe(resampler).pipe(process.stdout);
