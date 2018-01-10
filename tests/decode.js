const fs = require('fs')
const { Readable } = require('stream')

const { Decoder } = require('lame')
const Analyser = require('../lib/Analyser')

const stream = new Readable()
stream._read = () => {}

const decoder = new Decoder({ float: true })

const analyser = new Analyser()

const output = fs.createWriteStream('/dev/null')

stream.pipe(decoder).pipe(analyser).pipe(output)

for (let i = 2; i <= 7; i += 1) {
  fs.readFile(`${__dirname}/fixtures/chunk_0${i}.mp3`, (err, data) => {
    stream.push(Buffer.from(data))
  })
}

const arr = new Float32Array(2048)

setInterval(() => {
  analyser.getFloatFrequencyData(arr)
}, 20)
