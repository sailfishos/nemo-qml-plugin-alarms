/*
 * Copyright (C) 2012 Jolla Ltd.
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

#include "alarmsbackendmodel_p.h"
#include "alarmobject.h"
#include "interface.h"
#include <QDBusMessage>
#include <QDBusReply>
#include <QQmlEngine>
#include <algorithm>

inline static bool alarmSort(AlarmObject *a1, AlarmObject *a2)
{
    if (a1->hour() < a2->hour())
        return true;
    else if (a1->hour() > a2->hour())
        return false;
    else if (a1->minute() < a2->minute())
        return true;
    else if (a1->minute() > a2->minute())
        return false;
    
    if (!a1->daysOfWeek().isEmpty() || !a2->daysOfWeek().isEmpty()) {
        int re = a1->daysOfWeek().compare(a2->daysOfWeek());
        if (re != 0)
            return re < 0;
    }

    int re = a1->title().compare(a2->title());
    if (re == 0)
        re = (a1->createdDate() < a2->createdDate()) ? -1 : 0;

    return re < 0;
}

AlarmsBackendModelPriv::AlarmsBackendModelPriv(AlarmsBackendModel *m)
    : QObject(m), q(m), populated(false), countdown(false)
{
    connect(TimedInterface::instance(), SIGNAL(alarmTriggersChanged(QMap<quint32,quint32>)),
            this, SLOT(alarmTriggersChanged(QMap<quint32,quint32>)));
}

void AlarmsBackendModelPriv::populate()
{
    // Retrieve a list of cookies created by nemoalarms
    QMap<QString,QVariant> attributes;
    attributes.insert(QLatin1String("APPLICATION"), "nemoalarms");
    if (countdown)
        attributes.insert(QLatin1String("type"), "countdown");
    else
        attributes.insert(QLatin1String("type"), "clock");

    QDBusPendingCallWatcher *reply = new QDBusPendingCallWatcher(TimedInterface::instance()->query_async(attributes), this);
    connect(reply, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(queryReply(QDBusPendingCallWatcher*)));
}

void AlarmsBackendModelPriv::queryReply(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QVariantList> reply = *call;
    call->deleteLater();

    if (reply.isError()) {
        qWarning() << "Nemo.Alarms: Timed query failed:" << reply.error();
        return;
    }

    qDBusRegisterMetaType< QList<uint> >();

    QList<uint> cookies;
    foreach (QVariant v, reply.value())
        cookies.append(v.toUInt());

    // Get a list of attributes for each of those cookies
    QDBusPendingCall call2 = TimedInterface::instance()->get_attributes_by_cookies_async(cookies);
    QDBusPendingCallWatcher *reply2 = new QDBusPendingCallWatcher(call2, this);
    connect(reply2, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(attributesReply(QDBusPendingCallWatcher*)));
}

void AlarmsBackendModelPriv::attributesReply(QDBusPendingCallWatcher *call)
{
    // Q_FOREACH can't handle types containing commas
    typedef QMap<QString,QString> attributes;

    QDBusPendingReply<QMap<uint, attributes> > reply = *call;
    call->deleteLater();

    if (reply.isError()) {
        qWarning() << "Nemo.Alarms: Timed attributes query failed:" << reply.error();
        return;
    }

    q->beginResetModel();
    qDeleteAll(alarms);
    alarms.clear();

    foreach (const attributes &data, reply.value()) {
        AlarmObject *alarm = new AlarmObject(data, this);
        connect(alarm, SIGNAL(updated()), SLOT(alarmUpdated()));
        connect(alarm, SIGNAL(deleted()), SLOT(alarmDeleted()));
        alarms.append(alarm);
    }

    std::sort(alarms.begin(), alarms.end(), alarmSort);

    q->endResetModel();

    if (!populated) {
        populated = true;
        emit q->populatedChanged();
    }
}

void AlarmsBackendModelPriv::alarmTriggersChanged(QMap<quint32, quint32> triggerMap)
{
    foreach (AlarmObject *alarm, alarms) {
        if (!triggerMap.contains(alarm->id())) {
            // Extra enabling logic is needed for not resetting alarms that were not active
            if (alarm->isEnabled()) {
                alarm->setEnabled(false);
                alarm->reset();
            }
        } else if (!alarm->isCountdown()) {
            alarm->setEnabled(true);
        }
    }
}

void AlarmsBackendModelPriv::alarmUpdated()
{
    AlarmObject *alarm = qobject_cast<AlarmObject*>(sender());
    if (alarm)
        alarmUpdated(alarm);
}

void AlarmsBackendModelPriv::alarmUpdated(AlarmObject *alarm)
{
    int currentRow = alarms.indexOf(alarm);

    // std::lower_bound expects that the list is sorted, we do not know if that is the case after
    // the alarm has changed. Remove it temporarily from the list while calculating new row.
    if (currentRow >= 0)
        alarms.removeAt(currentRow);

    QList<AlarmObject*>::iterator it = std::lower_bound(alarms.begin(), alarms.end(), alarm, alarmSort);
    int newRow = it - alarms.begin();

    if (currentRow >= 0)
        alarms.insert(currentRow, alarm);

    if (currentRow < 0) {
        alarm->setParent(this);
        QQmlEngine::setObjectOwnership(alarm, QQmlEngine::CppOwnership);

        q->beginInsertRows(QModelIndex(), newRow, newRow);
        alarms.insert(newRow, alarm);
        q->endInsertRows();
        return;
    } else if (newRow != currentRow) {
        q->beginMoveRows(QModelIndex(), currentRow, currentRow, QModelIndex(), newRow > currentRow ? newRow + 1 : newRow);
        alarms.move(currentRow, newRow);
        q->endMoveRows();
    } else
        emit q->dataChanged(q->index(currentRow, 0), q->index(currentRow, 0));
}

void AlarmsBackendModelPriv::alarmDeleted()
{
    AlarmObject *alarm = qobject_cast<AlarmObject*>(sender());
    if (alarm)
        alarmDeleted(alarm);
}

void AlarmsBackendModelPriv::alarmDeleted(AlarmObject *alarm)
{
    int row = alarms.indexOf(alarm);
    if (row >= 0) {
        q->beginRemoveRows(QModelIndex(), row, row);
        alarms.removeAt(row);
        q->endRemoveRows();
    }

    alarm->deleteLater();
}

void AlarmsBackendModelPriv::reset()
{
    foreach (AlarmObject *alarm, alarms) {
        if (alarm->type() == AlarmObject::Countdown) {
            alarm->setEnabled(false);
            alarm->reset();
            alarm->save();
        }
    }
}
