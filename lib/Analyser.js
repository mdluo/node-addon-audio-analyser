const { Transform } = require('stream')

const bindings = require('./bindings')
const { isPowerOfTwo } = require('./utils')

const SIZE_OF_FLOAT_32 = 4

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

    this._inputBuffer = Buffer.alloc(this.fftSize * SIZE_OF_FLOAT_32)
    this._wirteIndex = 0

    this._analyser = new bindings.Analyser()
    this._analyser.fftSize = this.fftSize
    this._analyser.maxDecibels = this.maxDecibels
    this._analyser.minDecibels = this.minDecibels
    this._analyser.smoothingTimeConstant = this.smoothingTimeConstant
    this._analyser.inputBuffer = this._inputBuffer
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

  _transform(chunk, encoding, cb) {
    let readIndex = 0
    let writeIndex = this._wirteIndex
    const { fftSize, _inputBuffer } = this
    const { length } = chunk

    for (let byteToCopy = fftSize * SIZE_OF_FLOAT_32; readIndex < length; readIndex += byteToCopy) {
      if (readIndex + fftSize >= length) {
        byteToCopy = length - readIndex
      }
      chunk.copy(_inputBuffer, readIndex, writeIndex, writeIndex + byteToCopy)
      writeIndex += byteToCopy
      if (writeIndex >= fftSize) {
        writeIndex = 0
      }
    }
    this._wirteIndex = writeIndex

    cb(null, chunk)
  }

  getFloatFrequencyData(array) {
    if (!array || !(array instanceof Float32Array) || !array.length) {
      throw new TypeError('The parameter is not a valid Float32Array.')
    }
    if (array.length < this.fftSize) {
      throw new TypeError('The array must have enought space.')
    }
    return this._analyser.GetFloatFrequencyData(Buffer.from(array.buffer))
  }
}

module.exports = Analyser
