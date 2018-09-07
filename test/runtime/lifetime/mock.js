var EventEmitter = require('events')

module.exports.eventBus = new EventEmitter()

module.exports.getMockAppExecutors = getMockAppExecutors
/**
 *
 * @param {number} number
 * @param {boolean} daemon
 * @param {number} [startIdx]
 */
function getMockAppExecutors (number, daemon, startIdx) {
  var ret = {}
  var bus = module.exports.eventBus
  if (startIdx == null) {
    startIdx = 0
  }
  for (var idx = startIdx; idx < number + startIdx; ++idx) {
    var executor = {
      appId: `${idx}`,
      daemon: daemon,
      create: function create () {
        var app = new EventEmitter()
        bus.emit('create', this.appId, app, daemon)
        return Promise.resolve(app)
      },
      destruct: function destruct () {
        bus.emit('destruct', this.appId, daemon)
        return Promise.resolve()
      }
    }
    ret[executor.appId] = executor
  }
  return ret
}

module.exports.restore = function restore () {
  module.exports.eventBus.removeAllListeners()
  module.exports.eventBus = new EventEmitter()
}