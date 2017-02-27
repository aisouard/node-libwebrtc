/*
 * Copyright (c) 2017 Axel Isouard <axel@isouard.fr>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

'use strict';

const chai = require('chai');
const assert = chai.assert;
const RTCCertificate = require('../').RTCCertificate;
const RTCPeerConnection = require('../').RTCPeerConnection;

describe('RTCCertificate', () => {
  describe('generated using RSA algorithm', () => {
    let certificate;
    const now = (+ new Date());

    before((done) => {
      RTCPeerConnection.generateCertificate({
        name: 'RSASSA-PKCS1-v1_5',
        hash: 'SHA-256',
        modulusLength: 1024,
        publicExponent: new Uint8Array([1, 0, 1])
      }).then((cert) => {
        certificate = cert;
        done();
      });
    });

    describe('expire property', () => {
      it('should be 30 days higher than now', () => {
        const expected =
          Math.floor((now + (1000 * 60 * 60 * 24 * 30)) / 1000) * 1000;

        assert.equal(certificate.expires, expected);
      });
    });

    describe('fingerprints property', () => {
      it('should be an instance of ')
    });
  });
});
