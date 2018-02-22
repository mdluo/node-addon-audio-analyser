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

    // bytes of pcm data of one FFT input frame.
    this.fftFrameBytes = this.fftSize * SIZE_OF_FLOAT_32

    // bytes offset between two sequential FFT input frames.
    this.fftHopBytes = this.hopSize * SIZE_OF_FLOAT_32

    // number of cols (frequency bin) per FFT output frame.
    this.sgramColSize = this.fftSize / 2

    // bytes of each FFT output frame.
    this.sgramColBytes = this.sgramColSize * ((this.magnitude || this.float) ? SIZE_OF_FLOAT_32 : 1)

    // input PCM queue.
    this.pcmQueue = Buffer.alloc(0)

    // inputBuffer acts as both a node.js Buffer reference and a C++ float* pointer
    // first copy PCM data to this buffer, then C++ performs FFT on this buffer
    this.inputBuffer = Buffer.alloc(this.fftFrameBytes)

    // buffer to store raw C++ FFT output magnitude data.
    this.magnitudeBuffer = Buffer.alloc(this.fftFrameBytes / 2)

    // how many rows of sgram data are there in the buffer to push to downstream.
    this.sgramRowSize = 0

    // sgram buffer to push to downstream.
    // once sgramRowSize reaches bufferSize, push sgramBuffer to the downstream.
    // bufferSize: how many rows should be buffered before push to downstream.
    this.sgramBuffer = Buffer.alloc(this.sgramColBytes * this.bufferSize)

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
      pcmQueue, inputBuffer,
      fftFrameBytes, fftHopBytes,
      sgramBuffer, sgramColBytes, bufferSize,
    } = this
    this.pcmQueue = Buffer.concat([pcmQueue, chunk], pcmQueue.length + chunk.length)
    while (this.pcmQueue.length >= fftFrameBytes) {
      this.pcmQueue.copy(inputBuffer, 0, 0, fftFrameBytes)
      const writePointer = this.sgramRowSize * sgramColBytes
      this.getFrequencyData(sgramBuffer.slice(writePointer, writePointer + sgramColBytes))
      this.sgramRowSize += 1
      if (this.sgramRowSize === bufferSize) {
        this.sgramRowSize = 0
        const buffer = Buffer.allocUnsafe(sgramBuffer.length)
        sgramBuffer.copy(buffer)
        this.push(buffer)
      }
      this.pcmQueue = this.pcmQueue.slice(fftHopBytes)
    }
    cb()
  }

  _flush() {
    this.push(this.sgramBuffer.slice(0, this.sgramRowSize * this.sgramColBytes))
    this.sgramRowSize = 0
    this.pcmQueue = this.pcmQueue.slice(0)
    this.push(null)
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
