// Check that OCSP verification works
// @tags: [requires_http_client]

load("jstests/ocsp/lib/mock_ocsp.js");

(function() {
"use strict";

let mock_ocsp = new MockOCSPServer();
mock_ocsp.start();

const ocsp_options = {
    sslMode: "requireSSL",
    sslPEMKeyFile: OCSP_SERVER_CERT,
    sslCAFile: OCSP_CA_CERT,
    setParameter: {
        ocspEnabled: "true",
    },
    sslAllowInvalidHostnames: "",
};

let conn = null;
assert.doesNotThrow(() => {
    conn = MongoRunner.runMongod(ocsp_options);
});

mock_ocsp.stop();

// Test Scenario when Mock OCSP Server replies stating
// that the OCSP status of the client cert is revoked.
mock_ocsp = new MockOCSPServer(FAULT_REVOKED);
mock_ocsp.start();
assert.throws(() => {
    new Mongo(conn.host);
});

mock_ocsp.stop();
MongoRunner.stopMongod(conn);
}());