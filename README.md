# node_resampler
C++ libswresample bindings for audio resampling

Requirements
------------
libswresample
* Ubuntu/Debian via [Jon Severinsson's FFmpeg PPA](https://launchpad.net/~jon-severinsson/+archive/ubuntu/ffmpeg)
```sh
$ sudo apt-get install libswresample-dev
```
* Centos
```sh
$ sudo yum install ffmpeg-devel
```

Installation
------------
Install via git
```sh
$ npm install git+https://github.com/henriAbel/node_resampler.git
```

Example
------------
```javascript
var Resampler = require('node_resampler');

// Downsample from 44100 to 22050
var resampler = new Resampler({
        sourceRate: 44100,
        targetRate: 22050,
        stereo : true
});

// Read from stdin -> resample -> print to stdout
process.stdin.pipe(resampler).pipe(process.stdout);
```
