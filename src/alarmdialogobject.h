/*
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: John Brooks <john.brooks@jollamobile.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#ifndef ALARMDIALOGOBJECT_H
#define ALARMDIALOGOBJECT_H

#include "alarmobject.h"

namespace Maemo {
    namespace Timed {
        namespace Voland {
            class Reminder;
        }
    }
}

class QDBusPendingCallWatcher;

class AlarmDialogObject : public AlarmObject
{
    Q_OBJECT
    Q_PROPERTY(bool hideSnoozeButton READ hideSnoozeButton CONSTANT)
    Q_PROPERTY(bool hideDismissButton READ hideDismissButton CONSTANT)
    Q_PROPERTY(bool isMissed READ isMissed CONSTANT)

    friend class AlarmHandlerInterface;

public:
    AlarmDialogObject(QObject *parent = 0);
    AlarmDialogObject(const Maemo::Timed::Voland::Reminder &data, QObject *parent = 0);

    bool hideSnoozeButton() const { return m_hideSnooze; }

    bool hideDismissButton() const { return m_hideDismiss; }

    bool isMissed() const { return m_missed; }

    Q_INVOKABLE void snooze();
    Q_INVOKABLE void dismiss();
    Q_INVOKABLE void close();

    void closedExternally();

signals:
    void closed(QObject *alarm);

private slots:
    void responseReply(QDBusPendingCallWatcher *w);

private:
    bool m_hideSnooze, m_hideDismiss, m_missed;

    void sendResponse(int code);
};

#endif

