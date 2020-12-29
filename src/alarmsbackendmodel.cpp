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

#include "alarmsbackendmodel.h"
#include "alarmsbackendmodel_p.h"
#include "alarmobject.h"

AlarmsBackendModel::AlarmsBackendModel(QObject *parent)
    : QAbstractListModel(parent), completed(false)
{
    priv = new AlarmsBackendModelPriv(this);
}

AlarmsBackendModel::~AlarmsBackendModel()
{
}

QHash<int, QByteArray> AlarmsBackendModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "title";
    roles[AlarmObjectRole] = "alarm";
    roles[EnabledRole] = "enabled";
    roles[HourRole] = "hour";
    roles[MinuteRole] = "minute";
    roles[SecondRole] = "second";
    roles[WeekDaysRole] = "daysOfWeek";
    return roles;
}

/*!
 *  \qmlmethod Alarm AlarmsModel::createAlarm
 *
 *  Create a new alarm object. After setting properties, call the save()
 *  method of the object to commit it to the backend and model.
 *
 *  If the operation is aborted, call the deleteAlarm() method of the object.
 */
AlarmObject *AlarmsBackendModel::createAlarm()
{
    AlarmObject *alarm = new AlarmObject(this);
    connect(alarm, SIGNAL(updated()), priv, SLOT(alarmUpdated()));
    connect(alarm, SIGNAL(deleted()), priv, SLOT(alarmDeleted()));
    return alarm;
}

/*!
 *  \qmlproperty bool AlarmsModel::populated
 *
 *  True when the model has loaded all available alarms from the backend.
 *  The model may still be empty afterwards.
 */
bool AlarmsBackendModel::isPopulated() const
{
    return priv->populated;
}

bool AlarmsBackendModel::isOnlyCountdown() const
{
    return priv->countdown;
}

void AlarmsBackendModel::setOnlyCountdown(bool countdown)
{
    if (priv->countdown == countdown)
        return;

    priv->countdown = countdown;
    emit onlyCountdownChanged();

    if (completed)
        priv->populate();
}

int AlarmsBackendModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return priv->alarms.size();
}

QVariant AlarmsBackendModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= priv->alarms.size())
        return QVariant();

    AlarmObject *alarm = priv->alarms[index.row()];

    switch (role) {
        case Qt::DisplayRole: return alarm->title();
        case AlarmObjectRole: return QVariant::fromValue<QObject*>(alarm);
        case EnabledRole: return alarm->isEnabled();
        case HourRole: return alarm->hour();
        case MinuteRole: return alarm->minute();
        case SecondRole: return alarm->second();
        case WeekDaysRole: return alarm->daysOfWeek();
    }

    return QVariant();
}

void AlarmsBackendModel::classBegin()
{
}

void AlarmsBackendModel::componentComplete()
{
    priv->populate();
    completed = true;
}

/*!
 *  \qmlmethod void AlarmsModel::reset()
 *
 *  Goes through and resets alarms saved in the model. Active alarms
 *  will be turned off, i.e. enabled set to false.
 *
 *  If the alarm is a countdown alarm, then sets \a elapsed and \a triggerTime to 0.
 *  If the alarm is not a countdown alarm, then does nothing.
 *
 *  \sa Alarm::reset()
 */
void AlarmsBackendModel::reset()
{
    priv->reset();
}

