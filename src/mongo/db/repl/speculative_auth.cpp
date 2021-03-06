/**
 *    Copyright (C) 2020-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#include "mongo/db/repl/speculative_auth.h"

#include "mongo/client/authenticate.h"
#include "mongo/db/auth/sasl_command_constants.h"
#include "mongo/db/auth/sasl_commands.h"
#include "mongo/db/commands/authentication_commands.h"

namespace mongo {

void handleIsMasterSpeculativeAuth(OperationContext* opCtx,
                                   BSONObj cmdObj,
                                   BSONObjBuilder* result) {
    auto sae = cmdObj[auth::kSpeculativeAuthenticate];
    if (sae.eoo()) {
        return;
    }

    uassert(ErrorCodes::BadValue,
            str::stream() << "isMaster." << auth::kSpeculativeAuthenticate << " must be an Object",
            sae.type() == Object);
    auto specAuth = sae.Obj();

    uassert(ErrorCodes::BadValue,
            str::stream() << "isMaster." << auth::kSpeculativeAuthenticate
                          << " must be a non-empty Object",
            !specAuth.isEmpty());
    auto specCmd = specAuth.firstElementFieldNameStringData();

    if (specCmd == saslStartCommandName) {
        doSpeculativeSaslStart(opCtx, specAuth, result);
    } else if (specCmd == auth::kAuthenticateCommand) {
        doSpeculativeAuthenticate(opCtx, specAuth, result);
    } else {
        uasserted(51769,
                  str::stream() << "isMaster." << auth::kSpeculativeAuthenticate
                                << " unknown command: " << specCmd);
    }
}

}  // namespace mongo
