package com.example.ubiformskeletonkey

import android.app.Service
import android.content.Intent
import android.os.Binder
import android.os.IBinder
import android.service.notification.NotificationListenerService
import android.service.notification.StatusBarNotification
import android.util.Log

class NotificationPublisher : NotificationListenerService() {
    override fun onNotificationPosted(sbn: StatusBarNotification?) {
        Log.d("NOTIFICATION", sbn.toString())
    }

    override fun onListenerConnected() {
        super.onListenerConnected()
        Log.d("NOTIFICATION", "LISTENER CONNECTED")
        for(x in activeNotifications){
            Log.d("NOTIFICATION", x.key)
        }
    }
}