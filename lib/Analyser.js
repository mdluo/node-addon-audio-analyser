const { Transform } = require('stream')

const { RealtimeAnalyser } = require('./bindings')
const { isPowerOfTwo } = require('./utils')

const defaultFftSize = 2048
const defaultMaxDecibels = -30
const defaultMinDecibels = -100
const defaultSmoothingTimeConstant = 0.8
const minFftSize = 32
const maxFftSize = 32768

class Analyser extends Transform {
  constructor(options = {}) {
    super(options)
    this.fftSize = options.fftSize || defaultFftSize
    this.maxDecibels = options.maxDecibels || defaultMaxDecibels
    this.minDecibels = options.minDecibels || defaultMinDecibels
    this.smoothingTimeConstant = options.smoothingTimeConstant ||
      defaultSmoothingTimeConstant

    this._analyser = new RealtimeAnalyser()
    this._analyser.fftSize = this.fftSize
    this._analyser.maxDecibels = this.maxDecibels
    this._analyser.minDecibels = this.minDecibels
    this._analyser.smoothingTimeConstant = this.smoothingTimeConstant
    console.log(this._analyser)
  }

  get fftSize() {
    return this._fftSize
  }
  set fftSize(value) {
    if (!Number.isSafeInteger(value)) {
      throw new TypeError('FFT size must be a valid number.')
    }
    if (value < minFftSize || value > maxFftSize) {
      throw new RangeError(`FFT size must be between ${minFftSize} and ${maxFftSize}.`)
    }
    if (!isPowerOfTwo(value)) {
      throw new RangeError(`The value provided(${value}) is not a power of two.`)
    }
    this._fftSize = value
  }

  get maxDecibels() {
    return this._maxDecibels
  }
  set maxDecibels(value) {
    this._maxDecibels = value
  }

  get minDecibels() {
    return this._minDecibels
  }
  set minDecibels(value) {
    this._minDecibels = value
  }

  get smoothingTimeConstant() {
    return this._smoothingTimeConstant
  }
  set smoothingTimeConstant(value) {
    this._smoothingTimeConstant = value
  }

  _transform(chunk, encoding, callback) {
    this._analyser.WriteInput(chunk, chunk.lenght)
    /*
    const length = chunk.length / 4
    const f32a = new Float32Array(length)
    for (let i = 0; i < length; i += 1) {
      f32a[i] = chunk.readFloatLE(i * 4)
    }
    */
    this.push(chunk)
    callback()
  }
}

module.exports = Analyser
