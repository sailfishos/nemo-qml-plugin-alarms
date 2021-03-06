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

#ifndef ALARMSBACKENDMODEL_H
#define ALARMSBACKENDMODEL_H

#include <QAbstractListModel>
#include <QQmlParserStatus>

class AlarmsBackendModelPriv;
class AlarmObject;

class AlarmsBackendModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(bool populated READ isPopulated NOTIFY populatedChanged)
    Q_PROPERTY(bool onlyCountdown READ isOnlyCountdown WRITE setOnlyCountdown NOTIFY onlyCountdownChanged)

public:
    enum {
        AlarmObjectRole = Qt::UserRole,
        EnabledRole,
        HourRole,
        MinuteRole,
        SecondRole,
        WeekDaysRole
    };

    AlarmsBackendModel(QObject *parent = 0);
    virtual ~AlarmsBackendModel();

    Q_INVOKABLE AlarmObject *createAlarm();
    bool isPopulated() const;

    bool isOnlyCountdown() const;
    void setOnlyCountdown(bool countdown);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    void classBegin();
    void componentComplete();

    Q_INVOKABLE void reset();

signals:
    void populatedChanged();
    void onlyCountdownChanged();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    friend class AlarmsBackendModelPriv;
    AlarmsBackendModelPriv *priv;
    bool completed;
};

#endif // ALARMSBACKENDMODEL_H
