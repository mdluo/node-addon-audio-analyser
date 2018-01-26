const fs = require('fs')
const { Readable } = require('stream')

const { Decoder } = require('lame')
const Analyser = require('../lib/Analyser')

const stream = new Readable()
stream._read = () => {}

const decoder = new Decoder({ float: true })

const FFT_SIZE = 512

const analyser = new Analyser({
  fftSize: FFT_SIZE,
  hopSize: FFT_SIZE / 2,
  bufferSize: 4,
  magnitude: true,
  smoothingTimeConstant: 0,
})

stream.pipe(decoder).pipe(analyser)

for (let i = 2; i <= 7; i += 1) {
  fs.readFile(`${__dirname}/fixtures/chunk_0${i}.mp3`, (err, data) => {
    stream.push(Buffer.from(data))
  })
}

let count = 0
analyser.on('data', (buffer) => {
  if (count === 5) {
    fs.writeFileSync('/Users/mdluo/Desktop/sgram.bin', buffer)
  }
  count += 1
})
