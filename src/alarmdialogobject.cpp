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

#include "alarmdialogobject.h"
#include "interface.h"
#include <timed-voland-qt5/reminder>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

/*!
 *  \qmlproperty AlarmDialog::hideSnoozeButton
 *
 *  Flag indicating that the snooze button should not be displayed for this dialog
 */

/*!
 *  \qmlproperty AlarmDialog::hideDismissButton
 *
 *  Flag indicating that the dismiss button should not be displayed for this dialog
 */

/*!
 *  \qmlproperty AlarmDialog::isMissed
 *
 *  Flag indicating that the time for this alarm was missed and fired late
 */

/*!
 *  \qmlsignal void closed(AlarmDialog alarm)
 *
 *  Emitted when the dialog has been closed, either by sending a response or from
 *  an external action or timeout. UI should be closed in response.
 */

AlarmDialogObject::AlarmDialogObject(QObject *parent)
    : AlarmObject(parent), m_hideSnooze(false), m_hideDismiss(false), m_missed(false)
{
}

AlarmDialogObject::AlarmDialogObject(const Maemo::Timed::Voland::Reminder &data, QObject *parent)
    : AlarmObject(data.attributes(), parent),
      m_hideSnooze(data.hideSnoozeButton1()),
      m_hideDismiss(data.hideCancelButton2()),
      m_missed(data.isMissed())
{
    // Reminders mysteriously do not contain the 'COOKIE' attribute. Set it here.
    m_cookie = data.cookie();
}

/*!
 *  \qmlmethod void AlarmDialog::snooze()
 *
 *  Snooze the alarm for the system's default snooze duration. The alarm will
 *  trigger again afterwards.
 */
void AlarmDialogObject::snooze()
{
    sendResponse(-1);
}

/*!
 *  \qmlmethod void AlarmDialog::dismiss()
 *
 *  Dismiss the alarm. Repeatable alarms will be triggered again on the next repeat
 *  day, and non-repeatable alarms will be disabled.
 */
void AlarmDialogObject::dismiss()
{
    sendResponse(-2);
}

/*!
 *  \qmlmethod void AlarmDialog::close()
 *
 *  Close the alarm dialog and use the default action for the alarm. This usually happens
 *  as a result of no interaction from the user.
 */
void AlarmDialogObject::close()
{
    sendResponse(0);
}

void AlarmDialogObject::closedExternally()
{
    // Closed by external forces, don't send a response, just emit
    emit closed(this);
}

void AlarmDialogObject::sendResponse(int code)
{
    QDBusPendingCall call = TimedInterface::instance()->dialog_response_async(id(), code);
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(call, this);
    connect(w, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(responseReply(QDBusPendingCallWatcher*)));

    // Close dialog
    emit closed(this);
}

void AlarmDialogObject::responseReply(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<bool> reply = *w;
    w->deleteLater();

    if (reply.isError())
        qWarning() << "Nemo.Alarms: Error from sending alarm dialog response:" << reply.error();
}

