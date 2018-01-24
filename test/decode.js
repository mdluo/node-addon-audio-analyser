const fs = require('fs')
const { Readable } = require('stream')

const { Decoder } = require('lame')
const Analyser = require('../lib/Analyser')

const stream = new Readable()
stream._read = () => {}

const decoder = new Decoder({ float: true })

const analyser = new Analyser({
  magnitude: true,
})

stream.pipe(decoder).pipe(analyser)

for (let i = 2; i <= 7; i += 1) {
  fs.readFile(`${__dirname}/fixtures/chunk_0${i}.mp3`, (err, data) => {
    stream.push(Buffer.from(data))
  })
}

analyser.on('data', (buffer) => {
  console.log(new Float32Array(buffer.buffer))
})
