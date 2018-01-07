function extend(target, ...sources) {
  const { hasOwnProperty } = Object.prototype
  Array.prototype.forEach.call(sources, (source) => {
    for (const key in source) {
      if (hasOwnProperty.call(source, key)) {
        target[key] = source[key]
      }
    }
  })
  return target
}

function isPowerOfTwo(n) {
  return n && (n & (n - 1)) === 0
}

module.exports = {
  extend,
  isPowerOfTwo,
}
