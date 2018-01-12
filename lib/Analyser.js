const { Transform } = require('stream')

const bindings = require('./bindings')
const { isPowerOfTwo } = require('./utils')

const SIZE_OF_FLOAT_32 = 4

const defaultFftSize = 2048
const defaultMaxDecibels = -30
const defaultMinDecibels = -100
const defaultSmoothingTimeConstant = 0.8
const defaultStepSample = 500
const defaultFloat = false
const minFftSize = 32
const maxFftSize = 32768

class Analyser extends Transform {
  constructor(options = {}) {
    super(options)
    this.fftSize = options.hasOwnProperty('fftSize') ? options.fftSize : defaultFftSize
    if (!Number.isSafeInteger(this.fftSize)) {
      throw new TypeError('FFT size must be a valid number.')
    }
    if (this.fftSize < minFftSize || this._fftSize > maxFftSize) {
      throw new RangeError(`FFT size must be between ${minFftSize} and ${maxFftSize}.`)
    }
    if (!isPowerOfTwo(this.fftSize)) {
      throw new RangeError(`The value provided(${this.fftSize}) is not a power of two.`)
    }

    this.maxDecibels = options.hasOwnProperty('maxDecibels') ?
      options.maxDecibels : defaultMaxDecibels
    this.minDecibels = options.hasOwnProperty('minDecibels') ?
      options.minDecibels : defaultMinDecibels
    this.smoothingTimeConstant = options.hasOwnProperty('smoothingTimeConstant') ?
      options.smoothingTimeConstant : defaultSmoothingTimeConstant
    this.stepSample = options.hasOwnProperty('stepSample') ?
      options.stepSample : defaultStepSample
    this.float = options.hasOwnProperty('float') ?
      options.float : defaultFloat

    const bufferLength = this.fftSize * SIZE_OF_FLOAT_32
    this.inputBuffer = Buffer.alloc(bufferLength)
    this.bufferLength = bufferLength
    this.wirteIndex = 0

    this.inputQueue = Buffer.from(new Uint8Array())
    this.stepByte = this.stepSample * SIZE_OF_FLOAT_32

    this.magnitudeBuffer = Buffer.alloc(bufferLength / 2)

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
    const {
      inputBuffer, bufferLength,
      inputQueue, stepByte,
      float, outputArray,
    } = this
    this.inputQueue = Buffer.concat(
      [inputQueue, chunk],
      inputQueue.length + chunk.length
    )
    while (this.inputQueue.length >= bufferLength) {
      this.inputQueue.copy(inputBuffer, 0, 0, bufferLength)
      if (float) {
        this.getFloatFrequencyData(outputArray)
      } else {
        this.getByteFrequencyData(outputArray)
      }
      this.push(Buffer.from(outputArray))
      this.inputQueue = this.inputQueue.slice(stepByte)
    }
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
