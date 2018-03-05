'use strict';

const EventEmitter = require('events').EventEmitter;
const webrtc = require('bindings')('webrtc');

function inherit(cl,parent) {
    for( var k in parent.prototype ) {
        cl.prototype[k] = parent.prototype[k];
    }
}

var EventTarget = function(){};
inherit(EventTarget, EventEmitter);
EventTarget.prototype.addEventListener = EventEmitter.prototype.on;

[webrtc.RTCPeerConnection, webrtc.RTCDataChannel].map((cl)=>{
    inherit(cl, EventTarget);
});

module.exports = webrtc;