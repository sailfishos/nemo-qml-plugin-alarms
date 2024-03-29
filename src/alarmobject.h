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

#ifndef ALARMOBJECT_H
#define ALARMOBJECT_H

#include <QObject>
#include <QDateTime>
#include <QMap>

class AlarmPrivate;
class QDBusPendingCallWatcher;

class AlarmObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int hour READ hour WRITE setHour NOTIFY timeChanged)
    Q_PROPERTY(int minute READ minute WRITE setMinute NOTIFY timeChanged)
    Q_PROPERTY(int second READ second WRITE setSecond NOTIFY timeChanged)
    Q_PROPERTY(QString daysOfWeek READ daysOfWeek WRITE setDaysOfWeek NOTIFY daysOfWeekChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(QDateTime createdDate READ createdDate CONSTANT)
    Q_PROPERTY(bool countdown READ isCountdown WRITE setCountdown NOTIFY countdownChanged)
    Q_PROPERTY(uint triggerTime READ triggerTime NOTIFY triggerTimeChanged)
    Q_PROPERTY(int elapsed READ getElapsed NOTIFY elapsedChanged)
    Q_PROPERTY(int type READ type NOTIFY typeChanged)
    Q_PROPERTY(QDateTime startDate READ startDate CONSTANT)
    Q_PROPERTY(QDateTime endDate READ endDate CONSTANT)
    Q_PROPERTY(bool allDay READ allDay CONSTANT)
    Q_PROPERTY(QString calendarUid READ calendarUid CONSTANT)
    Q_PROPERTY(QString calendarEventUid READ calendarEventUid CONSTANT)
    Q_PROPERTY(QString notebookUid READ notebookUid CONSTANT)
    Q_PROPERTY(QString calendarEventRecurrenceId READ calendarEventRecurrenceId CONSTANT)
    Q_PROPERTY(QString phoneNumber READ phoneNumber CONSTANT)
    Q_PROPERTY(int timeoutSnoozeCounter READ timeoutSnoozeCounter CONSTANT)
    Q_PROPERTY(int maximalTimeoutSnoozeCount READ maximalTimeoutSnoozeCount WRITE setMaximalTimeoutSnoozeCount NOTIFY maximalTimeoutSnoozeCountChanged)

public:
    AlarmObject(QObject *parent = 0);
    AlarmObject(const QMap<QString,QString> &data, QObject *parent = 0);

    enum Type { Calendar, Clock, Countdown, Reminder };
    Q_ENUMS(Type)

    QString title() const { return m_title; }
    void setTitle(const QString &title);

    int hour() const { return m_hour; }
    void setHour(int hour);

    int minute() const { return m_minute; }
    void setMinute(int minute);

    int second() const { return m_second; }
    void setSecond(int second);

    QString daysOfWeek() const { return m_daysOfWeek; }
    void setDaysOfWeek(const QString &days);

    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled);

    int id() const { return static_cast<int>(m_cookie); }

    QDateTime createdDate() const { return m_createdDate; }

    bool isCountdown() const { return m_countdown; }
    void setCountdown(bool countdown);

    uint triggerTime() const { return m_triggerTime; }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    qint64 getElapsed() const { return m_elapsed; }
#else
    int getElapsed() const { return m_elapsed; }
#endif

    int type() const;

    QDateTime startDate() const;

    QDateTime endDate() const;

    bool allDay() const;

    QString calendarUid() const;

    QString calendarEventUid() const;

    QString notebookUid() const;

    QString calendarEventRecurrenceId() const;

    QString phoneNumber() const { return m_phoneNumber; }

    int timeoutSnoozeCounter() const { return static_cast<int>(m_timeoutSnoozeCounter); }

    int maximalTimeoutSnoozeCount() const;
    void setMaximalTimeoutSnoozeCount(int count);

    Q_INVOKABLE void reset();
    Q_INVOKABLE void save();
    Q_INVOKABLE void deleteAlarm();

signals:
    void titleChanged();
    void timeChanged();
    void daysOfWeekChanged();
    void enabledChanged();
    void idChanged();
    void countdownChanged();
    void triggerTimeChanged();
    void elapsedChanged();
    void typeChanged();
    void maximalTimeoutSnoozeCountChanged();

    void updated();
    void saved();
    void deleted();

private slots:
    void saveReply(QDBusPendingCallWatcher *w);
    void deleteReply(QDBusPendingCallWatcher *w);

protected:
    QString m_title;
    int m_hour, m_minute, m_second;
    QString m_daysOfWeek;
    bool m_enabled;
    QDateTime m_createdDate;
    bool m_countdown;
    bool m_reminder;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    qint64 m_triggerTime;
    qint64 m_elapsed;
#else
    uint m_triggerTime;
    uint m_elapsed;
#endif
    QDateTime m_startDate, m_endDate;
    QString m_uid;
    QString m_recurrenceId;
    QString m_notebookUid;
    QString m_phoneNumber;

    // Timed
    unsigned m_cookie;
    unsigned m_timeoutSnoozeCounter;
    int m_maximalTimeoutSnoozeCount;
};

#endif

