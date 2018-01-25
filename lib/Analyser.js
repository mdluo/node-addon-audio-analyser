const { Transform } = require('stream')

const bindings = require('./bindings')
const { isPowerOfTwo } = require('./utils')

const SIZE_OF_FLOAT_32 = 4

const defaultFftSize = 2048
const defaultHopSize = 294 // 150 hops per second when SampleRate = 44.1
const defaultBufferSize = 1 // how many windows of frequency data to buffer before push to downstream
const defaultMaxDecibels = -30
const defaultMinDecibels = -100
const defaultSmoothingTimeConstant = 0.8
const defaultMagnitude = false
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

    this.hopSize = options.hasOwnProperty('hopSize') ?
      options.hopSize : defaultHopSize
    this.bufferSize = options.hasOwnProperty('bufferSize') ?
      options.bufferSize : defaultBufferSize

    this.maxDecibels = options.hasOwnProperty('maxDecibels') ?
      options.maxDecibels : defaultMaxDecibels
    this.minDecibels = options.hasOwnProperty('minDecibels') ?
      options.minDecibels : defaultMinDecibels
    this.smoothingTimeConstant = options.hasOwnProperty('smoothingTimeConstant') ?
      options.smoothingTimeConstant : defaultSmoothingTimeConstant

    this.magnitude = options.hasOwnProperty('magnitude') ?
      options.magnitude : defaultMagnitude
    this.float = options.hasOwnProperty('float') ?
      options.float : defaultFloat

    this.inputBufferLength = this.fftSize * SIZE_OF_FLOAT_32
    this.inputBuffer = Buffer.alloc(this.inputBufferLength)

    this.inputQueue = Buffer.from(new Uint8Array())
    this.hopLength = this.hopSize * SIZE_OF_FLOAT_32

    this.magnitudeBuffer = Buffer.alloc(this.inputBufferLength / 2)

    this.outputArrayLength = (this.fftSize / 2) * ((this.magnitude || this.float) ? SIZE_OF_FLOAT_32 : 1)
    this.outputBufferLength = this.outputArrayLength * this.bufferSize
    this.outputBuffer = Buffer.alloc(this.outputBufferLength)
    this.outputCounter = 0

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
      inputBuffer, inputBufferLength,
      inputQueue, hopLength,
      outputBuffer, outputArrayLength,
    } = this
    this.inputQueue = Buffer.concat(
      [inputQueue, chunk],
      inputQueue.length + chunk.length
    )
    while (this.inputQueue.length >= inputBufferLength) {
      this.inputQueue.copy(inputBuffer, 0, 0, inputBufferLength)
      this.getFrequencyData(outputBuffer.slice(
        this.outputCounter * outputArrayLength,
        (this.outputCounter + 1) * outputArrayLength
      ))
      this.outputCounter += 1
      if (this.outputCounter === this.bufferSize) {
        this.push(outputBuffer)
        this.outputCounter = 0
      }
      this.inputQueue = this.inputQueue.slice(hopLength)
    }
    cb()
  }

  getFrequencyData(buffer) {
    if (this.magnitude) {
      this.analyser.GetFloatMagnitudeData()
      return this.magnitudeBuffer.copy(buffer)
    }
    if (this.float) {
      return this.getFloatFrequencyData(buffer)
    } else {
      return this.getByteFrequencyData(buffer)
    }
  }

  getFloatFrequencyData(buffer) {
    return this.analyser.GetFloatFrequencyData(buffer)
  }

  getByteFrequencyData(buffer) {
    return this.analyser.GetByteFrequencyData(buffer)
  }
}

module.exports = Analyser
