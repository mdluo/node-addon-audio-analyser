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
    if (!Number.isSafeInteger(this.fftSize)) {
      throw new TypeError('FFT size must be a valid number.')
    }
    if (this.fftSize < minFftSize || this._fftSize > maxFftSize) {
      throw new RangeError(`FFT size must be between ${minFftSize} and ${maxFftSize}.`)
    }
    if (!isPowerOfTwo(this.fftSize)) {
      throw new RangeError(`The value provided(${this.fftSize}) is not a power of two.`)
    }

    this.maxDecibels = options.maxDecibels || defaultMaxDecibels
    this.minDecibels = options.minDecibels || defaultMinDecibels
    this.smoothingTimeConstant =
      options.smoothingTimeConstant || defaultSmoothingTimeConstant

    const byteLength = this.fftSize * SIZE_OF_FLOAT_32
    this.inputBuffer = Buffer.alloc(byteLength)
    this.magnitudeBuffer = Buffer.alloc(byteLength / 2)
    this.wirteIndex = 0

    this.analyser = new bindings.Analyser(
      this.fftSize,
      this.maxDecibels,
      this.minDecibels,
      this.smoothingTimeConstant,
      this.inputBuffer,
      this.magnitudeBuffer
    )
  }

  _transform(chunk, encoding, cb) {
    let readIndex = 0
    let writeIndex = this.wirteIndex
    const { fftSize, inputBuffer } = this
    const { length } = chunk

    for (let byteToCopy = fftSize * SIZE_OF_FLOAT_32; readIndex < length; readIndex += byteToCopy) {
      if (readIndex + fftSize >= length) {
        byteToCopy = length - readIndex
      }
      chunk.copy(inputBuffer, readIndex, writeIndex, writeIndex + byteToCopy)
      writeIndex += byteToCopy
      if (writeIndex >= fftSize) {
        writeIndex = 0
      }
    }
    this.wirteIndex = writeIndex

    cb(null, chunk)
  }

  getFloatFrequencyData(array) {
    if (this.wirteIndex) {
      return
    }
    if (!array || !(array instanceof Float32Array) || !array.length) {
      throw new TypeError('The parameter is not a valid Float32Array.')
    }
    if (array.length < this.fftSize) {
      throw new TypeError('The array must have enought space.')
    }
    return this.analyser.GetFloatFrequencyData(Buffer.from(array.buffer))
  }

  getByteFrequencyData(array) {
    if (this.wirteIndex) {
      return
    }
    if (!array || !(array instanceof Uint8Array) || !array.length) {
      throw new TypeError('The parameter is not a valid Uint8Array.')
    }
    if (array.length < this.fftSize) {
      throw new TypeError('The array must have enought space.')
    }
    return this.analyser.GetByteFrequencyData(Buffer.from(array.buffer))
  }
}

module.exports = Analyser
