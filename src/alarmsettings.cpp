/*
 * Copyright (C) 2019 Jolla Ltd.
 * Contact: Slava Monich <slava.monich@jolla.com>
 *
 * You may use this file under the terms of BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the name of Jolla Ltd nor the names of its contributors may
 *      be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * any official policies, either expressed or implied.
 */

#include "alarmsettings.h"
#include "time_interface.h"

#include <QDebug>
#include <QDBusReply>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>

#define TIMED_CONNECTION       QDBusConnection::systemBus()
#define TIMED_SERVICE          "com.nokia.time"
#define TIMED_PATH             "/com/nokia/time"
#define CLOCK_APP              "nemoalarms"

AlarmSettings::AlarmSettings(QObject* parent) :
    QObject(parent),
    m_snooze(-1),
    m_ready(false),
    m_daemon(new TimeDaemon(TIMED_SERVICE, TIMED_PATH, TIMED_CONNECTION, this))
{
    connect(new QDBusPendingCallWatcher(
        m_daemon->get_app_snooze(CLOCK_APP), m_daemon),
        &QDBusPendingCallWatcher::finished, this, &AlarmSettings::onSnoozeFinished);
}

int AlarmSettings::snooze() const
{
    return m_snooze;
}

bool AlarmSettings::ready() const
{
    return m_ready;
}

void AlarmSettings::onSnoozeFinished(QDBusPendingCallWatcher* watcher)
{
    QDBusPendingReply<int> reply = *watcher;
    if (reply.isError()) {
        qWarning() << reply.error();
    } else {
        const int value = reply.value();
        const bool oldReady = m_ready;
        m_ready = true;
        if (m_snooze != value) {
            m_snooze = value;
            emit snoozeChanged();
        }
        if (!oldReady) {
            emit readyChanged();
        }
    }
    watcher->deleteLater();
}

void AlarmSettings::setSnooze(int snooze)
{
    if (snooze < MinimumSnooze && snooze > MaximumSnooze) {
        qWarning() << "Setting of invalid snooze value of" << snooze << "seconds ignored. Snooze needs to be more than 1 minute and less than 30 minutes.";
        return;
    }
    if (m_snooze != snooze) {
        m_snooze = snooze;
        m_daemon->set_app_snooze(CLOCK_APP, m_snooze);
        emit snoozeChanged();
    }
}
