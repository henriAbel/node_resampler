var node_resampler = require('./build/Release/node_resampler').Resampler;
var stream = require('stream')
var util = require('util');

var Resampler = function(opts) {
	stream.Transform.call(this, opts);
	this.bind = new node_resampler(opts.sourceRate, opts.targetRate, opts.stereo);
	this.buff = new Buffer(0);
	this.opts = opts;
}

util.inherits(Resampler, stream.Transform);

Resampler.prototype.configure = function(opts) {
	this.opts = opts;
	this.bind.configure(opts.sourceRate, opts.targetRate, opts.stereo);
}

Resampler.prototype._transform = function(chunk, encoding, done) {
	if (this.opts.sourceRate == this.opts.targetRate) {
		this.push(chunk);
		done();
	}
	else {
		var self = this;
		// Lets assume that audio has always 16 bit samples
		if (chunk.length % 16 !== 0) {
			var offset = Math.floor(chunk.length / 16) * 16;
			this.buff = Buffer.concat([this.buff, chunk.slice(0, offset)])
		}
		this.bind.resample(chunk.slice(0, offset), function(data) {
			self.push(data);
			done();
		});	
	}	
}

module.exports = Resampler;

