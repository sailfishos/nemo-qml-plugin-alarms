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

public:
    AlarmObject(QObject *parent = 0);
    AlarmObject(const QMap<QString,QString> &data, QObject *parent = 0);

    enum Type { Calendar, Clock, Countdown };
    Q_ENUMS(Type)

    /*!
     *  \qmlproperty string Alarm::title
     *
     *  User-specified title of the alarm. May be empty.
     */
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    QString title() const { return m_title; }
    void setTitle(const QString &title);

    /*!
     *  \qmlproperty int Alarm::hour
     *  Hour component of the alarm time
     *
     *  An alarm will trigger when the device time is next at the specified
     *  hour and minute, after being enabled. Alarm time is independent of
     *  timezones.
     */
    Q_PROPERTY(int hour READ hour WRITE setHour NOTIFY timeChanged)
    int hour() const { return m_hour; }
    void setHour(int hour);

    /*!
     *  \qmlproperty int Alarm::minute
     *  Minute component of the alarm time
     *
     *  An alarm will trigger when the device time is next at the specified
     *  hour and minute, after being enabled. Alarm time is independent of
     *  timezones.
     */
    Q_PROPERTY(int minute READ minute WRITE setMinute NOTIFY timeChanged)
    int minute() const { return m_minute; }
    void setMinute(int minute);

    /*!
     *  \qmlproperty string Alarm::daysOfWeek
     *  List of weekdays when the alarm will be repeated
     *  
     *  String containing the characters 'mtwTfsS' representing Monday through Sunday.
     *  If non-empty, the alarm will only trigger on these days, and will repeat until
     *  disabled.
     *
     *  By default (when this property is empty), alarms are single-shot: they trigger
     *  at the specified time in the next day, and then are disabled and won't trigger
     *  again unless enabled again.
     */
    Q_PROPERTY(QString daysOfWeek READ daysOfWeek WRITE setDaysOfWeek NOTIFY daysOfWeekChanged)
    QString daysOfWeek() const { return m_daysOfWeek; }
    void setDaysOfWeek(const QString &days);

    /*!
     *  \qmlproperty bool Alarm::enabled
     *
     *  True to enable and schedule the alarm, false when the alarm should not be active.
     */
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled);

    /*!
     *  \qmlproperty int Alarm::id
     *
     *  Internal identifier for the alarm. May change after modifications are saved.
     *  0 is an invalid ID, used for unsaved alarms.
     */
    Q_PROPERTY(int id READ id NOTIFY idChanged)
    int id() const { return static_cast<int>(m_cookie); }

    /*!
     *  \qmlproperty datetime Alarm::createdDate
     *
     *  Date when the alarm was first created
     */
    Q_PROPERTY(QDateTime createdDate READ createdDate CONSTANT)
    QDateTime createdDate() const { return m_createdDate; }

    /*!
     *  \qmlproperty bool Alarm::countdown
     *
     *  True to make the alarm a countdown alarm, which will trigger after
     *  \a hour hours and \a min minutes.
     *
     *  \sa triggerTime
     *  \sa elapsed
     *  \sa reset
     */
    Q_PROPERTY(bool countdown READ isCountdown WRITE setCountdown NOTIFY countdownChanged)
    bool isCountdown() const { return m_countdown; }
    void setCountdown(bool countdown);

    /*!
     *  \qmlproperty bool Alarm::triggerTime
     *
     *  Indicates the trigger time in seconds since Unix epoch. Valid only for
     *  countdown alarms. The remaining time for an countdown alarm can be
     *  calculated \a triggerTime - now - \a elapsed, where now is the current
     *  time expressed as seconds since the Unix epoch.
     *
     *  \sa countdown
     *  \sa elapsed
     */
    Q_PROPERTY(uint triggerTime READ triggerTime NOTIFY triggerTimeChanged)
    uint triggerTime() const { return m_triggerTime; }

    /*!
     *  \qmlproperty bool Alarm::elapsed
     *
     *  Indicates the elapsed time for a countdown alarm in seconds. Valid only for
     *  countdown alarms. When an alarm is not enabled, then \a elapsed indicates how
     *  long the countdown alarm has been running before it was paused. If an alarm is
     *  running, then \a elapsed indicates how long the countdown alarm has been running
     *  before getting enabled.
     *
     *  The \a elapsed property is not updated in realtime when an alarm is enabled, it
     *  simply indicates how long the alarm has been running in the past. See \a triggerTime
     *  about how to calculate the remaining time of a countdown alarm.
     *
     *  \sa countdown
     *  \sa triggerTime
     */
    Q_PROPERTY(int elapsed READ getElapsed NOTIFY elapsedChanged)
    int getElapsed() const { return m_elapsed; }

    /*!
      * \qmlproperty string Alarm::type
      *
      * Indicates the type of the alarm, possible values are Alarm::Clock, Alarm::Countdown,
      * and Alarm::Calendar. Calendar alarms are read only, they cannot be added through this
      * plugin, only read.
      *
      * \sa countdown
      */
    Q_PROPERTY(int type READ type NOTIFY typeChanged)
    int type() const;

    /*!
      * \qmlproperty string Alarm::startDate
      *
      * The start date of an calendar event. Only valid for calendar alarms.
      *
      * \sa type
      */
    Q_PROPERTY(QDateTime startDate READ startDate CONSTANT)
    QDateTime startDate() const;

    /*!
      * \qmlproperty string Alarm::endDate
      *
      * The end date of an calendar event. Only valid for calendar alarms.
      *
      * \sa type
      */
    Q_PROPERTY(QDateTime endDate READ endDate CONSTANT)
    QDateTime endDate() const;

    /*!
      * \qmlproperty string Alarm::allDay
      *
      * Indicates if this alarm represents an all day event. Only valid for calendar alarms.
      *
      * \sa type
      */
    Q_PROPERTY(bool allDay READ allDay CONSTANT)
    bool allDay() const;

    /*!
      * \qmlproperty string Alarm::calendarUid
      *
      * An unique identifier of a calendar event. Only valid for calendar alarms. Deprecated, use calendarEventUid instead.
      *
      * \sa type
      */
    Q_PROPERTY(QString calendarUid READ calendarUid CONSTANT)
    QString calendarUid() const;

    /*!
      * \qmlproperty string Alarm::calendarEventUid
      *
      * An unique identifier of a calendar event. Only valid for calendar alarms.
      *
      * \sa type
      */
    Q_PROPERTY(QString calendarEventUid READ calendarEventUid CONSTANT)
    QString calendarEventUid() const;

    /*!
      * \qmlproperty string Alarm::notebookUid
      *
      * An unique identifier of the notebook that a calendar event belongs to. Only valid for
      * calendar alarms.
      *
      * \sa type
      */
    Q_PROPERTY(QString notebookUid READ notebookUid CONSTANT)
    QString notebookUid() const;

    /*!
      * \qmlproperty string Alarm::calendarEventRecurrenceId
      *
      * Recurrence identifier of a calendar event. Only valid for calendar alarms.
      *
      * \sa type
      */
    Q_PROPERTY(QString calendarEventRecurrenceId READ calendarEventRecurrenceId CONSTANT)
    QString calendarEventRecurrenceId() const;

    /*!
      * \qmlproperty string Alarm::autoSnoozeCounter
      *
      * Indicates how many times this alarm has been snoozed as a result of
      * calling AlarmDialog::close(). This property will reset to 0 if
      * the alarm is snoozed or dismissed by AlarmDialog::snooze() or
      * AlarmDialog::dismiss().
      *
      * Can be used together with Alarm::maximalTimeoutSnoozeCount to
      * determine if an alarm will be snoozed or dismissed when calling
      * AlarmDialog::close()
      *
      * \sa AlarmDialog::close()
      * \sa Alarm::maximalTimeoutSnoozeCount
      */
    Q_PROPERTY(int timeoutSnoozeCounter READ timeoutSnoozeCounter CONSTANT)
    int timeoutSnoozeCounter() const { return static_cast<int>(m_timeoutSnoozeCounter); }

    /*!
      * \qmlproperty string Alarm::maximalTimeoutSnoozeCount
      *
      * Indicates how many times this alarm should be snoozed as a result of calling
      * AlarmDialog::close(). A value of 0 indicates that calling AlarmDialog::close()
      * will dismiss the alarm. A non-zero value n indicates that the alarm will be snoozed
      * when Alarm::autoSnoozeCounter < n, and dismissed when Alarm::autoSnoozeCounter >= n.
      *
      * Default value is 0.
      *
      * \sa AlarmDialog::close()
      * \sa Alarm::autoSnoozeCounter
      */
    Q_PROPERTY(int maximalTimeoutSnoozeCount READ maximalTimeoutSnoozeCount WRITE setMaximalTimeoutSnoozeCount NOTIFY maximalTimeoutSnoozeCountChanged)
    int maximalTimeoutSnoozeCount() const;
    void setMaximalTimeoutSnoozeCount(int count);

    /*!
     *  \qmlmethod void Alarm::reset()
     *
     *  If the alarm is a countdown alarm, then sets \a elapsed and \a triggerTime to 0.
     *  If the alarm is not a countdown alarm, then does nothing.
     *
     *  \sa countdown
     *  \sa triggerTime
     *  \sa elapsed
     */
    Q_INVOKABLE void reset();

    /*!
     *  \qmlmethod void Alarm::save()
     *
     *  Commit changes to the object to the backend. No modifications, including \a enabled,
     *  take effect until this method is called.
     *  
     *  \sa updated, saved
     */
    Q_INVOKABLE void save();

    /*!
     *  \qmlmethod void Alarm::deleteAlarm()
     *
     *  Remove this alarm from the backend.
     */
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

    /*!
     *  \qmlsignal Alarm::updated()
     *
     *  Emitted at the beginning of a save operation, indicating that the data is updated.
     *
     *  \sa saved
     */
    void updated();
    /*!
     *  \qmlsignal Alarm::saved()
     *
     *  Emitted when changes have been recorded to the backend.
     *
     *  \sa updated
     */
    void saved();
    /*!
     *  \qmlsignal Alarm::deleted()
     *
     *  Emitted after a call to deleteAlarm(), indicating that this object should be removed
     *  and no longer used.
     *
     *  \sa deleteAlarm
     */
    void deleted();

private slots:
    void saveReply(QDBusPendingCallWatcher *w);
    void deleteReply(QDBusPendingCallWatcher *w);

protected:
    QString m_title;
    int m_hour, m_minute;
    QString m_daysOfWeek;
    bool m_enabled;
    QDateTime m_createdDate;
    bool m_countdown;
    uint m_triggerTime;
    uint m_elapsed;
    QDateTime m_startDate, m_endDate;
    QString m_uid;
    QString m_recurrenceId;
    QString m_notebookUid;

    // Timed
    unsigned m_cookie;
    unsigned m_timeoutSnoozeCounter;
    int m_maximalTimeoutSnoozeCount;
};

#endif

