const { Transform } = require('stream')

const bindings = require('./bindings')
const { isPowerOfTwo } = require('./utils')

const SIZE_OF_FLOAT_32 = 4

const defaultFftSize = 2048
const defaultMaxDecibels = -30
const defaultMinDecibels = -100
const defaultSmoothingTimeConstant = 0.8
const defaultFloat = false
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
    this.float = options.float || defaultFloat

    const byteLength = this.fftSize * SIZE_OF_FLOAT_32
    this.inputBuffer = Buffer.alloc(byteLength)
    this.magnitudeBuffer = Buffer.alloc(byteLength / 2)
    this.wirteIndex = 0

    if (this.float) {
      this.outputArray = new Float32Array(this.fftSize / 2)
    } else {
      this.outputArray = new Uint8Array(this.fftSize / 2)
    }

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
    const {
      fftSize, inputBuffer,
      float, outputArray,
    } = this
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
      if (float) {
        this.getFloatFrequencyData(outputArray)
      } else {
        this.getByteFrequencyData(outputArray)
      }
      this.push(Buffer.from(outputArray))
    }
    this.wirteIndex = writeIndex

    cb()
  }

  getFloatFrequencyData(array) {
    return this.analyser.GetFloatFrequencyData(Buffer.from(array.buffer))
  }

  getByteFrequencyData(array) {
    return this.analyser.GetByteFrequencyData(Buffer.from(array.buffer))
  }
}

module.exports = Analyser
